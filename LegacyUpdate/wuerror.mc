;// Reverse engineered from wuerror.h from Windows SDK 10.0.26100.0

;/***************************************************************************
;*                                                                          *
;*   wuerror.mc --  error code definitions for Windows Update.              *
;*                                                                          *
;*   Copyright (c) Microsoft Corporation. All rights reserved.              *
;*                                                                          *
;***************************************************************************/
;#ifndef _WUERROR_
;#define _WUERROR_

MessageIdTypedef = DWORD
OutputBase    = 16

FacilityNames = (
	WindowsUpdate = 0x024
)

SeverityNames = (
	Success    = 0x0
	Error      = 0x2
)

LanguageNames = (
	English    = 0x0409:MSG0409
)

;///////////////////////////////////////////////////////////////////////////////
;// Windows Update Success Codes
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x0001
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_SERVICE_STOP
Language     = English
Windows Update Agent was stopped successfully.
.

MessageId    = 0x0002
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_SELFUPDATE
Language     = English
Windows Update Agent updated itself.
.

MessageId    = 0x0003
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_UPDATE_ERROR
Language     = English
Operation completed successfully but there were errors applying the updates.
.

MessageId    = 0x0004
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_MARKED_FOR_DISCONNECT
Language     = English
A callback was marked to be disconnected later because the request to disconnect the operation came while a callback was executing.
.

MessageId    = 0x0005
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_REBOOT_REQUIRED
Language     = English
The system must be restarted to complete installation of the update.
.

MessageId    = 0x0006
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_ALREADY_INSTALLED
Language     = English
The update to be installed is already installed on the system.
.

MessageId    = 0x0007
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_ALREADY_UNINSTALLED
Language     = English
The update to be removed is not installed on the system.
.

MessageId    = 0x0008
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_ALREADY_DOWNLOADED
Language     = English
The update to be downloaded has already been downloaded.
.

MessageId    = 0x0009
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_SOME_UPDATES_SKIPPED_ON_BATTERY
Language     = English
The operation completed successfully, but some updates were skipped because the system is running on batteries.
.

MessageId    = 0x000A
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_ALREADY_REVERTED
Language     = English
The update to be reverted is not present on the system.
.

MessageId    = 0x0010
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_SEARCH_CRITERIA_NOT_SUPPORTED
Language     = English
The operation is skipped because the update service does not support the requested search criteria.
.

MessageId    = 0x2015
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_UH_INSTALLSTILLPENDING
Language     = English
The installation operation for the update is still in progress.
.

MessageId    = 0x2016
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_UH_DOWNLOAD_SIZE_CALCULATED
Language     = English
The actual download size has been calculated by the handler.
.

MessageId    = 0x5001
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_SIH_NOOP
Language     = English
No operation was required by the server-initiated healing server response.
.

MessageId    = 0x6001
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_DM_ALREADYDOWNLOADING
Language     = English
The update to be downloaded is already being downloaded.
.

MessageId    = 0x7101
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_METADATA_SKIPPED_BY_ENFORCEMENTMODE
Language     = English
Metadata verification was skipped by enforcement mode.
.

MessageId    = 0x7102
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_METADATA_IGNORED_SIGNATURE_VERIFICATION
Language     = English
A server configuration refresh resulted in metadata signature verification to be ignored.
.

MessageId    = 0x8001
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_SEARCH_LOAD_SHEDDING
Language     = English
Search operation completed successfully but one or more services were shedding load.
.

MessageId    = 0x8002
Facility     = WindowsUpdate
Severity     = Success
SymbolicName = WU_S_AAD_DEVICE_TICKET_NOT_NEEDED
Language     = English
There was no need to retrieve an AAD device ticket.
.

;///////////////////////////////////////////////////////////////////////////////
;// Windows Update Error Codes
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x0001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_SERVICE
Language     = English
Windows Update Agent was unable to provide the service.
.

MessageId    = 0x0002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MAX_CAPACITY_REACHED
Language     = English
The maximum capacity of the service was exceeded.
.

MessageId    = 0x0003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UNKNOWN_ID
Language     = English
An ID cannot be found.
.

MessageId    = 0x0004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NOT_INITIALIZED
Language     = English
The object could not be initialized.
.

MessageId    = 0x0005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_RANGEOVERLAP
Language     = English
The update handler requested a byte range overlapping a previously requested range.
.

MessageId    = 0x0006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_TOOMANYRANGES
Language     = English
The requested number of byte ranges exceeds the maximum number (2^31 - 1).
.

MessageId    = 0x0007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALIDINDEX
Language     = English
The index to a collection was invalid.
.

MessageId    = 0x0008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_ITEMNOTFOUND
Language     = English
The key for the item queried could not be found.
.

MessageId    = 0x0009
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_OPERATIONINPROGRESS
Language     = English
Another conflicting operation was in progress. Some operations such as installation cannot be performed twice simultaneously.
.

MessageId    = 0x000A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_COULDNOTCANCEL
Language     = English
Cancellation of the operation was not allowed.
.

MessageId    = 0x000B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_CALL_CANCELLED
Language     = English
Operation was cancelled.
.

MessageId    = 0x000C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NOOP
Language     = English
No operation was required.
.

MessageId    = 0x000D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_XML_MISSINGDATA
Language     = English
Windows Update Agent could not find required information in the update's XML data.
.

MessageId    = 0x000E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_XML_INVALID
Language     = English
Windows Update Agent found invalid information in the update's XML data.
.

MessageId    = 0x000F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_CYCLE_DETECTED
Language     = English
Circular update relationships were detected in the metadata.
.

MessageId    = 0x0010
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_TOO_DEEP_RELATION
Language     = English
Update relationships too deep to evaluate were evaluated.
.

MessageId    = 0x0011
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_RELATIONSHIP
Language     = English
An invalid update relationship was detected.
.

MessageId    = 0x0012
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REG_VALUE_INVALID
Language     = English
An invalid registry value was read.
.

MessageId    = 0x0013
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DUPLICATE_ITEM
Language     = English
Operation tried to add a duplicate item to a list.
.

MessageId    = 0x0014
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_INSTALL_REQUESTED
Language     = English
Updates requested for install are not installable by caller.
.

MessageId    = 0x0016
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INSTALL_NOT_ALLOWED
Language     = English
Operation tried to install while another installation was in progress or the system was pending a mandatory restart.
.

MessageId    = 0x0017
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NOT_APPLICABLE
Language     = English
Operation was not performed because there are no applicable updates.
.

MessageId    = 0x0018
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_USERTOKEN
Language     = English
Operation failed because a required user token is missing.
.

MessageId    = 0x0019
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EXCLUSIVE_INSTALL_CONFLICT
Language     = English
An exclusive update cannot be installed with other updates at the same time.
.

MessageId    = 0x001A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_POLICY_NOT_SET
Language     = English
A policy value was not set.
.

MessageId    = 0x001B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SELFUPDATE_IN_PROGRESS
Language     = English
The operation could not be performed because the Windows Update Agent is self-updating.
.

MessageId    = 0x001D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_UPDATE
Language     = English
An update contains invalid metadata.
.

MessageId    = 0x001E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SERVICE_STOP
Language     = English
Operation did not complete because the service or system was being shut down.
.

MessageId    = 0x001F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_CONNECTION
Language     = English
Operation did not complete because the network connection was unavailable.
.

MessageId    = 0x0020
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_INTERACTIVE_USER
Language     = English
Operation did not complete because there is no logged-on interactive user.
.

MessageId    = 0x0021
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_TIME_OUT
Language     = English
Operation did not complete because it timed out.
.

MessageId    = 0x0022
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_ALL_UPDATES_FAILED
Language     = English
Operation failed for all the updates.
.

MessageId    = 0x0023
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EULAS_DECLINED
Language     = English
The license terms for all updates were declined.
.

MessageId    = 0x0024
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_UPDATE
Language     = English
There are no updates.
.

MessageId    = 0x0025
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_USER_ACCESS_DISABLED
Language     = English
Group Policy settings prevented access to Windows Update.
.

MessageId    = 0x0026
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_UPDATE_TYPE
Language     = English
The type of update is invalid.
.

MessageId    = 0x0027
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_URL_TOO_LONG
Language     = English
The URL exceeded the maximum length.
.

MessageId    = 0x0028
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UNINSTALL_NOT_ALLOWED
Language     = English
The update could not be uninstalled because the request did not originate from a WSUS server.
.

MessageId    = 0x0029
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_PRODUCT_LICENSE
Language     = English
Search may have missed some updates before there is an unlicensed application on the system.
.

MessageId    = 0x002A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MISSING_HANDLER
Language     = English
A component required to detect applicable updates was missing.
.

MessageId    = 0x002B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_LEGACYSERVER
Language     = English
An operation did not complete because it requires a newer version of server.
.

MessageId    = 0x002C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_BIN_SOURCE_ABSENT
Language     = English
A delta-compressed update could not be installed because it required the source.
.

MessageId    = 0x002D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SOURCE_ABSENT
Language     = English
A full-file update could not be installed because it required the source.
.

