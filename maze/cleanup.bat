@echo off

if exist bin\ (
	echo CLEANUP bin
	rmdir /S /Q bin\
)

if exist release\ (
	echo CLEANUP release
	rmdir /S /Q release\
)