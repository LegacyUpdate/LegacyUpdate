@echo off
cd %~dp0
set build=%1
if "%build%" == "" set build=Debug
path %PATH%;%ProgramFiles%\Resource Hacker

:: Copy files
copy ..\%build%\LegacyUpdateOCX.dll LegacyUpdate.dll

set redist=%ProgramFiles%\Microsoft SDKs\Windows\v6.0A\Bootstrapper\Packages
if not exist vcredist_x86.exe copy "%redist%\vcredist_x86\vcredist_x86.exe" vcredist_x86.exe

:: cab for IE installation
:: makecab /f LegacyUpdate.ddf
:: del setup.inf
:: del setup.rpt

:: exe for manual installation
iexpress /n /q LegacyUpdate.sed
ResourceHacker -open versioninfo.rc -save versioninfo.res -action compile -log CONSOLE
ResourceHacker -script resources.txt
del versioninfo.res

:: Highlight in Explorer
start "" explorer /select,LegacyUpdate.exe