MessageId    = 0x002E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WU_DISABLED
Language     = English
Access to an unmanaged server is not allowed.
.

MessageId    = 0x002F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_CALL_CANCELLED_BY_POLICY
Language     = English
Operation did not complete because the DisableWindowsUpdateAccess policy was set.
.

MessageId    = 0x0030
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_PROXY_SERVER
Language     = English
The format of the proxy list was invalid.
.

MessageId    = 0x0031
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_FILE
Language     = English
The file is in the wrong format.
.

MessageId    = 0x0032
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_CRITERIA
Language     = English
The search criteria string was invalid.
.

MessageId    = 0x0033
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EULA_UNAVAILABLE
Language     = English
License terms could not be downloaded.
.

MessageId    = 0x0034
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DOWNLOAD_FAILED
Language     = English
Update failed to download.
.

MessageId    = 0x0035
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UPDATE_NOT_PROCESSED
Language     = English
The update was not processed.
.

MessageId    = 0x0036
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_OPERATION
Language     = English
The object's current state did not allow the operation.
.

MessageId    = 0x0037
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NOT_SUPPORTED
Language     = English
The functionality for the operation is not supported.
.

MessageId    = 0x0038
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WINHTTP_INVALID_FILE
Language     = English
The downloaded file has an unexpected content type.
.

MessageId    = 0x0039
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_TOO_MANY_RESYNC
Language     = English
Agent is asked by server to resync too many times.
.

MessageId    = 0x0040
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_SERVER_CORE_SUPPORT
Language     = English
WUA API method does not run on Server Core installation.
.

MessageId    = 0x0041
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SYSPREP_IN_PROGRESS
Language     = English
Service is not available while sysprep is running.
.

MessageId    = 0x0042
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UNKNOWN_SERVICE
Language     = English
The update service is no longer registered with AU.
.

MessageId    = 0x0043
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_UI_SUPPORT
Language     = English
There is no support for WUA UI.
.

MessageId    = 0x0044
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PER_MACHINE_UPDATE_ACCESS_DENIED
Language     = English
Only administrators can perform this operation on per-machine updates.
.

MessageId    = 0x0045
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UNSUPPORTED_SEARCHSCOPE
Language     = English
A search was attempted with a scope that is not currently supported for this type of search.
.

MessageId    = 0x0046
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_BAD_FILE_URL
Language     = English
The URL does not point to a file.
.

MessageId    = 0x0047
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REVERT_NOT_ALLOWED
Language     = English
The update could not be reverted.
.

MessageId    = 0x0048
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_NOTIFICATION_INFO
Language     = English
The featured update notification info returned by the server is invalid.
.

MessageId    = 0x0049
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_OUTOFRANGE
Language     = English
The data is out of range.
.

MessageId    = 0x004A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_IN_PROGRESS
Language     = English
Windows Update agent operations are not available while OS setup is running.
.

MessageId    = 0x004B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_ORPHANED_DOWNLOAD_JOB
Language     = English
An orphaned downloadjob was found with no active callers.
.

MessageId    = 0x004C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_LOW_BATTERY
Language     = English
An update could not be installed because the system battery power level is too low.
.

MessageId    = 0x004D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INFRASTRUCTUREFILE_INVALID_FORMAT
Language     = English
The downloaded infrastructure file is incorrectly formatted.
.

MessageId    = 0x004E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INFRASTRUCTUREFILE_REQUIRES_SSL
Language     = English
The infrastructure file must be downloaded using strong SSL.
.

MessageId    = 0x004F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_IDLESHUTDOWN_OPCOUNT_DISCOVERY
Language     = English
A discovery call contributed to a non-zero operation count at idle timer shutdown.
.

MessageId    = 0x0050
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_IDLESHUTDOWN_OPCOUNT_SEARCH
Language     = English
A search call contributed to a non-zero operation count at idle timer shutdown.
.

MessageId    = 0x0051
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_IDLESHUTDOWN_OPCOUNT_DOWNLOAD
Language     = English
A download call contributed to a non-zero operation count at idle timer shutdown.
.

MessageId    = 0x0052
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_IDLESHUTDOWN_OPCOUNT_INSTALL
Language     = English
An install call contributed to a non-zero operation count at idle timer shutdown.
.

MessageId    = 0x0053
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_IDLESHUTDOWN_OPCOUNT_OTHER
Language     = English
An unspecified call contributed to a non-zero operation count at idle timer shutdown.
.

MessageId    = 0x0054
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INTERACTIVE_CALL_CANCELLED
Language     = English
An interactive user cancelled this operation, which was started from the Windows Update Agent UI.
.

MessageId    = 0x0055
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_CALL_CANCELLED
Language     = English
Automatic Updates cancelled this operation because it applies to an update that is no longer applicable to this computer.
.

MessageId    = 0x0056
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SYSTEM_UNSUPPORTED
Language     = English
This version or edition of the operating system doesn't support the needed functionality.
.

MessageId    = 0x0057
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NO_SUCH_HANDLER_PLUGIN
Language     = English
The requested update download or install handler, or update applicability expression evaluator, is not provided by this Agent plugin.
.

MessageId    = 0x0058
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_SERIALIZATION_VERSION
Language     = English
The requested serialization version is not supported.
.

MessageId    = 0x0059
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NETWORK_COST_EXCEEDS_POLICY
Language     = English
The current network cost does not meet the conditions set by the network cost policy.
.

MessageId    = 0x005A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_CALL_CANCELLED_BY_HIDE
Language     = English
The call is cancelled because it applies to an update that is hidden (no longer applicable to this computer).
.

MessageId    = 0x005B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_CALL_CANCELLED_BY_INVALID
Language     = English
The call is cancelled because it applies to an update that is invalid (no longer applicable to this computer).
.

MessageId    = 0x005C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_VOLUMEID
Language     = English
The specified volume id is invalid.
.

MessageId    = 0x005D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UNRECOGNIZED_VOLUMEID
Language     = English
The specified volume id is unrecognized by the system.
.

MessageId    = 0x005E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EXTENDEDERROR_NOTSET
Language     = English
The installation extended error code is not specified.
.

MessageId    = 0x005F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EXTENDEDERROR_FAILED
Language     = English
The installation extended error code is set to general fail.
.

MessageId    = 0x0060
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_IDLESHUTDOWN_OPCOUNT_SERVICEREGISTRATION
Language     = English
A service registration call contributed to a non-zero operation count at idle timer shutdown.
.

MessageId    = 0x0061
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_FILETRUST_SHA2SIGNATURE_MISSING
Language     = English
Signature validation of the file fails to find valid SHA2+ signature on MS signed payload.
.

MessageId    = 0x0062
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UPDATE_NOT_APPROVED
Language     = English
The update is not in the servicing approval list.
.

MessageId    = 0x0063
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_CALL_CANCELLED_BY_INTERACTIVE_SEARCH
Language     = English
The search call was cancelled by another interactive search against the same service.
.

MessageId    = 0x0064
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INSTALL_JOB_RESUME_NOT_ALLOWED
Language     = English
Resume of install job not allowed due to another installation in progress.
.

MessageId    = 0x0065
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INSTALL_JOB_NOT_SUSPENDED
Language     = English
Resume of install job not allowed because job is not suspended.
.

MessageId    = 0x0066
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INSTALL_USERCONTEXT_ACCESSDENIED
Language     = English
User context passed to installation from caller with insufficient privileges.
.

MessageId    = 0x0067
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_STANDBY_ACTIVITY_NOT_ALLOWED
Language     = English
Operation is not allowed because the device is in DC (Direct Current) and DS (Disconnected Standby).
.

MessageId    = 0x0068
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_COULD_NOT_EVALUATE_PROPERTY
Language     = English
The property could not be evaluated.
.

MessageId    = 0x0FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UNEXPECTED
Language     = English
An operation failed due to reasons not covered by another error code.
.

;///////////////////////////////////////////////////////////////////////////////
;// Windows Installer minor errors
;//
;// The following errors are used to indicate that part of a search failed for
;// MSI problems. Another part of the search may successfully return updates.
;// All MSI minor codes should share the same error code range so that the caller
;// tell that they are related to Windows Installer.
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x1001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MSI_WRONG_VERSION
Language     = English
Search may have missed some updates because the Windows Installer is less than version 3.1.
.

MessageId    = 0x1002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MSI_NOT_CONFIGURED
Language     = English
Search may have missed some updates because the Windows Installer is not configured.
.

MessageId    = 0x1003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MSP_DISABLED
Language     = English
Search may have missed some updates because policy has disabled Windows Installer patching.
.

MessageId    = 0x1004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MSI_WRONG_APP_CONTEXT
Language     = English
An update could not be applied because the application is installed per-user.
.

MessageId    = 0x1005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MSI_NOT_PRESENT
Language     = English
Search may have missed some updates because the Windows Installer is less than version 3.1.
.

MessageId    = 0x1FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_MSP_UNEXPECTED
Language     = English
Search may have missed some updates because there was a failure of the Windows Installer.
.

