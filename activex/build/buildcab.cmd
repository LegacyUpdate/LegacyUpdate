@echo off
cd %~dp0
set build=%1
if "%build%" == "" set build=Debug
path %PATH%;%ProgramFiles%\Resource Hacker

:: Copy files
copy ..\%build%\LegacyUpdateOCX.dll LegacyUpdate.dll

set redist=%ProgramFiles%\Microsoft Visual Studio 10.0\VC\redist
if not exist msvcr100.dll copy "%redist%\x86\Microsoft.VC100.CRT\msvcr100.dll" msvcr100.dll
if not exist mfc100u.dll copy "%redist%\x86\Microsoft.VC100.MFC\mfc100u.dll" mfc100u.dll

:: cab for IE installation
:: makecab /f LegacyUpdate.ddf
:: del setup.inf
:: del setup.rpt

:: exe for manual installation
iexpress /n /q LegacyUpdate.sed
ResourceHacker -open versioninfo.rc -save versioninfo.res -action compile -log CONSOLE
ResourceHacker -script resources.txt
del versioninfo.res
