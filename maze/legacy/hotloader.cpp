#pragma once

#include <typeinfo>

#include "utils.cpp"
#include "logger.cpp"

#include "shader.cpp"


#define DIRWATCHER_FAILED_WATCH_DIR_ACTION    logger.fatal("Failed to watch dir '{}', error code '{}'", dir, GetLastError());
#define DIRWATCHER_FAILED_CLOSE_HANDLE_ACTION logger.error("Failed to close directory watching handle, error code '{}'", GetLastError());
#define DIRWATCHER_DEFAULT_CALLBACK_MESSAGE   logger.log("Default DirWatcher callback: action '{}', filename '{}'", (int)type, filename);
#define DIRWATCHER_USE_STD_FUNCTION
#include "../../libs/dirwatcher.hpp"
using namespace ciwh;


struct Lvalue {
	String name;
	const std::type_info& typeinfo;
	void* data = nullptr;
};

struct VariableFolder {
	String name;
	DynArray<Lvalue> variables;
};

#define MAKE_LVALUE(var) MAKE_LVALUE2(#var, var)
#define MAKE_LVALUE2(name, var) Lvalue{name, typeid(var), &var}


struct Hotloader {
	DirWatcher watcher;
	Deque<String> updates;
	std::mutex updateMutex;

	DynArray<VariableFolder> variableFolders;
	DynArray<String> variableFiles;
	DynArray<ShaderProgram*> shaders;

	Hotloader() {
		watcher.setDir(PATH);
		watcher.setRecursive(true);

		watcher.setCallback([&](FileActionType type, const char* filePath) {
			// logger.log("[DirWatcher callback] {}, {}", (int)type, filePath);
			if (type == FileActionType::MODIFIED) {
				updateMutex.lock();
				defer(updateMutex.unlock());

				for (String& str : updates) {
					if (str == filePath) return;
				}

				updates.push_back(String(PATH) + filePath);
			}
		});
		watcher.start();
	}

	void attach(const String& filePath) {
		if (!file_exists(filePath)) {
			logger.fatal("[Hotloader.attach] File '{}' not exist", filePath);
		}
		//IMPROVEMENT get file extension seperatly?

		if (ends_with(filePath, ".variables")) {
			variableFiles.push_back(filePath);
		}  else {
			logger.fatal("[Hotloader.attach] File '{}' format is unsupported", filePath);
		}
	}

	void attach(ShaderProgram& shader) {
		shaders.push_back(&shader);
	}

	void attach(String folderName, Lvalue var) {
		for (VariableFolder& folder : variableFolders) {
			if (folder.name == folderName) {
				for (Lvalue& foldervar : folder.variables) {
					if (var.name == foldervar.name) {
						logger.error("[Hotloader.attach] Variable '{}' already attached to folder '{}'", var.name, folderName);
						return;
					}
				}
				folder.variables.push_back(var);
				return;
			}
		}
		variableFolders.push_back(VariableFolder{folderName, DynArray<Lvalue>{var}});
	}

	bool needsReload() {
		return updates.size() != 0;
	}

	void reload() {
		updateMutex.lock();
		defer(updateMutex.unlock());

		while (!updates.empty()) {
			String filePath = updates.front();
			updates.pop_front();

			if (ends_with(filePath, ".variables")) {                                               //.variables
				for (String& variableFilename : variableFiles) {
					if (variableFilename == filePath) {
						loadVariables(filePath);
						logger.good("[Hotloader.reload] Variables file '{}' reloaded", filePath);
						break;
					}
				}
			} else if (ends_with(filePath, ".frag") || ends_with(filePath, ".vert")) {             //shaders
				DynArray<ShaderProgram*> shadersThatNeedsToBeUpdated;

				if (ends_with(filePath, ".frag")) {
					for (ShaderProgram* shader : shaders) {
						if (shader->fragPath == filePath) {
							shadersThatNeedsToBeUpdated.push_back(shader);
						}
					}
				} else {
					for (ShaderProgram* shader : shaders) {
						if (shader->vertPath == filePath) {
							shadersThatNeedsToBeUpdated.push_back(shader);
						}
					}
				}
				for (ShaderProgram* shader : shadersThatNeedsToBeUpdated) {
					//BUG sometimes crashes with linking error "void main() definition not found" - may be bug in filereading?
					auto temp = compile_shader_program_from_files(shader->vertPath, shader->fragPath);
					if (temp.valid) {
						glDeleteProgram(shader->programID);
						*shader = temp;
						logger.good("[Hotloader.reload] Shader file '{}' reloaded", filePath);
					} else {
						logger.error("[Hotloader.reload] Shader file '{}' failed to reload", filePath);						
					}
				}
			}
		}
	}