;///////////////////////////////////////////////////////////////////////////////
;// Protocol Talker errors
;//
;// The following map to SOAPCLIENT_ERRORs from atlsoap.h. These errors
;// are obtained from calling GetClientError() on the CClientWebService
;// object.
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x4000
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_BASE
Language     = English
WU_E_PT_SOAPCLIENT_* error codes map to the SOAPCLIENT_ERROR enum of the ATL Server Library.
.

MessageId    = 0x4001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_INITIALIZE
Language     = English
SOAPCLIENT_INITIALIZE_ERROR - initialization of the SOAP client failed, possibly because of an MSXML installation failure.
.

MessageId    = 0x4002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_OUTOFMEMORY
Language     = English
SOAPCLIENT_OUTOFMEMORY - SOAP client failed because it ran out of memory.
.

MessageId    = 0x4003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_GENERATE
Language     = English
SOAPCLIENT_GENERATE_ERROR - SOAP client failed to generate the request.
.

MessageId    = 0x4004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_CONNECT
Language     = English
SOAPCLIENT_CONNECT_ERROR - SOAP client failed to connect to the server.
.

MessageId    = 0x4005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_SEND
Language     = English
SOAPCLIENT_SEND_ERROR - SOAP client failed to send a message for reasons of WU_E_WINHTTP_* error codes.
.

MessageId    = 0x4006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_SERVER
Language     = English
SOAPCLIENT_SERVER_ERROR - SOAP client failed because there was a server error.
.

MessageId    = 0x4007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_SOAPFAULT
Language     = English
SOAPCLIENT_SOAPFAULT - SOAP client failed because there was a SOAP fault for reasons of WU_E_PT_SOAP_* error codes.
.

MessageId    = 0x4008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_PARSEFAULT
Language     = English
SOAPCLIENT_PARSEFAULT_ERROR - SOAP client failed to parse a SOAP fault.
.

MessageId    = 0x4009
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_READ
Language     = English
SOAPCLIENT_READ_ERROR - SOAP client failed while reading the response from the server.
.

MessageId    = 0x400A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAPCLIENT_PARSE
Language     = English
SOAPCLIENT_PARSE_ERROR - SOAP client failed to parse the response from the server.
.

;// The following map to SOAP_ERROR_CODEs from atlsoap.h. These errors
;// are obtained from the m_fault.m_soapErrCode member on the
;// CClientWebService object when GetClientError() returned
;// SOAPCLIENT_SOAPFAULT.
MessageId    = 0x400B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAP_VERSION
Language     = English
SOAP_E_VERSION_MISMATCH - SOAP client found an unrecognizable namespace for the SOAP envelope.
.

MessageId    = 0x400C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAP_MUST_UNDERSTAND
Language     = English
SOAP_E_MUST_UNDERSTAND - SOAP client was unable to understand a header.
.

MessageId    = 0x400D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAP_CLIENT
Language     = English
SOAP_E_CLIENT - SOAP client found the message was malformed; fix before resending.
.

MessageId    = 0x400E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SOAP_SERVER
Language     = English
SOAP_E_SERVER - The SOAP message could not be processed due to a server error; resend later.
.

MessageId    = 0x400F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_WMI_ERROR
Language     = English
There was an unspecified Windows Management Instrumentation (WMI) error.
.

MessageId    = 0x4010
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_EXCEEDED_MAX_SERVER_TRIPS
Language     = English
The number of round trips to the server exceeded the maximum limit.
.

MessageId    = 0x4011
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SUS_SERVER_NOT_SET
Language     = English
WUServer policy value is missing in the registry.
.

MessageId    = 0x4012
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_DOUBLE_INITIALIZATION
Language     = English
Initialization failed because the object was already initialized.
.

MessageId    = 0x4013
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_INVALID_COMPUTER_NAME
Language     = English
The computer name could not be determined.
.

MessageId    = 0x4015
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_REFRESH_CACHE_REQUIRED
Language     = English
The reply from the server indicates that the server was changed or the cookie was invalid; refresh the state of the internal cache and retry.
.

MessageId    = 0x4016
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_BAD_REQUEST
Language     = English
HTTP status 400 - the server could not process the request due to invalid syntax.
.

MessageId    = 0x4017
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_DENIED
Language     = English
HTTP status 401 - the requested resource requires user authentication.
.

MessageId    = 0x4018
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_FORBIDDEN
Language     = English
HTTP status 403 - server understood the request, but declined to fulfill it.
.

MessageId    = 0x4019
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_NOT_FOUND
Language     = English
HTTP status 404 - the server cannot find the requested URI (Uniform Resource Identifier).
.

MessageId    = 0x401A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_BAD_METHOD
Language     = English
HTTP status 405 - the HTTP method is not allowed.
.

MessageId    = 0x401B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_PROXY_AUTH_REQ
Language     = English
HTTP status 407 - proxy authentication is required.
.

MessageId    = 0x401C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_REQUEST_TIMEOUT
Language     = English
HTTP status 408 - the server timed out waiting for the request.
.

MessageId    = 0x401D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_CONFLICT
Language     = English
HTTP status 409 - the request was not completed due to a conflict with the current state of the resource.
.

MessageId    = 0x401E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_GONE
Language     = English
HTTP status 410 - requested resource is no longer available at the server.
.

MessageId    = 0x401F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_SERVER_ERROR
Language     = English
HTTP status 500 - an error internal to the server prevented fulfilling the request.
.

MessageId    = 0x4020
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_NOT_SUPPORTED
Language     = English
HTTP status 500 - server does not support the functionality required to fulfill the request.
.

MessageId    = 0x4021
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_BAD_GATEWAY
Language     = English
HTTP status 502 - the server, while acting as a gateway or proxy, received an invalid response from the upstream server it accessed in attempting to fulfill the request.
.

MessageId    = 0x4022
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_SERVICE_UNAVAIL
Language     = English
HTTP status 503 - the service is temporarily overloaded.
.

MessageId    = 0x4023
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_GATEWAY_TIMEOUT
Language     = English
HTTP status 503 - the request was timed out waiting for a gateway.
.

MessageId    = 0x4024
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_VERSION_NOT_SUP
Language     = English
HTTP status 505 - the server does not support the HTTP protocol version used for the request.
.

MessageId    = 0x4025
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_FILE_LOCATIONS_CHANGED
Language     = English
Operation failed due to a changed file location; refresh internal state and resend.
.

MessageId    = 0x4026
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_REGISTRATION_NOT_SUPPORTED
Language     = English
Operation failed because Windows Update Agent does not support registration with a non-WSUS server.
.

MessageId    = 0x4027
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_NO_AUTH_PLUGINS_REQUESTED
Language     = English
The server returned an empty authentication information list.
.

MessageId    = 0x4028
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_NO_AUTH_COOKIES_CREATED
Language     = English
Windows Update Agent was unable to create any valid authentication cookies.
.

MessageId    = 0x4029
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_INVALID_CONFIG_PROP
Language     = English
A configuration property value was wrong.
.

MessageId    = 0x402A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_CONFIG_PROP_MISSING
Language     = English
A configuration property value was missing.
.

MessageId    = 0x402B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_HTTP_STATUS_NOT_MAPPED
Language     = English
The HTTP request could not be completed and the reason did not correspond to any of the WU_E_PT_HTTP_* error codes.
.

MessageId    = 0x402C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_WINHTTP_NAME_NOT_RESOLVED
Language     = English
ERROR_WINHTTP_NAME_NOT_RESOLVED - the proxy server or target server name cannot be resolved.
.

MessageId    = 0x402D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_LOAD_SHEDDING
Language     = English
The server is shedding load.
.

MessageId    = 0x402E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_CLIENT_ENFORCED_LOAD_SHEDDING
Language     = English
Windows Update Agent is enforcing honoring the service load shedding interval.
.

MessageId    = 0x502D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SAME_REDIR_ID
Language     = English
Windows Update Agent failed to download a redirector cabinet file with a new redirectorId value from the server during the recovery.
.

MessageId    = 0x502E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_NO_MANAGED_RECOVER
Language     = English
A redirector recovery action did not complete because the server is managed.
.

MessageId    = 0x402F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ECP_SUCCEEDED_WITH_ERRORS
Language     = English
External cab file processing completed with some errors.
.

MessageId    = 0x4030
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ECP_INIT_FAILED
Language     = English
The external cab processor initialization did not complete.
.

MessageId    = 0x4031
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ECP_INVALID_FILE_FORMAT
Language     = English
The format of a metadata file was invalid.
.

MessageId    = 0x4032
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ECP_INVALID_METADATA
Language     = English
External cab processor found invalid metadata.
.

MessageId    = 0x4033
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ECP_FAILURE_TO_EXTRACT_DIGEST
Language     = English
The file digest could not be extracted from an external cab file.
.

MessageId    = 0x4034
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ECP_FAILURE_TO_DECOMPRESS_CAB_FILE
Language     = English
An external cab file could not be decompressed.
.

MessageId    = 0x4035
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ECP_FILE_LOCATION_ERROR
Language     = English
External cab processor was unable to get file locations.
.

