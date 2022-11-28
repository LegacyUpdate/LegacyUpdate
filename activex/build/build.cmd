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

:: Make sure we have the NSIS plugins
if not exist "%ProgramFiles32%\NSIS\Plugins\x86-unicode\INetC.dll" (
	echo NSIS INetC.dll plugin not found. Refer to README.md. >&2
	exit /b 1
)

if not exist "%ProgramFiles32%\NSIS\Plugins\x86-unicode\FindProcDLL.dll" (
	echo NSIS FindProcDLL.dll plugin not found. Refer to README.md. >&2
	exit /b 1
)

:: Find Visual Studio installation
if exist "%ProgramFiles32%\Microsoft Visual Studio\Installer\vswhere.exe" (
	:: Get modern Visual Studio install path
	for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do set VSPath=%%i
	call "!VSPath!\VC\Auxiliary\Build\vcvarsall.bat" x86
	if "%errorlevel%" neq "0" exit /b %errorlevel%
) else if exist "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" (
	:: Visual Studio 2010
	call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat"
	if "%errorlevel%" neq "0" exit /b %errorlevel%
) else (
	echo Visual Studio not found. Refer to README.md. >&2
	exit /b 1
)

msbuild ..\LegacyUpdate.sln /m /p:Configuration=Release /p:Platform=Win32 %*
if "%errorlevel%" neq "0" exit /b %errorlevel%
makensis setup.nsi
if "%errorlevel%" neq "0" exit /b %errorlevel%
