#include "ramdisk"
#include "ntintsafe.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, RamDiskEvtDeviceAdd)
#pragma alloc_text(PAGE, RamDiskEvtDeviceContextCleanup)
#pragma alloc_text(PAGE, RamDiskQueryDiskRegParameters)
#pragma alloc_text(PAGE, RamDiskFormatDisk)
#endif
//函数入口
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	WDF_DRIVER_CONFIG config;

	WDF_DRIVER_CONFIG_INIT(&config,RamDiskEvtDeviceAdd);

	return WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES,&config,WDF_NO_HANDLE);
}
//明天解决容量限制问题