MessageId    = 0x0436
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_CATALOG_SYNC_REQUIRED
Language     = English
The server does not support category-specific search; Full catalog search has to be issued instead.
.

MessageId    = 0x0437
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SECURITY_VERIFICATION_FAILURE
Language     = English
There was a problem authorizing with the service.
.

MessageId    = 0x0438
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ENDPOINT_UNREACHABLE
Language     = English
There is no route or network connectivity to the endpoint.
.

MessageId    = 0x0439
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_INVALID_FORMAT
Language     = English
The data received does not meet the data contract expectations.
.

MessageId    = 0x043A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_INVALID_URL
Language     = English
The url is invalid.
.

MessageId    = 0x043B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_NWS_NOT_LOADED
Language     = English
Unable to load NWS runtime.
.

MessageId    = 0x043C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_PROXY_AUTH_SCHEME_NOT_SUPPORTED
Language     = English
The proxy auth scheme is not supported.
.

MessageId    = 0x043D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SERVICEPROP_NOTAVAIL
Language     = English
The requested service property is not available.
.

MessageId    = 0x043E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ENDPOINT_REFRESH_REQUIRED
Language     = English
The endpoint provider plugin requires online refresh.
.

MessageId    = 0x043F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ENDPOINTURL_NOTAVAIL
Language     = English
A URL for the requested service endpoint is not available.
.

MessageId    = 0x0440
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ENDPOINT_DISCONNECTED
Language     = English
The connection to the service endpoint died.
.

MessageId    = 0x0441
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_INVALID_OPERATION
Language     = English
The operation is invalid because protocol talker is in an inappropriate state.
.

;// Same as WS_E_OBJECT_FAULTED
MessageId    = 0x0442
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_OBJECT_FAULTED
Language     = English
The object is in a faulted state due to a previous error.
.

;// Same as WS_E_NUMERIC_OVERFLOW
MessageId    = 0x0443
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_NUMERIC_OVERFLOW
Language     = English
The operation would lead to numeric overflow.
.

;// Same as WS_E_OPERATION_ABORTED
MessageId    = 0x0444
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_OPERATION_ABORTED
Language     = English
The operation was aborted.
.

;// Same as WS_E_OPERATION_ABANDONED
MessageId    = 0x0445
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_OPERATION_ABANDONED
Language     = English
The operation was abandoned.
.

;// Same as WS_E_QUOTA_EXCEEDED
MessageId    = 0x0446
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_QUOTA_EXCEEDED
Language     = English
A quota was exceeded.
.

;// Same as WS_E_NO_TRANSLATION_AVAILABLE
MessageId    = 0x0447
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_NO_TRANSLATION_AVAILABLE
Language     = English
The information was not available in the specified language.
.

;// Same as WS_E_ADDRESS_IN_USE
MessageId    = 0x0448
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ADDRESS_IN_USE
Language     = English
The address is already being used.
.

;// Same as WS_E_ADDRESS_NOT_AVAILABLE
MessageId    = 0x0449
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_ADDRESS_NOT_AVAILABLE
Language     = English
The address is not valid for this context.
.

;// Same as WS_E_OTHER
MessageId    = 0x044A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_OTHER
Language     = English
Unrecognized error occurred in the Windows Web Services framework.
.

;// Same as WS_E_SECURITY_SYSTEM_FAILURE
MessageId    = 0x044B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_SECURITY_SYSTEM_FAILURE
Language     = English
A security operation failed in the Windows Web Services framework.
.

MessageId    = 0x4100
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_DATA_BOUNDARY_RESTRICTED
Language     = English
The client is data boundary restricted and needs to talk to a restricted endpoint.
.

MessageId    = 0x4101
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_GENERAL_AAD_CLIENT_ERROR
Language     = English
The client hit an error in retrieving AAD device ticket.
.

MessageId    = 0x4FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_PT_UNEXPECTED
Language     = English
A communication error not covered by another WU_E_PT_* error code.
.

;///////////////////////////////////////////////////////////////////////////////
;// Redirector errors
;//
;// The following errors are generated by the components that download and
;// parse the wuredir.cab
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x5001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_LOAD_XML
Language     = English
The redirector XML document could not be loaded into the DOM class.
.

MessageId    = 0x5002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_S_FALSE
Language     = English
The redirector XML document is missing some required information.
.

MessageId    = 0x5003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_ID_SMALLER
Language     = English
The redirectorId in the downloaded redirector cab is less than in the cached cab.
.

MessageId    = 0x5004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_UNKNOWN_SERVICE
Language     = English
The service ID is not supported in the service environment.
.

MessageId    = 0x5005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_UNSUPPORTED_CONTENTTYPE
Language     = English
The response from the redirector server had an unsupported content type.
.

MessageId    = 0x5006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_INVALID_RESPONSE
Language     = English
The response from the redirector server had an error status or was invalid.
.

MessageId    = 0x5008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_ATTRPROVIDER_EXCEEDED_MAX_NAMEVALUE
Language     = English
The maximum number of name value pairs was exceeded by the attribute provider.
.

MessageId    = 0x5009
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_ATTRPROVIDER_INVALID_NAME
Language     = English
The name received from the attribute provider was invalid.
.

MessageId    = 0x500A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_ATTRPROVIDER_INVALID_VALUE
Language     = English
The value received from the attribute provider was invalid.
.

MessageId    = 0x500B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_SLS_GENERIC_ERROR
Language     = English
There was an error in connecting to or parsing the response from the Service Locator Service redirector server.
.

MessageId    = 0x500C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_CONNECT_POLICY
Language     = English
Connections to the redirector server are disallowed by managed policy.
.

MessageId    = 0x500D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_ONLINE_DISALLOWED
Language     = English
The redirector would go online but is disallowed by caller configuration.
.

MessageId    = 0x50FF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REDIRECTOR_UNEXPECTED
Language     = English
The redirector failed for reasons not covered by another WU_E_REDIRECTOR_* error code.
.

;///////////////////////////////////////////////////////////////////////////////
;// SIH errors
;//
;// The following errors are generated by the components that are involved with
;// service-initiated healing.
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x5101
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_VERIFY_DOWNLOAD_ENGINE
Language     = English
Verification of the servicing engine package failed.
.

MessageId    = 0x5102
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_VERIFY_DOWNLOAD_PAYLOAD
Language     = English
Verification of a servicing package failed.
.

MessageId    = 0x5103
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_VERIFY_STAGE_ENGINE
Language     = English
Verification of the staged engine failed.
.

MessageId    = 0x5104
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_VERIFY_STAGE_PAYLOAD
Language     = English
Verification of a staged payload failed.
.

MessageId    = 0x5105
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_ACTION_NOT_FOUND
Language     = English
An internal error occurred where the servicing action was not found.
.

MessageId    = 0x5106
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_SLS_PARSE
Language     = English
There was a parse error in the service environment response.
.

MessageId    = 0x5107
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_INVALIDHASH
Language     = English
A downloaded file failed an integrity check.
.

MessageId    = 0x5108
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_NO_ENGINE
Language     = English
No engine was provided by the server-initiated healing server response.
.

MessageId    = 0x5109
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_POST_REBOOT_INSTALL_FAILED
Language     = English
Post-reboot install failed.
.

MessageId    = 0x510A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_POST_REBOOT_NO_CACHED_SLS_RESPONSE
Language     = English
There were pending reboot actions, but cached SLS response was not found post-reboot.
.

MessageId    = 0x510B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_PARSE
Language     = English
Parsing command line arguments failed.
.

MessageId    = 0x510C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_SECURITY
Language     = English
Security check failed.
.

MessageId    = 0x510D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_PPL
Language     = English
PPL check failed.
.

MessageId    = 0x510E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_POLICY
Language     = English
Execution was disabled by policy.
.

MessageId    = 0x510F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_STDEXCEPTION
Language     = English
A standard exception was caught.
.

MessageId    = 0x5110
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_NONSTDEXCEPTION
Language     = English
A non-standard exception was caught.
.

MessageId    = 0x5111
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_ENGINE_EXCEPTION
Language     = English
The server-initiated healing engine encountered an exception not covered by another WU_E_SIH_* error code.
.

MessageId    = 0x5112
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_BLOCKED_FOR_PLATFORM
Language     = English
You are running SIH Client with cmd not supported on your platform.
.

MessageId    = 0x5113
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_ANOTHER_INSTANCE_RUNNING
Language     = English
Another SIH Client is already running.
.

MessageId    = 0x5114
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_DNSRESILIENCY_OFF
Language     = English
Disable DNS resiliency feature per service configuration.
.

MessageId    = 0x51FF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SIH_UNEXPECTED
Language     = English
There was a failure for reasons not covered by another WU_E_SIH_* error code.
.

;///////////////////////////////////////////////////////////////////////////////
;// driver util errors
;//
;// The device PnP enumerated device was pruned from the SystemSpec because
;// one of the hardware or compatible IDs matched an installed printer driver.
;// This is not considered a fatal error and the device is simply skipped.
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0xC001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_PRUNED
Language     = English
A driver was skipped.
.

