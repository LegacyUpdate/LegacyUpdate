# [<img src="https://legacyupdate.net/socialbanner.png" alt="Legacy Update">](http://legacyupdate.net/)

Since Windows XP was discontinued in 2014, followed by Windows 7 in 2020, Microsoft’s support for their earlier OSes has significantly dwindled. As XP and earlier don’t officially support modern security improvements, such as the SHA256 hash algorithm required by modern SSL and Authenticode certificates [as of 2019](https://support.microsoft.com/en-us/topic/2019-sha-2-code-signing-support-requirement-for-windows-and-wsus-64d1c82d-31ee-c273-3930-69a4cde8e64f), much of the internet has become inaccessible to these devices. Adding insult to injury, Microsoft [actively removed](https://techcommunity.microsoft.com/t5/windows-it-pro-blog/sha-1-windows-content-to-be-retired-august-3-2020/ba-p/1544373) many downloads for XP and earlier versions in 2020. In effect, working with these OSes is now incredibly difficult.

To address this, [**Legacy Update**](https://legacyupdate.net/) hosts a patched instance of the Windows Update website. This works because the original Windows Update website simply accesses an ActiveX control that connects to the Windows Update service running on your computer.

> *If this website helped you to update your old PCs, please consider [leaving a tip](https://github.com/sponsors/kirb) to help me pay for the server costs. Thank you!*

## The ActiveX Control
This repo hosts LegacyUpdateOCX, an ActiveX control used as a replica of the original one developed by Microsoft for the official Windows Update website. The original version of Legacy Update required using a proxy autoconfiguration file (.pac) and some additional configuration of IE security settings to intercept requests to the **update.microsoft.com** site, because the Microsoft Wuweb.dll control validates that it is being used on the official update.microsoft.com domain. With the custom LegacyUpdateOCX ActiveX control, proxying is no longer required, because we have full control over the validation logic. This also allows adding additional convenient features not possible with JavaScript alone.

### Building
The project can be built on any version of Windows from XP to 11, though if you run a later OS you’ll ideally want to set up an XP VM for testing.

You will need to install:

* [Visual Studio](https://visualstudio.microsoft.com/vs/) - select the following individual components:
	* C++ Windows XP Support for VS 2017 (v141) tools
	* C++ ATL for v141 build tools (x86 & x64)
	* C++ MFC for v141 build tools (x86 & x64)

	If building on XP, use [Visual Studio 2010](https://my.visualstudio.com/Downloads?q=Visual%20Studio%202010&pgroup=) instead (the last version that runs on XP). The solution is compatible with 2010 and up.
* [Visual Studio 2008](https://my.visualstudio.com/Downloads?q=Visual%20Studio%20Express%202008%20with%20Service%20Pack%201&pgroup=) for compiling a build that works on XP RTM and 2000 SP4 - not required if you only want to build for XP SP2 and later
* [NSIS](https://nsis.sourceforge.io/)

In `activex/build/` you will need to download the following additional files:

* A copy of updroots.exe, which you can extract from [this update](http://download.windowsupdate.com/d/msdownload/update/software/secu/2015/03/rvkroots_3f2ce4676450c06f109b5b4e68bec252873ccc21.exe) using 7-Zip or extract.exe.

### Testing
For debugging, if running on XP with IE8, install [Utilu IE Collection](https://www.utilu.com/iecollection/). IE6/IE7 is much more useful for debugging the native code, because of its simplistic single-process model. Visual Studio is able to launch it and directly attach to the process the code is running in.

To configure the debugger:

1. Right click LegacyUpdateOCX in the Solution Explorer &rarr; Properties
2. In the Debugging tab, set the Command field to:
	* For system IE install: `$(ProgramW6432)\Internet Explorer\iexplore.exe`
	* For Utilu IE6 RTM: `$(ProgramW6432)\Utilu IE Collection\IE600\iexplore.exe`
	* For Utilu IE6 SP2: `$(ProgramW6432)\Utilu IE Collection\IE600XPSP2\iexplore.exe`
	* For Utilu IE7: `$(ProgramW6432)\Utilu IE Collection\IE700\iexplore.exe`
3. Set the Command Arguments field to `http://legacyupdate.net/windowsupdate/v6/`, or any other URL you want to use for debugging
4. If running on XP, in the Debugger tab, set Register Output to Yes.
	If running on Vista or later, this will throw a permission denied error due to UAC. You’ll need to manually register the control using `regsvr32 LegacyUpdateOCX.dll` in an administrator command prompt.

## Disclaimer
The existence of this project shouldn’t be taken as an endorsement to continue using unsupported OSes. You should stick to a supported OS such as Windows 10 or 11 (or, try Linux?!). However, this service exists anyway in recognition that using these OSes is sometimes necessary to run legacy hardware/software, or just interesting to play around with.

This project is not affiliated with or endorsed by Microsoft. This software is provided “as is”, without warranty of any kind. We don’t believe anything should go wrong, but please ensure you have backups of any important data anyway.

## License
Licensed under the Apache License, version 2.0. Refer to [LICENSE.md](https://github.com/kirb/LegacyUpdate/blob/main/LICENSE.md).