	void loadVariables(String filepath) {
		//
		//helping functions
		//
		auto find_next_nonspace_pos = [](const String& str, size_t begin) -> size_t {
			while (begin < str.length() && isspace(str[begin]))
				begin += 1;
			return begin;
		};

		auto find_prev_nonspace_pos = [](const String& str, size_t begin) -> size_t {
			while (isspace(str[begin]))
				begin -= 1;
			return begin;
		};

		String fileData = read_file(filepath) + "\n"; //fix for last line
		
		//remove comments
		while (true) {
			size_t commentBegin = fileData.find("#");
			if (commentBegin == String::npos) break;

			fileData.erase(
				commentBegin,
				fileData.find('\n', commentBegin)-commentBegin
			);
		}

		//process each line
		u32 lineNum   = 1;
		u64 lineBegin = 0;
		u64 lineEnd   = 0;
		VariableFolder* activeFolder = nullptr;
		while (true) {
			defer(lineNum += 1);

			lineEnd = fileData.find('\n', lineBegin);
			defer(lineBegin = lineEnd + 1);

			if (lineEnd == String::npos) break; //eof
			if (lineBegin == lineEnd) continue; //empty line

			lineBegin = find_next_nonspace_pos(fileData, lineBegin);
			auto realLineEnd = find_prev_nonspace_pos(fileData, lineEnd);

			if (lineBegin >= realLineEnd) continue; //empty line

			//
			//folder
			//
			if (fileData[lineBegin] == ':') {
				lineBegin += 2;
				activeFolder = nullptr;
				String folderName(&fileData[lineBegin], &fileData[realLineEnd+1]);

				//find this folder in mananger
				for (auto& folder : variableFolders) {
					if (folder.name == folderName) {
						activeFolder = &folder;
						break;
					}
				}
				if (!activeFolder) { //folder was not found
					logger.error("[Hotloader.loadVariables] Error at {}:{}: folder '{}' was not found", filepath, lineNum, folderName);
					return;
				}
				continue; //go to next line
			}
			
			//
			//variable
			//
			size_t middleSpace = lineBegin;
			while (!isspace(fileData[middleSpace])) {
				middleSpace += 1;
			}


			String varName(&fileData[lineBegin], &fileData[middleSpace]);
			
			if (!activeFolder) {
				logger.error("[Hotloader.loadVariables] Error at {}:{}: variable '{}': folder is unknown", filepath, lineNum, varName);
				return;
			}

			//find this variable in folder
			Lvalue* var = nullptr;
			for (Lvalue& lvalue : activeFolder->variables) {
				if (lvalue.name == varName) {
					var = &lvalue;
					break;
				}
			}
			if (!var) {
				logger.error("[Hotloader.loadVariables] Error at {}:{}: variable '{}' was not found in folder '{}'", filepath, lineNum, varName, activeFolder->name);
				return;
			}

			size_t valBegin = find_next_nonspace_pos(fileData, middleSpace);

			if (valBegin > realLineEnd) {
				logger.error("[Hotloader.loadVariables] Error at {}:{}: cant find value for variable '{}'", filepath, lineNum, varName);
				return;
			}

			auto& typeinfo = var->typeinfo;

			if (typeinfo == typeid(float32)) {                                   //float32
				*(float32*)(var->data) = strtof(&fileData[valBegin], nullptr);
			}
			else if (typeinfo == typeid(float64)) {                              //float64
				*(float64*)(var->data) = strtod(&fileData[valBegin], nullptr);
			}
			else if (typeinfo == typeid(s8)                                      //signed
			      || typeinfo == typeid(s16)
			      || typeinfo == typeid(s32)
			      || typeinfo == typeid(s64)
			) {
				//TODO chech for overflow?
				s64 val = strtoll(&fileData[valBegin], nullptr, 10);
				if      (typeinfo == typeid(s8))  *(s8*) (var->data) = val;
				else if (typeinfo == typeid(s16)) *(s16*)(var->data) = val;
				else if (typeinfo == typeid(s32)) *(s32*)(var->data) = val;
				else if (typeinfo == typeid(s64)) *(s64*)(var->data) = val;
			}
			else if (typeinfo == typeid(u8)                                      //unsigned
			      || typeinfo == typeid(u16)
			      || typeinfo == typeid(u32)
			      || typeinfo == typeid(u64)
			) {
				if (fileData[valBegin] == '-') {
					logger.error("[Hotloader.loadVariables] Error at {}:{}: variable '{}' is unsigned, but given value is negative", filepath, lineNum, varName);
					return;
				}
				u64 val = strtoull(&fileData[valBegin], nullptr, 10);
				if      (typeinfo == typeid(u8))  *(u8*) (var->data) = val;
				else if (typeinfo == typeid(u16)) *(u16*)(var->data) = val;
				else if (typeinfo == typeid(u32)) *(u32*)(var->data) = val;
				else if (typeinfo == typeid(u64)) *(u64*)(var->data) = val;
			}
			else if (typeinfo == typeid(bool)) {                                 //bool
				char c = fileData[valBegin];
				*(bool*)(var->data) = !(c == 'f' || c == '0');
			}
			else if (typeinfo == typeid(String)) {                               //string
				if (realLineEnd == valBegin || fileData[valBegin] != '\"' || fileData[realLineEnd] != '\"') {
					logger.error("[Hotloader.loadVariables] Error at {}:{}: type of variable '{}' is String, but given value is not String", filepath, lineNum, varName);
					return;
				}

				String str(&fileData[valBegin+1], &fileData[realLineEnd]);

				//why macro instead of lambda?
				#define REPLACE(s1, s2)                 \
				{                                       \
					size_t pos = 0;                     \
					while (true) {                      \
						pos = str.find(s1, pos);        \
						if (pos == String::npos) break; \
						str.replace(pos, 2, s2);        \
					}                                   \
				}

				REPLACE("\\n",  "\n");
				REPLACE("\\\"", "\"");
				REPLACE("\\\'", "\'");
				REPLACE("\\\\", "\\");

				#undef REPLACE

				*(String*)(var->data) = str;
			}
			else {
				logger.error("[Hotloader.loadVariables] Error at {}:{}: variable '{}' have unsupported for hotloading type '{}'", filepath, lineNum, varName, typeinfo.name());
				return;
			}
		}
	}
};