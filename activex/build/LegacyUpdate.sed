[Version]
Class=IEXPRESS
SEDVersion=3

[Options]
PackagePurpose=InstallApp
ShowInstallProgramWindow=0
HideExtractAnimation=0
UseLongFileName=1
InsideCompressed=1
CAB_FixedSize=0
CAB_ResvCodeSigning=0
RebootMode=I
TargetName=LegacyUpdate.exe
FriendlyName=Legacy Update
AppLaunched=rundll32.exe advpack.dll,LaunchINFSection LegacyUpdate.inf
PostInstallCmd=<None>
AdminQuietInstCmd=LegacyUpdate.inf
UserQuietInstCmd=LegacyUpdate.inf
SourceFiles=SourceFiles

[SourceFiles]
SourceFiles0=.

[SourceFiles0]
LegacyUpdate.inf=
LegacyUpdate.dll=
msvcr100.dll=
mfc100u.dll=
