@echo off
setlocal enabledelayedexpansion
cd %~dp0

set ProgramFiles32=%ProgramFiles%
if "%ProgramFiles(x86)%" neq "" set ProgramFiles32=%ProgramFiles(x86)%

path %ProgramFiles32%\NSIS\Bin;%path%

:: Make sure we have MakeNSIS
if not exist "%ProgramFiles32%\NSIS\Bin\makensis.exe" (
	echo NSIS not found. Refer to README.md. >&2
	exit /b 1
)

:: Build project
call LegacyUpdate\build.cmd

:: Build NSIS
makensis setup\setup.nsi
if "%errorlevel%" neq "0" exit /b %errorlevel%
