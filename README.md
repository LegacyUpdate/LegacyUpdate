# [<img src="https://legacyupdate.net/socialbanner.png" alt="Legacy Update">](http://legacyupdate.net/)

[![wakatime](https://wakatime.com/badge/user/b9fcf8ba-3fce-41a4-a480-d3fe6074a1ad/project/c9516ff1-10b9-41da-82fb-b86b24b0acc8.svg)](https://wakatime.com/badge/user/b9fcf8ba-3fce-41a4-a480-d3fe6074a1ad/project/c9516ff1-10b9-41da-82fb-b86b24b0acc8)
[![Build](https://github.com/kirb/LegacyUpdate/actions/workflows/build.yml/badge.svg)](https://github.com/kirb/LegacyUpdate/actions/workflows/build.yml)

Since Windows XP was discontinued in 2014, followed by Windows 7 in 2020, Microsoft has closed services they depend on, such as Windows Update. There are also design flaws with these earlier versions of Windows Update, which make them difficult to get working on new installations.

Legacy Update can install all relevant updates necessary to fix access to the Windows Update service on unsupported versions of Windows. These versions of Windows may display the error “Windows could not search for new updates: Windows Update encountered an unknown error” with error code **80072EFE**, or may simply never finish checking for updates. Legacy Update identifies the updates your system lacks, and installs them automatically, restoring the Windows Update service to full functionality.

Windows Update provides many optional and recommended updates, in addition to drivers for your system, but Windows XP and 2000 can only install critical security updates through the built-in Automatic Updates feature. **Legacy Update** revives the original Windows Update website - the only way to see and install every update available for your system. Legacy Update also restores access to **Windows Ultimate Extras** on Windows Vista Ultimate.

Legacy Update also restores connectivity to some websites in Internet Explorer, and other programs that use the Windows built-in networking functionality. This includes **Windows Product Activation** on Windows XP and Windows Server 2003, allowing you to activate these versions of Windows online in seconds (a legitimate product key is [still required](https://legacyupdate.net/faq/security)).

Just want to appreciate the nostalgia of the classic Windows Update website? Legacy Update can also be installed on Windows 10 and 11. This works even on versions of these OSes that have removed Internet Explorer. Legacy Update won’t modify your Windows 10 or 11 installation.

> *If this website helped you to update your old PCs, please consider [leaving a tip](https://ko-fi.com/adamdemasi) to help me pay for the server costs. Thank you!*

## Download

Download the latest version from the [Releases](https://github.com/kirb/LegacyUpdate/releases) page of this repo, or from [**legacyupdate.net**](https://legacyupdate.net/).

You can also download the [latest nightly build](https://nightly.link/kirb/LegacyUpdate/workflows/build/main/artifact.zip), based on the current development work. Nightly builds are not guaranteed to be stable, and unlike release builds, require at least Windows 2000 SP4 or Windows XP SP2. You may also need to accept extra SmartScreen and other security warnings since these executables are unique to each build. If you’re not sure what to download, you probably want the [stable release](https://legacyupdate.net/).

## The ActiveX Control

This repo hosts an ActiveX control used as a replica of the original one developed by Microsoft for the official Windows Update website. The original version of Legacy Update required using a proxy autoconfiguration file (.pac) and some additional configuration of Internet Explorer security settings to intercept requests to the **update.microsoft.com** domain, because the Microsoft Wuweb.dll control has safety measures ensuring it can only be used by the official update.microsoft.com domain. With the custom Legacy Update ActiveX control, proxying is no longer required, because we have full control over the validation logic.

This also allows us to extend it with convenient features not possible with JavaScript alone. Particularly, we have “forward-ported” the control to add support for Windows Vista and later, which introduces extra challenges such as User Account Control (UAC) and Internet Explorer’s Protected Mode.

### Building

The project is built on Windows 10/11 with [WSL 2](https://aka.ms/wslinstall). You can test much of the project using your running version of Windows, although for best results you should test using a virtual machine of Windows XP, Windows Vista, and Windows 7. Take snapshots of your VMs - this will make it far easier to test update scenarios.

You will need to install:

* On Windows:
  * [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) - select the following individual components:
    * Desktop development with C++
    * C++ Windows XP Support for VS 2017 (v141) tools
    * C++ ATL for v141 build tools (x86 & x64)
    * C++/CLI support for v141 build tools (14.16)
  * [Visual Studio 2008](https://my.visualstudio.com/Downloads?q=Visual%20Studio%20Express%202008%20with%20Service%20Pack%201&pgroup=) for compiling a build that works on XP RTM and 2000 SP4 - not required if you only want to build for XP SP2 and later
    * You will also need [Visual Studio 2010](https://my.visualstudio.com/Downloads?q=Visual%20Studio%202010&pgroup=), which provides bridging from 2008’s VSBuild system to the modern MSBuild.
* On Linux:
  * [MinGW-w64](https://www.mingw-w64.org/) for i686 and x86_64
  * [NSIS](https://nsis.sourceforge.io/)
  * [UPX](https://upx.github.io/)

Run the following command to install Linux build dependencies. This command specific to Ubuntu - if you use a different distro, you will need to find and install the equivalent packages from your package manager.

```bash
sudo apt install make nsis upx-ucl mingw-w64-i686-dev mingw-w64-x86-64-dev
```

If you use Debian/Ubuntu’s build of NSIS, please note that it is compiled for Pentium II and later, and will fail to launch on Pentium, AMD K6, and other CPUs lacking SSE instructions. If you want to support these CPUs, run `./build/fix-nsis.sh` to patch the NSIS exehead binaries with a build that supports these CPUs.

When opening the solution for the first time in Visual Studio 2022, it will suggest to retarget it against the latest Windows SDK. Cancel this dialog.

### Testing

For debugging, if running on Windows XP with IE8, consider installing [Utilu IE Collection](https://www.utilu.com/iecollection/). IE6/IE7 are more convenient for debugging the native code because of their simplistic single-process model. Visual Studio is able to launch it and directly attach to the process the code is running in. Otherwise, you will need to manually find and attach the debugger to the IE child process hosting the website and ActiveX control.

To configure the debugger:

1. Right-click LegacyUpdate in the Solution Explorer &rarr; Properties
2. In the Debugging tab, set the Command field to:
    * For system IE install: `$(ProgramW6432)\Internet Explorer\iexplore.exe`
    * For Utilu IE6 RTM: `$(ProgramW6432)\Utilu IE Collection\IE600\iexplore.exe`
    * For Utilu IE6 SP2: `$(ProgramW6432)\Utilu IE Collection\IE600XPSP2\iexplore.exe`
    * For Utilu IE7: `$(ProgramW6432)\Utilu IE Collection\IE700\iexplore.exe`
    * For PowerShell: `$(SystemRoot)\System32\WindowsPowerShell\v1.0\powershell.exe` (or SysWOW64 to test the 32-bit build on 64-bit Windows)
3. If using IE, set the Command Arguments field to `http://legacyupdate.net/windowsupdate/v6/`, or any other URL you want to use for debugging
4. In the Debugger tab, set Register Output to Yes. (If running on Windows Vista or later, start Visual Studio as administrator to ensure the rights needed for this.)

You can directly test the ActiveX control using PowerShell:

```powershell
PS> $lu = New-Object -ComObject LegacyUpdate.Control
PS> $lu.CheckControl()
True
```

If set up correctly, `CheckControl()` should return `True`. This is only supported in debug builds - release builds throw `E_ACCESSDENIED`.

## Setup and Launcher

Legacy Update makes use of the [Nullsoft Scriptable Install System](https://nsis.sourceforge.io/) (NSIS) for its setup program. NSIS provides a scripting language we use to install prerequisite updates. We extend NSIS with a custom plugin to provide features that improve the setup user experience.

As of 1.10, Legacy Update includes a “launcher” program, responsible for tasks including launching the website in Internet Explorer, repairing a broken installation of Legacy Update, and continuing setup when the system restarts into Winlogon “setup mode”.

Both are compiled using MinGW-w64 and are written in C to keep things simple.

## Website source code

I haven’t yet open sourced the website. This is because much of it is Microsoft code - just with patches I’ve made to remove Microsoft trademark branding, switch it to the Legacy Update ActiveX control, and make some quality-of-life improvements. It doesn’t feel appropriate to put an open source license on something I don’t own. Since it’s almost entirely client-side HTML and JavaScript, you can still review it by poking around the source on the website. You might find [DebugBar](https://www.debugbar.com/download.php) and [CompanionJS](https://www.my-debugbar.com/wiki/CompanionJS/HomePage) (requires Visual Studio or standalone [Microsoft Script Debugger](https://web.archive.org/web/20131113042519/http://download.microsoft.com/download/7/7/d/77d8df05-6fbc-4718-a319-be14317a6811/scd10en.exe)) useful.

I’ve tinkered with writing a [ground-up replacement](https://twitter.com/hbkirb/status/1584537446716350466) of the Microsoft-developed Windows Update website. I haven’t worked on this in a while, but I’ll open source it when I feel it’s ready enough.

## Disclaimer

The existence of this project shouldn’t be taken as an endorsement to continue using unsupported OSes. You should stick to a supported OS such as Windows 10 or 11 (or, try Linux?!). However, this service exists anyway in recognition that using these OSes is sometimes necessary to run legacy hardware/software, or just interesting to play around with.

This project is not affiliated with or endorsed by Microsoft. This software is provided “as is”, without warranty of any kind. We don’t believe anything should go wrong, but please ensure you have backups of any important data anyway.

## Credits

Legacy Update was started and is primarily developed by [Adam Demasi (@kirb)](https://kirb.me/), with contributions from:

* [Douglas R. Reno (@renodr)](https://github.com/renodr): Build system improvements and other project maintenance. Check out his awesome work on [Linux From Scratch](https://www.linuxfromscratch.org/)!
* [@stdin82](https://github.com/stdin82): Helped us find some patches on the Windows Update server, and other general help on the issue trackerf
* The [Windows Update Restored](https://windowsupdaterestored.com/) team: [@CaptainTER06](https://github.com/CaptainTER06), [@TheOneGoofAli](https://github.com/TheOneGoofAli), [et al.](https://windowsupdaterestored.com/)

We make use of some portions of the Windows SDK, [MinGW-w64](https://www.mingw-w64.org/), and [NSIS](https://nsis.sourceforge.io/).

## License

Licensed under the Apache License, version 2.0. Refer to [LICENSE.md](https://github.com/kirb/LegacyUpdate/blob/main/LICENSE.md).

The repository includes some portions of NSIS, licensed under the [zlib/libpng license](https://nsis.sourceforge.io/License). It also includes a compiled copy of my fork of [NSxfer](https://github.com/kirb/nsis-nsxfer), licensed under the [zlib/libpng license](https://github.com/kirb/nsis-nsxfer/blob/master/LICENSE).
