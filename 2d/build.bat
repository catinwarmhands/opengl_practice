@echo off

rem Имя итогового исполняемого файла
set name=game

call cleanup
mkdir bin

rem первым параметром можно указать какую версию надо собрать
rem 32, 64, или если не указать, то соберутся обе
set compile32=false
set compile64=false

if "%1"=="" (
	set compile32=true
	set compile64=true
)

if "%1"=="32" set compile32=true
if "%1"=="64" set compile64=true


if %compile32%==true (
	echo COMPILE %name%32
	clang++ ^
	-m32 ^
	src\main.cpp ^
	-L.\libs\glew\lib\Release\Win32 ^
	-L.\libs\glfw32\lib-vc2015 ^
	-lglew32s -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lkernel32 -lmsvcrt ^
	-z /ignore:4098 ^
	-o bin\%name%32.exe 

	if ERRORLEVEL 1 (
		call cleanup
		goto :eof
	)

	echo OK
)

if %compile64%==true (
	echo COMPILE %name%64
	clang++ ^
	-m64 ^
	src\main.cpp ^
	-L.\libs\glew\lib\Release\x64 ^
	-L.\libs\glfw64\lib-vc2015 ^
	-lglew32s -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lkernel32 -lmsvcrt ^
	-z /ignore:4098 ^
	-o bin\%name%64.exe

	if ERRORLEVEL 1 (
		call cleanup
		goto :eof
	)

	echo OK
)