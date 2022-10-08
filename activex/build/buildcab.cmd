@echo off
cd %~dp0
set build=%1
if "%build%" == "" set build=Debug
path %PATH%;%ProgramFiles%\Resource Hacker;%ProgramFiles%\7-Zip

:: Copy files
copy ..\%build%\LegacyUpdateOCX.dll LegacyUpdate.dll

set redist=%ProgramFiles%\Microsoft SDKs\Windows\v6.0A\Bootstrapper\Packages
if not exist vcredist_x86.exe copy "%redist%\vcredist_x86\vcredist_x86.exe" vcredist_x86.exe

:: cab for IE installation
:: makecab /f LegacyUpdate.ddf
:: del setup.inf
:: del setup.rpt

:: exe for manual installation
del LegacyUpdate.7z
7z a -mx6 -- LegacyUpdate.7z LegacyUpdate.inf LegacyUpdate.dll vcredist_x86.exe
copy /b 7zSD.sfx + sfx.txt + LegacyUpdate.7z LegacyUpdate.exe

ResourceHacker -open versioninfo.rc -save versioninfo.res -action compile -log CONSOLE
ResourceHacker -script resources.txt
del versioninfo.res

:: Highlight in Explorer
start "" explorer /select,LegacyUpdate.exe
