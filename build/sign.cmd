@echo off
setlocal enabledelayedexpansion

:: Find Visual Studio installation
call %~dp0getvc.cmd x64

:: Sign
signtool sign /n "Hashbang Productions" /tr http://time.certum.pl/ /fd SHA1 /td SHA256 /v %*
signtool sign /n "Hashbang Productions" /tr http://time.certum.pl/ /fd SHA256 /td SHA256 /as /v %*
if "%errorlevel%" neq "0" exit /b %errorlevel%