MessageId    = 0xC002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_NOPROP_OR_LEGACY
Language     = English
A property for the driver could not be found. It may not conform with required specifications.
.

MessageId    = 0xC003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_REG_MISMATCH
Language     = English
The registry type read for the driver does not match the expected type.
.

MessageId    = 0xC004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_NO_METADATA
Language     = English
The driver update is missing metadata.
.

MessageId    = 0xC005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_MISSING_ATTRIBUTE
Language     = English
The driver update is missing a required attribute.
.

MessageId    = 0xC006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_SYNC_FAILED
Language     = English
Driver synchronization failed.
.

MessageId    = 0xC007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_NO_PRINTER_CONTENT
Language     = English
Information required for the synchronization of applicable printers is missing.
.

MessageId    = 0xC008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_DEVICE_PROBLEM
Language     = English
After installing a driver update, the updated device has reported a problem.
.

;// MessageId 0xCE00 through 0xCEFF are reserved for post-install driver problem codes
;// (see uhdriver.cpp)
MessageId    = 0xCFFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DRV_UNEXPECTED
Language     = English
A driver error not covered by another WU_E_DRV_* code.
.

;//////////////////////////////////////////////////////////////////////////////
;// data store errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x8000
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_SHUTDOWN
Language     = English
An operation failed because Windows Update Agent is shutting down.
.

MessageId    = 0x8001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_INUSE
Language     = English
An operation failed because the data store was in use.
.

MessageId    = 0x8002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_INVALID
Language     = English
The current and expected states of the data store do not match.
.

MessageId    = 0x8003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_TABLEMISSING
Language     = English
The data store is missing a table.
.

MessageId    = 0x8004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_TABLEINCORRECT
Language     = English
The data store contains a table with unexpected columns.
.

MessageId    = 0x8005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_INVALIDTABLENAME
Language     = English
A table could not be opened because the table is not in the data store.
.

MessageId    = 0x8006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_BADVERSION
Language     = English
The current and expected versions of the data store do not match.
.

MessageId    = 0x8007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA
Language     = English
The information requested is not in the data store.
.

MessageId    = 0x8008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_MISSINGDATA
Language     = English
The data store is missing required information or has a NULL in a table column that requires a non-null value.
.

MessageId    = 0x8009
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_MISSINGREF
Language     = English
The data store is missing required information or has a reference to missing license terms, file, localized property or linked row.
.

MessageId    = 0x800A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_UNKNOWNHANDLER
Language     = English
The update was not processed because its update handler could not be recognized.
.

MessageId    = 0x800B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_CANTDELETE
Language     = English
The update was not deleted because it is still referenced by one or more services.
.

MessageId    = 0x800C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_LOCKTIMEOUTEXPIRED
Language     = English
The data store section could not be locked within the allotted time.
.

MessageId    = 0x800D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NOCATEGORIES
Language     = English
The category was not added because it contains no parent categories and is not a top-level category itself.
.

MessageId    = 0x800E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_ROWEXISTS
Language     = English
The row was not added because an existing row has the same primary key.
.

MessageId    = 0x800F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_STOREFILELOCKED
Language     = English
The data store could not be initialized because it was locked by another process.
.

MessageId    = 0x8010
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_CANNOTREGISTER
Language     = English
The data store is not allowed to be registered with COM in the current process.
.

MessageId    = 0x8011
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_UNABLETOSTART
Language     = English
Could not create a data store object in another process.
.

MessageId    = 0x8013
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_DUPLICATEUPDATEID
Language     = English
The server sent the same update to the client with two different revision IDs.
.

MessageId    = 0x8014
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_UNKNOWNSERVICE
Language     = English
An operation did not complete because the service is not in the data store.
.

MessageId    = 0x8015
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_SERVICEEXPIRED
Language     = English
An operation did not complete because the registration of the service has expired.
.

MessageId    = 0x8016
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_DECLINENOTALLOWED
Language     = English
A request to hide an update was declined because it is a mandatory update or because it was deployed with a deadline.
.

MessageId    = 0x8017
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_TABLESESSIONMISMATCH
Language     = English
A table was not closed because it is not associated with the session.
.

MessageId    = 0x8018
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_SESSIONLOCKMISMATCH
Language     = English
A table was not closed because it is not associated with the session.
.

MessageId    = 0x8019
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NEEDWINDOWSSERVICE
Language     = English
A request to remove the Windows Update service or to unregister it with Automatic Updates was declined because it is a built-in service and/or Automatic Updates cannot fall back to another service.
.

MessageId    = 0x801A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_INVALIDOPERATION
Language     = English
A request was declined because the operation is not allowed.
.

MessageId    = 0x801B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_SCHEMAMISMATCH
Language     = English
The schema of the current data store and the schema of a table in a backup XML document do not match.
.

MessageId    = 0x801C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_RESETREQUIRED
Language     = English
The data store requires a session reset; release the session and retry with a new session.
.

MessageId    = 0x801D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_IMPERSONATED
Language     = English
A data store operation did not complete because it was requested with an impersonated identity.
.

MessageId    = 0x801E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_DATANOTAVAILABLE
Language     = English
An operation against update metadata did not complete because the data was never received from server.
.

MessageId    = 0x801F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_DATANOTLOADED
Language     = English
An operation against update metadata did not complete because the data was available but not loaded from datastore.
.

MessageId    = 0x8020
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_NOSUCHREVISION
Language     = English
A data store operation did not complete because no such update revision is known.
.

MessageId    = 0x8021
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_NOSUCHUPDATE
Language     = English
A data store operation did not complete because no such update is known.
.

MessageId    = 0x8022
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_EULA
Language     = English
A data store operation did not complete because an update's EULA information is missing.
.

MessageId    = 0x8023
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_SERVICE
Language     = English
A data store operation did not complete because a service's information is missing.
.

MessageId    = 0x8024
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_COOKIE
Language     = English
A data store operation did not complete because a service's synchronization information is missing.
.

MessageId    = 0x8025
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_TIMER
Language     = English
A data store operation did not complete because a timer's information is missing.
.

MessageId    = 0x8026
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_CCR
Language     = English
A data store operation did not complete because a download's information is missing.
.

MessageId    = 0x8027
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_FILE
Language     = English
A data store operation did not complete because a file's information is missing.
.

MessageId    = 0x8028
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_DOWNLOADJOB
Language     = English
A data store operation did not complete because a download job's information is missing.
.

MessageId    = 0x8029
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_NODATA_TMI
Language     = English
A data store operation did not complete because a service's timestamp information is missing.
.

MessageId    = 0x8FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DS_UNEXPECTED
Language     = English
A data store error not covered by another WU_E_DS_* code.
.

;/////////////////////////////////////////////////////////////////////////////
;//Inventory Errors
;/////////////////////////////////////////////////////////////////////////////
MessageId    = 0x9001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVENTORY_PARSEFAILED
Language     = English
Parsing of the rule file failed.
.

MessageId    = 0x9002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVENTORY_GET_INVENTORY_TYPE_FAILED
Language     = English
Failed to get the requested inventory type from the server.
.

MessageId    = 0x9003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVENTORY_RESULT_UPLOAD_FAILED
Language     = English
Failed to upload inventory result to the server.
.

MessageId    = 0x9004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVENTORY_UNEXPECTED
Language     = English
There was an inventory error not covered by another error code.
.

MessageId    = 0x9005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVENTORY_WMI_ERROR
Language     = English
A WMI error occurred when enumerating the instances for a particular class.
.

;/////////////////////////////////////////////////////////////////////////////
;//AU Errors
;/////////////////////////////////////////////////////////////////////////////
MessageId    = 0xA000
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_NOSERVICE
Language     = English
Automatic Updates was unable to service incoming requests.
.

MessageId    = 0xA002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_NONLEGACYSERVER
Language     = English
The old version of the Automatic Updates client has stopped because the WSUS server has been upgraded.
.

MessageId    = 0xA003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_LEGACYCLIENTDISABLED
Language     = English
The old version of the Automatic Updates client was disabled.
.

MessageId    = 0xA004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_PAUSED
Language     = English
Automatic Updates was unable to process incoming requests because it was paused.
.

MessageId    = 0xA005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_NO_REGISTERED_SERVICE
Language     = English
No unmanaged service is registered with AU.
.

MessageId    = 0xA006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_DETECT_SVCID_MISMATCH
Language     = English
The default service registered with AU changed during the search.
.

MessageId    = 0xA007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REBOOT_IN_PROGRESS
Language     = English
A reboot is in progress.
.

MessageId    = 0xA008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_OOBE_IN_PROGRESS
Language     = English
Automatic Updates can't process incoming requests while Windows Welcome is running.
.

MessageId    = 0xAFFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AU_UNEXPECTED
Language     = English
An Automatic Updates error not covered by another WU_E_AU * code.
.

;//////////////////////////////////////////////////////////////////////////////
;// update handler errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x2000
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_REMOTEUNAVAILABLE
Language     = English
A request for a remote update handler could not be completed because no remote process is available.
.

MessageId    = 0x2001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_LOCALONLY
Language     = English
A request for a remote update handler could not be completed because the handler is local only.
.

