@echo off
setlocal enabledelayedexpansion
cd %~dp0\..

:: Clean
call build\getvc.cmd x86
msbuild LegacyUpdate.sln /v:quiet /m /p:Configuration=Release /p:Platform=Win32 /t:clean
if "%errorlevel%" neq "0" exit /b %errorlevel%
msbuild LegacyUpdate.sln /v:quiet /m /p:Configuration=Release /p:Platform=x64 /t:clean
if "%errorlevel%" neq "0" exit /b %errorlevel%

:: Build DLL
msbuild LegacyUpdate.sln /v:minimal /m /p:Configuration=Release /p:Platform=Win32 %*
if "%errorlevel%" neq "0" exit /b %errorlevel%
call build\getvc.cmd x64
msbuild LegacyUpdate.sln /v:minimal /m /p:Configuration=Release /p:Platform=x64 %*
if "%errorlevel%" neq "0" exit /b %errorlevel%
