@echo off
setlocal enabledelayedexpansion

set ProgramFiles32=%ProgramFiles%
if "%ProgramFiles(x86)%" neq "" set ProgramFiles32=%ProgramFiles(x86)%

:: Find Visual Studio installation
if exist "%ProgramFiles32%\Microsoft Visual Studio\Installer\vswhere.exe" (
	:: Get modern Visual Studio install path
	for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do set VSPath=%%i
	set "vcvarsall=!VSPath!\VC\Auxiliary\Build\vcvarsall.bat"
	if "%errorlevel%" neq "0" exit /b %errorlevel%
) else if exist "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" (
	:: Visual Studio 2010
	set "vcvarsall=%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat"
	if "%errorlevel%" neq "0" exit /b %errorlevel%
) else (
	echo Visual Studio not found. Refer to README.md. >&2
	exit /b 1
)

endlocal & set "vcvarsall=%vcvarsall%"
call "%vcvarsall%" %* >nul