MessageId    = 0x2002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_UNKNOWNHANDLER
Language     = English
A request for an update handler could not be completed because the handler could not be recognized.
.

MessageId    = 0x2003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_REMOTEALREADYACTIVE
Language     = English
A remote update handler could not be created because one already exists.
.

MessageId    = 0x2004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_DOESNOTSUPPORTACTION
Language     = English
A request for the handler to install (uninstall) an update could not be completed because the update does not support install (uninstall).
.

MessageId    = 0x2005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_WRONGHANDLER
Language     = English
An operation did not complete because the wrong handler was specified.
.

MessageId    = 0x2006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_INVALIDMETADATA
Language     = English
A handler operation could not be completed because the update contains invalid metadata.
.

MessageId    = 0x2007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_INSTALLERHUNG
Language     = English
An operation could not be completed because the installer exceeded the time limit.
.

MessageId    = 0x2008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_OPERATIONCANCELLED
Language     = English
An operation being done by the update handler was cancelled.
.

MessageId    = 0x2009
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_BADHANDLERXML
Language     = English
An operation could not be completed because the handler-specific metadata is invalid.
.

MessageId    = 0x200A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_CANREQUIREINPUT
Language     = English
A request to the handler to install an update could not be completed because the update requires user input.
.

MessageId    = 0x200B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_INSTALLERFAILURE
Language     = English
The installer failed to install (uninstall) one or more updates.
.

MessageId    = 0x200C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_FALLBACKTOSELFCONTAINED
Language     = English
The update handler should download self-contained content rather than delta-compressed content for the update.
.

MessageId    = 0x200D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_NEEDANOTHERDOWNLOAD
Language     = English
The update handler did not install the update because it needs to be downloaded again.
.

MessageId    = 0x200E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_NOTIFYFAILURE
Language     = English
The update handler failed to send notification of the status of the install (uninstall) operation.
.

MessageId    = 0x200F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_INCONSISTENT_FILE_NAMES
Language     = English
The file names contained in the update metadata and in the update package are inconsistent.
.

MessageId    = 0x2010
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_FALLBACKERROR
Language     = English
The update handler failed to fall back to the self-contained content.
.

MessageId    = 0x2011
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_TOOMANYDOWNLOADREQUESTS
Language     = English
The update handler has exceeded the maximum number of download requests.
.

MessageId    = 0x2012
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_UNEXPECTEDCBSRESPONSE
Language     = English
The update handler has received an unexpected response from CBS.
.

MessageId    = 0x2013
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_BADCBSPACKAGEID
Language     = English
The update metadata contains an invalid CBS package identifier.
.

MessageId    = 0x2014
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_POSTREBOOTSTILLPENDING
Language     = English
The post-reboot operation for the update is still in progress.
.

MessageId    = 0x2015
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_POSTREBOOTRESULTUNKNOWN
Language     = English
The result of the post-reboot operation for the update could not be determined.
.

MessageId    = 0x2016
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_POSTREBOOTUNEXPECTEDSTATE
Language     = English
The state of the update after its post-reboot operation has completed is unexpected.
.

MessageId    = 0x2017
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_NEW_SERVICING_STACK_REQUIRED
Language     = English
The OS servicing stack must be updated before this update is downloaded or installed.
.

MessageId    = 0x2018
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_CALLED_BACK_FAILURE
Language     = English
A callback installer called back with an error.
.

MessageId    = 0x2019
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_CUSTOMINSTALLER_INVALID_SIGNATURE
Language     = English
The custom installer signature did not match the signature required by the update.
.

MessageId    = 0x201A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_UNSUPPORTED_INSTALLCONTEXT
Language     = English
The installer does not support the installation configuration.
.

MessageId    = 0x201B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_INVALID_TARGETSESSION
Language     = English
The targeted session for install is invalid.
.

MessageId    = 0x201C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_DECRYPTFAILURE
Language     = English
The handler failed to decrypt the update files.
.

MessageId    = 0x201D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_HANDLER_DISABLEDUNTILREBOOT
Language     = English
The update handler is disabled until the system reboots.
.

MessageId    = 0x201E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_APPX_NOT_PRESENT
Language     = English
The AppX infrastructure is not present on the system.
.

MessageId    = 0x201F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_NOTREADYTOCOMMIT
Language     = English
The update cannot be committed because it has not been previously installed or staged.
.

MessageId    = 0x2020
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_APPX_INVALID_PACKAGE_VOLUME
Language     = English
The specified volume is not a valid AppX package volume.
.

MessageId    = 0x2021
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_APPX_DEFAULT_PACKAGE_VOLUME_UNAVAILABLE
Language     = English
The configured default storage volume is unavailable.
.

MessageId    = 0x2022
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_APPX_INSTALLED_PACKAGE_VOLUME_UNAVAILABLE
Language     = English
The volume on which the application is installed is unavailable.
.

MessageId    = 0x2023
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_APPX_PACKAGE_FAMILY_NOT_FOUND
Language     = English
The specified package family is not present on the system.
.

MessageId    = 0x2024
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_APPX_SYSTEM_VOLUME_NOT_FOUND
Language     = English
Unable to find a package volume marked as system.
.

MessageId    = 0x2025
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_UA_SESSION_INFO_VERSION_NOT_SUPPORTED
Language     = English
UA does not support the version of OptionalSessionInfo.
.

MessageId    = 0x2026
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_RESERVICING_REQUIRED_BASELINE
Language     = English
This operation cannot be completed. You must install the baseline update(s) before you can install this update.
.

MessageId    = 0x2FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UH_UNEXPECTED
Language     = English
An update handler error not covered by another WU_E_UH_* code.
.

;//////////////////////////////////////////////////////////////////////////////
;// download manager errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x6001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_URLNOTAVAILABLE
Language     = English
A download manager operation could not be completed because the requested file does not have a URL.
.

MessageId    = 0x6002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_INCORRECTFILEHASH
Language     = English
A download manager operation could not be completed because the file digest was not recognized.
.

MessageId    = 0x6003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UNKNOWNALGORITHM
Language     = English
A download manager operation could not be completed because the file metadata requested an unrecognized hash algorithm.
.

MessageId    = 0x6004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_NEEDDOWNLOADREQUEST
Language     = English
An operation could not be completed because a download request is required from the download handler.
.

MessageId    = 0x6005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_NONETWORK
Language     = English
A download manager operation could not be completed because the network connection was unavailable.
.

MessageId    = 0x6006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_WRONGBITSVERSION
Language     = English
A download manager operation could not be completed because the version of Background Intelligent Transfer Service (BITS) is incompatible.
.

MessageId    = 0x6007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_NOTDOWNLOADED
Language     = English
The update has not been downloaded.
.

MessageId    = 0x6008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_FAILTOCONNECTTOBITS
Language     = English
A download manager operation failed because the download manager was unable to connect the Background Intelligent Transfer Service (BITS).
.

MessageId    = 0x6009
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_BITSTRANSFERERROR
Language     = English
A download manager operation failed because there was an unspecified Background Intelligent Transfer Service (BITS) transfer error.
.

MessageId    = 0x600A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOWNLOADLOCATIONCHANGED
Language     = English
A download must be restarted because the location of the source of the download has changed.
.

MessageId    = 0x600B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_CONTENTCHANGED
Language     = English
A download must be restarted because the update content changed in a new revision.
.

MessageId    = 0x600C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOWNLOADLIMITEDBYUPDATESIZE
Language     = English
A download failed because the current network limits downloads by update size for the update service.
.

MessageId    = 0x600E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UNAUTHORIZED
Language     = English
The download failed because the client was denied authorization to download the content.
.

MessageId    = 0x600F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_BG_ERROR_TOKEN_REQUIRED
Language     = English
The download failed because the user token associated with the BITS job no longer exists.
.

MessageId    = 0x6010
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOWNLOADSANDBOXNOTFOUND
Language     = English
The sandbox directory for the downloaded update was not found.
.

MessageId    = 0x6011
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOWNLOADFILEPATHUNKNOWN
Language     = English
The downloaded update has an unknown file path.
.

MessageId    = 0x6012
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOWNLOADFILEMISSING
Language     = English
One or more of the files for the downloaded update is missing.
.

MessageId    = 0x6013
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UPDATEREMOVED
Language     = English
An attempt was made to access a downloaded update that has already been removed.
.

MessageId    = 0x6014
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_READRANGEFAILED
Language     = English
Windows Update couldn't find a needed portion of a downloaded update's file.
.

MessageId    = 0x6016
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UNAUTHORIZED_NO_USER
Language     = English
The download failed because the client was denied authorization to download the content due to no user logged on.
.

MessageId    = 0x6017
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UNAUTHORIZED_LOCAL_USER
Language     = English
The download failed because the local user was denied authorization to download the content.
.

MessageId    = 0x6018
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UNAUTHORIZED_DOMAIN_USER
Language     = English
The download failed because the domain user was denied authorization to download the content.
.

MessageId    = 0x6019
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UNAUTHORIZED_MSA_USER
Language     = English
The download failed because the MSA account associated with the user was denied authorization to download the content.
.

