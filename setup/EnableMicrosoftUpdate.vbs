' This script enables opting into Microsoft Update on Windows 7.

' First, we need to create a service manager object.
Set ServiceManager = CreateObject("Microsoft.Update.ServiceManager")

' Next, we need to define a Client Application ID.
ServiceManager.ClientApplicationID = "Legacy Update" ' Could be anything

' Finally, add the Microsoft Update Service GUID
Set NewUpdateService = ServiceManager.AddService2("7971f918-a847-4430-9279-4a52d1efe18d", 7, "")

' The Windows Update service now needs to be restarted to acquire the new service registration.