@echo off

rem Имя итогового исполняемого файла
set name=game

call cleanup
mkdir bin

echo COMPILE %name%32 (-Os)
clang++ ^
-m32 ^
src\main.cpp ^
-L.\libs\glew\lib\Release\Win32 ^
-L.\libs\glfw32\lib-vc2015 ^
-lglew32s -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lkernel32 -lmsvcrt ^
-z /ignore:4098 ^
-Os ^
-o bin\%name%32.exe 

if ERRORLEVEL 1 (
	call cleanup
	goto :eof
)

echo OK


echo COMPILE %name%64 (-Os)
clang++ ^
-m64 ^
src\main.cpp ^
-L.\libs\glew\lib\Release\x64 ^
-L.\libs\glfw64\lib-vc2015 ^
-lglew32s -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lkernel32 -lmsvcrt ^
-z /ignore:4098 ^
-Os ^
-o bin\%name%64.exe

if ERRORLEVEL 1 (
	call cleanup
	goto :eof
)

echo OK


echo COPY to release\ folder
mkdir release

xcopy /e/i/y bin release\bin

if not ERRORLEVEL == 0 (
	call cleanup
	goto :eof
)