MessageId    = 0x601A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_FALLINGBACKTOBITS
Language     = English
The download will be continued by falling back to BITS to download the content.
.

MessageId    = 0x601B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOWNLOAD_VOLUME_CONFLICT
Language     = English
Another caller has requested download to a different volume.
.

MessageId    = 0x601C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_SANDBOX_HASH_MISMATCH
Language     = English
The hash of the update's sandbox does not match the expected value.
.

MessageId    = 0x601D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_HARDRESERVEID_CONFLICT
Language     = English
The hard reserve id specified conflicts with an id from another caller.
.

MessageId    = 0x601E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOSVC_REQUIRED
Language     = English
The update has to be downloaded via DO.
.

MessageId    = 0x601F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_DOWNLOADTYPE_CONFLICT
Language     = English
Windows Update only supports one download type per update at one time. The download failure is by design here since the same update with different download type is operating. Please try again later.
.

MessageId    = 0x6FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_DM_UNEXPECTED
Language     = English
There was a download manager error not covered by another WU_E_DM_* error code.
.

;//////////////////////////////////////////////////////////////////////////////
;// Setup/SelfUpdate errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0xD001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_INVALID_INFDATA
Language     = English
Windows Update Agent could not be updated because an INF file contains invalid information.
.

MessageId    = 0xD002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_INVALID_IDENTDATA
Language     = English
Windows Update Agent could not be updated because the wuident.cab file contains invalid information.
.

MessageId    = 0xD003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_ALREADY_INITIALIZED
Language     = English
Windows Update Agent could not be updated because of an internal error that caused setup initialization to be performed twice.
.

MessageId    = 0xD004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_NOT_INITIALIZED
Language     = English
Windows Update Agent could not be updated because setup initialization never completed successfully.
.

MessageId    = 0xD005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_SOURCE_VERSION_MISMATCH
Language     = English
Windows Update Agent could not be updated because the versions specified in the INF do not match the actual source file versions.
.

MessageId    = 0xD006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_TARGET_VERSION_GREATER
Language     = English
Windows Update Agent could not be updated because a WUA file on the target system is newer than the corresponding source file.
.

MessageId    = 0xD007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_REGISTRATION_FAILED
Language     = English
Windows Update Agent could not be updated because regsvr32.exe returned an error.
.

MessageId    = 0xD008
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SELFUPDATE_SKIP_ON_FAILURE
Language     = English
An update to the Windows Update Agent was skipped because previous attempts to update have failed.
.

MessageId    = 0xD009
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_SKIP_UPDATE
Language     = English
An update to the Windows Update Agent was skipped due to a directive in the wuident.cab file.
.

MessageId    = 0xD00A
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_UNSUPPORTED_CONFIGURATION
Language     = English
Windows Update Agent could not be updated because the current system configuration is not supported.
.

MessageId    = 0xD00B
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_BLOCKED_CONFIGURATION
Language     = English
Windows Update Agent could not be updated because the system is configured to block the update.
.

MessageId    = 0xD00C
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_REBOOT_TO_FIX
Language     = English
Windows Update Agent could not be updated because a restart of the system is required.
.

MessageId    = 0xD00D
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_ALREADYRUNNING
Language     = English
Windows Update Agent setup is already running.
.

MessageId    = 0xD00E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_REBOOTREQUIRED
Language     = English
Windows Update Agent setup package requires a reboot to complete installation.
.

MessageId    = 0xD00F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_HANDLER_EXEC_FAILURE
Language     = English
Windows Update Agent could not be updated because the setup handler failed during execution.
.

MessageId    = 0xD010
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_INVALID_REGISTRY_DATA
Language     = English
Windows Update Agent could not be updated because the registry contains invalid information.
.

MessageId    = 0xD011
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SELFUPDATE_REQUIRED
Language     = English
Windows Update Agent must be updated before search can continue.
.

MessageId    = 0xD012
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SELFUPDATE_REQUIRED_ADMIN
Language     = English
Windows Update Agent must be updated before search can continue.  An administrator is required to perform the operation.
.

MessageId    = 0xD013
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_WRONG_SERVER_VERSION
Language     = English
Windows Update Agent could not be updated because the server does not contain update information for this version.
.

MessageId    = 0xD014
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_DEFERRABLE_REBOOT_PENDING
Language     = English
Windows Update Agent is successfully updated, but a reboot is required to complete the setup.
.

MessageId    = 0xD015
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_NON_DEFERRABLE_REBOOT_PENDING
Language     = English
Windows Update Agent is successfully updated, but a reboot is required to complete the setup.
.

MessageId    = 0xD016
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_FAIL
Language     = English
Windows Update Agent could not be updated because of an unknown error.
.

MessageId    = 0xDFFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SETUP_UNEXPECTED
Language     = English
Windows Update Agent could not be updated because of an error not covered by another WU_E_SETUP_* error code.
.

;//////////////////////////////////////////////////////////////////////////////
;// expression evaluator errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0xE001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_UNKNOWN_EXPRESSION
Language     = English
An expression evaluator operation could not be completed because an expression was unrecognized.
.

MessageId    = 0xE002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_INVALID_EXPRESSION
Language     = English
An expression evaluator operation could not be completed because an expression was invalid.
.

MessageId    = 0xE003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_MISSING_METADATA
Language     = English
An expression evaluator operation could not be completed because an expression contains an incorrect number of metadata nodes.
.

MessageId    = 0xE004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_INVALID_VERSION
Language     = English
An expression evaluator operation could not be completed because the version of the serialized expression data is invalid.
.

MessageId    = 0xE005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_NOT_INITIALIZED
Language     = English
The expression evaluator could not be initialized.
.

MessageId    = 0xE006
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_INVALID_ATTRIBUTEDATA
Language     = English
An expression evaluator operation could not be completed because there was an invalid attribute.
.

MessageId    = 0xE007
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_CLUSTER_ERROR
Language     = English
An expression evaluator operation could not be completed because the cluster state of the computer could not be determined.
.

MessageId    = 0xEFFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_EE_UNEXPECTED
Language     = English
There was an expression evaluator error not covered by another WU_E_EE_* error code.
.

;//////////////////////////////////////////////////////////////////////////////
;// UI errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0x3001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INSTALLATION_RESULTS_UNKNOWN_VERSION
Language     = English
The results of download and installation could not be read from the registry due to an unrecognized data format version.
.

MessageId    = 0x3002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INSTALLATION_RESULTS_INVALID_DATA
Language     = English
The results of download and installation could not be read from the registry due to an invalid data format.
.

MessageId    = 0x3003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INSTALLATION_RESULTS_NOT_FOUND
Language     = English
The results of download and installation are not available; the operation may have failed to start.
.

MessageId    = 0x3004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_TRAYICON_FAILURE
Language     = English
A failure occurred when trying to create an icon in the taskbar notification area.
.

MessageId    = 0x3FFD
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_NON_UI_MODE
Language     = English
Unable to show UI when in non-UI mode; WU client UI modules may not be installed.
.

MessageId    = 0x3FFE
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WUCLTUI_UNSUPPORTED_VERSION
Language     = English
Unsupported version of WU client UI exported functions.
.

MessageId    = 0x3FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_AUCLIENT_UNEXPECTED
Language     = English
There was a user interface error not covered by another WU_E_AUCLIENT_* error code.
.

;//////////////////////////////////////////////////////////////////////////////
;// reporter errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0xF001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REPORTER_EVENTCACHECORRUPT
Language     = English
The event cache file was defective.
.

MessageId    = 0xF002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REPORTER_EVENTNAMESPACEPARSEFAILED
Language     = English
The XML in the event namespace descriptor could not be parsed.
.

MessageId    = 0xF003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_EVENT
Language     = English
The XML in the event namespace descriptor could not be parsed.
.

MessageId    = 0xF004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SERVER_BUSY
Language     = English
The server rejected an event because the server was too busy.
.

MessageId    = 0xF005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_CALLBACK_COOKIE_NOT_FOUND
Language     = English
The specified callback cookie is not found.
.

MessageId    = 0xFFFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_REPORTER_UNEXPECTED
Language     = English
There was a reporter error not covered by another error code.
.

MessageId    = 0x7001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_OL_INVALID_SCANFILE
Language     = English
An operation could not be completed because the scan package was invalid.
.

MessageId    = 0x7002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_OL_NEWCLIENT_REQUIRED
Language     = English
An operation could not be completed because the scan package requires a greater version of the Windows Update Agent.
.

MessageId    = 0x7003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_EVENT_PAYLOAD
Language     = English
An invalid event payload was specified.
.

MessageId    = 0x7004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_INVALID_EVENT_PAYLOADSIZE
Language     = English
The size of the event payload submitted is invalid.
.

MessageId    = 0x7005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SERVICE_NOT_REGISTERED
Language     = English
The service is not registered.
.

MessageId    = 0x7FFF
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_OL_UNEXPECTED
Language     = English
Search using the scan package failed.
.

