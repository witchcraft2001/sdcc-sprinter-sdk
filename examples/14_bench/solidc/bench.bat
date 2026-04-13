@echo off
echo === Building bench (SOLID C) ===
set PROG=bench
set PREFIX=C:\SRC\%PROG%
set SOLIDC=C:\SOLID
REM SolidC EXE files must be in PATH
REM SOLID C looks for <*.h> in PREFIX\INCLUDE
if not exist %PREFIX%\INCLUDE\stdio.h copy %SOLIDC%\INCLUDE\*.* %PREFIX%\INCLUDE
cls 2
del %PROG%.exe
cc1 -m %PREFIX%\%PROG%
if errorlevel 1 goto error
cc2 %PREFIX%\%PROG%
if errorlevel 1 goto error
as %PREFIX%\%PROG%
if errorlevel 1 goto error
ld %PREFIX%\%PROG%,clib/l/gXMAIN /x
if errorlevel 1 goto error
del bench.rel
del bench.asm
del bench.tmc
echo === Built: bench.exe ===
goto end
:error
echo *** Build error ***
:end
