@echo off

if exist bin\ (
	echo CLEANUP
	rmdir /S /Q bin\
)