;//////////////////////////////////////////////////////////////////////////////
;// WU Metadata Integrity related errors - 0x71FE
;///////////////////////////////////////////////////////////////////////////////
;///////
;// Metadata General errors 0x7100 - 0x711F
;///////
MessageId    = 0x7100
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_NOOP
Language     = English
No operation was required by update metadata verification.
.

MessageId    = 0x7101
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_CONFIG_INVALID_BINARY_ENCODING
Language     = English
The binary encoding of metadata config data was invalid.
.

MessageId    = 0x7102
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_FETCH_CONFIG
Language     = English
Unable to fetch required configuration for metadata signature verification.
.

MessageId    = 0x7104
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_INVALID_PARAMETER
Language     = English
A metadata verification operation failed due to an invalid parameter.
.

MessageId    = 0x7105
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_UNEXPECTED
Language     = English
A metadata verification operation failed due to reasons not covered by another error code.
.

MessageId    = 0x7106
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_NO_VERIFICATION_DATA
Language     = English
None of the update metadata had verification data, which may be disabled on the update server.
.

MessageId    = 0x7107
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_BAD_FRAGMENTSIGNING_CONFIG
Language     = English
The fragment signing configuration used for verifying update metadata signatures was bad.
.

MessageId    = 0x7108
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_FAILURE_PROCESSING_FRAGMENTSIGNING_CONFIG
Language     = English
There was an unexpected operational failure while parsing fragment signing configuration.
.

;///////
;// Metadata XML errors 0x7120 - 0x713F
;///////
MessageId    = 0x7120
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_MISSING
Language     = English
Required xml data was missing from configuration.
.

MessageId    = 0x7121
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_FRAGMENTSIGNING_MISSING
Language     = English
Required fragmentsigning data was missing from xml configuration.
.

MessageId    = 0x7122
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_MODE_MISSING
Language     = English
Required mode data was missing from xml configuration.
.

MessageId    = 0x7123
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_MODE_INVALID
Language     = English
An invalid metadata enforcement mode was detected.
.

MessageId    = 0x7124
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_VALIDITY_INVALID
Language     = English
An invalid timestamp validity window configuration was detected.
.

MessageId    = 0x7125
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_LEAFCERT_MISSING
Language     = English
Required leaf certificate data was missing from xml configuration.
.

MessageId    = 0x7126
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_INTERMEDIATECERT_MISSING
Language     = English
Required intermediate certificate data was missing from xml configuration.
.

MessageId    = 0x7127
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_LEAFCERT_ID_MISSING
Language     = English
Required leaf certificate id attribute was missing from xml configuration.
.

MessageId    = 0x7128
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_XML_BASE64CERDATA_MISSING
Language     = English
Required certificate base64CerData attribute was missing from xml configuration.
.

;///////
;// Metadata Signature/Hash-related errors 0x7140 - 0x714F
;///////
MessageId    = 0x7140
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_BAD_SIGNATURE
Language     = English
The metadata for an update was found to have a bad or invalid digital signature.
.

MessageId    = 0x7141
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_UNSUPPORTED_HASH_ALG
Language     = English
An unsupported hash algorithm for metadata verification was specified.
.

MessageId    = 0x7142
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_SIGNATURE_VERIFY_FAILED
Language     = English
An error occurred during an update's metadata signature verification.
.

;///////
;// Metadata Certificate Chain trust related errors 0x7150 - 0x715F
;///////
MessageId    = 0x7150
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATATRUST_CERTIFICATECHAIN_VERIFICATION
Language     = English
An failure occurred while verifying trust for metadata signing certificate chains.
.

MessageId    = 0x7151
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATATRUST_UNTRUSTED_CERTIFICATECHAIN
Language     = English
A metadata signing certificate had an untrusted certificate chain.
.

;///////
;// Metadata Timestamp Token/Signature errors 0x7160 - 0x717F
;///////
MessageId    = 0x7160
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_MISSING
Language     = English
An expected metadata timestamp token was missing.
.

MessageId    = 0x7161
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_VERIFICATION_FAILED
Language     = English
A metadata Timestamp token failed verification.
.

MessageId    = 0x7162
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_UNTRUSTED
Language     = English
A metadata timestamp token signer certificate chain was untrusted.
.

MessageId    = 0x7163
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_VALIDITY_WINDOW
Language     = English
A metadata signature timestamp token was no longer within the validity window.
.

MessageId    = 0x7164
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_SIGNATURE
Language     = English
A metadata timestamp token failed signature validation
.

MessageId    = 0x7165
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_CERTCHAIN
Language     = English
A metadata timestamp token certificate failed certificate chain verification.
.

MessageId    = 0x7166
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_REFRESHONLINE
Language     = English
A failure occurred when refreshing a missing timestamp token from the network.
.

MessageId    = 0x7167
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_ALL_BAD
Language     = English
All update metadata verification timestamp tokens from the timestamp token cache are invalid.
.

MessageId    = 0x7168
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_NODATA
Language     = English
No update metadata verification timestamp tokens exist in the timestamp token cache.
.

MessageId    = 0x7169
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_CACHELOOKUP
Language     = English
An error occurred during cache lookup of update metadata verification timestamp token.
.

MessageId    = 0x717E
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_VALIDITYWINDOW_UNEXPECTED
Language     = English
An metadata timestamp token validity window failed unexpectedly due to reasons not covered by another error code.
.

MessageId    = 0x717F
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_TIMESTAMP_TOKEN_UNEXPECTED
Language     = English
An metadata timestamp token verification operation failed due to reasons not covered by another error code.
.

;///////
;// Metadata Certificate-Related errors 0x7180 - 0x719F
;///////
MessageId    = 0x7180
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_CERT_MISSING
Language     = English
An expected metadata signing certificate was missing.
.

MessageId    = 0x7181
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_LEAFCERT_BAD_TRANSPORT_ENCODING
Language     = English
The transport encoding of a metadata signing leaf certificate was malformed.
.

MessageId    = 0x7182
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_INTCERT_BAD_TRANSPORT_ENCODING
Language     = English
The transport encoding of a metadata signing intermediate certificate was malformed.
.

MessageId    = 0x7183
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_METADATA_CERT_UNTRUSTED
Language     = English
A metadata certificate chain was untrusted.
.

;//////////////////////////////////////////////////////////////////////////////
;// WU Task related errors
;///////////////////////////////////////////////////////////////////////////////
MessageId    = 0xB001
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WUTASK_INPROGRESS
Language     = English
The task is currently in progress.
.

MessageId    = 0xB002
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WUTASK_STATUS_DISABLED
Language     = English
The operation cannot be completed since the task status is currently disabled.
.

MessageId    = 0xB003
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WUTASK_NOT_STARTED
Language     = English
The operation cannot be completed since the task is not yet started.
.

MessageId    = 0xB004
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WUTASK_RETRY
Language     = English
The task was stopped and needs to be run again to complete.
.

MessageId    = 0xB005
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WUTASK_CANCELINSTALL_DISALLOWED
Language     = English
Cannot cancel a non-scheduled install.
.

;//////////////////////////////////////////////////////////////////////////////
;// Hardware Capability related errors
;////
MessageId    = 0xB101
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UNKNOWN_HARDWARECAPABILITY
Language     = English
Hardware capability meta data was not found after a sync with the service.
.

MessageId    = 0xB102
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_BAD_XML_HARDWARECAPABILITY
Language     = English
Hardware capability meta data was malformed and/or failed to parse.
.

MessageId    = 0xB103
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_WMI_NOT_SUPPORTED
Language     = English
Unable to complete action due to WMI dependency, which isn't supported on this platform.
.

MessageId    = 0xB104
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_UPDATE_MERGE_NOT_ALLOWED
Language     = English
Merging of the update is not allowed
.

MessageId    = 0xB105
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SKIPPED_UPDATE_INSTALLATION
Language     = English
Installing merged updates only. So skipping non mergeable updates.
.

;//////////////////////////////////////////////////////////////////////////////
;// SLS related errors - 0xB201
;////
;///////
;// SLS General errors 0xB201 - 0xB2FF
;///////
MessageId    = 0xB201
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_SLS_INVALID_REVISION
Language     = English
SLS response returned invalid revision number.
.

;//////////////////////////////////////////////////////////////////////////////
;// trust related errors - 0xB301
;////
;///////
;// trust General errors 0xB301 - 0xB3FF
;///////
MessageId    = 0xB301
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_FILETRUST_DUALSIGNATURE_RSA
Language     = English
File signature validation fails to find valid RSA signature on infrastructure payload.
.

MessageId    = 0xB302
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_FILETRUST_DUALSIGNATURE_ECC
Language     = English
File signature validation fails to find valid ECC signature on infrastructure payload.
.

MessageId    = 0xB303
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_TRUST_SUBJECT_NOT_TRUSTED
Language     = English
The subject is not trusted by WU for the specified action.
.

MessageId    = 0xB304
Facility     = WindowsUpdate
Severity     = Error
SymbolicName = WU_E_TRUST_PROVIDER_UNKNOWN
Language     = English
Unknown trust provider for WU.
.

;#endif //_WUERROR_
