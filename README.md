# [<img src="https://legacyupdate.net/banner.png" alt="Legacy Update">](http://legacyupdate.net/)

Since Windows XP was discontinued in 2014, followed by Windows 7 in 2020, Microsoft’s support for their earlier OSes has significantly dwindled. As XP and earlier don’t officially support modern security improvements, such as the SHA256 hash algorithm required by modern SSL and Authenticode certificates [as of 2019](https://support.microsoft.com/en-us/topic/2019-sha-2-code-signing-support-requirement-for-windows-and-wsus-64d1c82d-31ee-c273-3930-69a4cde8e64f), much of the internet has become inaccessible to these devices. Adding insult to injury, Microsoft [actively removed](https://techcommunity.microsoft.com/t5/windows-it-pro-blog/sha-1-windows-content-to-be-retired-august-3-2020/ba-p/1544373) many downloads for XP and earlier versions in 2020. In effect, working with these OSes is now incredibly difficult.

To address this, [**Legacy Update**](https://legacyupdate.net/) hosts a patched instance of the Windows Update website. This works because the original Windows Update website simply accesses an ActiveX control that connects to the Windows Update service running on your computer.

> *If this website helped you to update your old PCs, please consider [leaving a tip](https://paypal.me/HashbangProductions) to help me pay for the server costs. Thank you!*

## The ActiveX Control
This repo hosts LegacyUpdateOCX, an ActiveX control used as a replica of the original one developed by Microsoft for the official Windows Update website. The original version of Legacy Update required using a proxy autoconfiguration file (.pac) and some additional configuration of IE security settings to intercept requests to the **update.microsoft.com** site, because the Microsoft Wuweb.dll control validates that it is being used on the official update.microsoft.com domain. With the custom LegacyUpdateOCX ActiveX control, proxying is no longer required, because we have full control over the validation logic. This also allows adding additional convenient features not possible with JavaScript alone.

## Disclaimer
The existence of this project shouldn’t be taken as an endorsement to continue using unsupported OSes. You should stick to a supported OS such as Windows 10 or 11 (or, try Linux?!). However, this service exists anyway in recognition that using these OSes is sometimes necessary to run legacy hardware/software, or just interesting to play around with.

This project is not affiliated with or endorsed by Microsoft. This software is provided “as is”, without warranty of any kind. We don’t believe anything should go wrong, but please ensure you have backups of any important data anyway.

## License
Licensed under the Apache License, version 2.0. Refer to [LICENSE.md](https://github.com/kirb/LegacyUpdate/blob/main/LICENSE.md).
