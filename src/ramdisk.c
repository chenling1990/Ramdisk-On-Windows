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
//读操作
VOID RamDiskEvtIoRead(IN WDFQUEUE Queue,IN WDFREQUEST Request,IN size_t Length)
{
    PDEVICE_EXTENSION      devExt = QueueGetExtension(Queue)->DeviceExtension;
    NTSTATUS               Status = STATUS_INVALID_PARAMETER;
    WDF_REQUEST_PARAMETERS Parameters;
    LARGE_INTEGER          ByteOffset;
    WDFMEMORY              hMemory;

    __analysis_assume(Length > 0);

    WDF_REQUEST_PARAMETERS_INIT(&Parameters);
    WdfRequestGetParameters(Request, &Parameters);

    ByteOffset.QuadPart = Parameters.Parameters.Read.DeviceOffset;

    if (RamDiskCheckParameters(devExt, ByteOffset, Length))
    {

        Status = WdfRequestRetrieveOutputMemory(Request, &hMemory);
        if(NT_SUCCESS(Status))
        {
            // Destination #　Offset into the destination ＃　source
            Status = WdfMemoryCopyFromBuffer(hMemory,0, devExt->DiskImage + ByteOffset.LowPart, Length);
        }
    }

    WdfRequestCompleteWithInformation(Request, Status, (ULONG_PTR)Length);
}
// 写操作
VOID RamDiskEvtIoWrite(IN WDFQUEUE Queue,IN WDFREQUEST Request,IN size_t Length)
{
    PDEVICE_EXTENSION      devExt = QueueGetExtension(Queue)->DeviceExtension;
    NTSTATUS               Status = STATUS_INVALID_PARAMETER;
    WDF_REQUEST_PARAMETERS Parameters;
    LARGE_INTEGER          ByteOffset;
    WDFMEMORY              hMemory;

    __analysis_assume(Length > 0);

    WDF_REQUEST_PARAMETERS_INIT(&Parameters);
    WdfRequestGetParameters(Request, &Parameters);
    // from read to write
    ByteOffset.QuadPart = Parameters.Parameters.Write.DeviceOffset;

    if (RamDiskCheckParameters(devExt, ByteOffset, Length))
    {
        // from output to input
        Status = WdfRequestRetrieveInputMemory(Request, &hMemory);
        if(NT_SUCCESS(Status))
        {
            // Source # offset in Source memory where the copy has to start # destination
            Status = WdfMemoryCopyToBuffer(hMemory, 0,devExt->DiskImage + ByteOffset.LowPart, Length);
            // from "from" to "to"
        }

    }

    WdfRequestCompleteWithInformation(Request, Status, (ULONG_PTR)Length);
}
VOID RamDiskEvtIoDeviceControl(IN WDFQUEUE Queue,IN WDFREQUEST Request,IN size_t OutputBufferLength,IN size_t InputBufferLength,IN ULONG IoControlCode)
{
    NTSTATUS          Status = STATUS_INVALID_DEVICE_REQUEST;
    ULONG_PTR         information = 0;
    size_t            bufSize;
    PDEVICE_EXTENSION devExt = QueueGetExtension(Queue)->DeviceExtension;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    switch (IoControlCode)
    {
        case IOCTL_DISK_GET_PARTITION_INFO:
        {

            PPARTITION_INFORMATION outputBuffer;
            PBOOT_SECTOR bootSector = (PBOOT_SECTOR) devExt->DiskImage;

            information = sizeof(PARTITION_INFORMATION);

            Status = WdfRequestRetrieveOutputBuffer(Request, sizeof(PARTITION_INFORMATION), &outputBuffer, &bufSize);
            if(NT_SUCCESS(Status) )
            {

                outputBuffer->PartitionType = (bootSector->bsFileSystemType[4] == '6') ? PARTITION_FAT_16 : PARTITION_FAT_12;

                outputBuffer->BootIndicator       = FALSE;
                outputBuffer->RecognizedPartition = TRUE;
                outputBuffer->RewritePartition    = FALSE;
                outputBuffer->StartingOffset.QuadPart = 0;
                outputBuffer->PartitionLength.QuadPart = devExt->DiskRegInfo.DiskSize;
                outputBuffer->HiddenSectors       = (ULONG) (1L);
                outputBuffer->PartitionNumber     = (ULONG) (-1L);

                Status = STATUS_SUCCESS;
            }
        }
        break;

        case IOCTL_DISK_GET_DRIVE_GEOMETRY:
        {

            PDISK_GEOMETRY outputBuffer;

            //
            // Return the drive geometry for the ram disk. Note that
            // we return values which were made up to suit the disk size.
            //
            information = sizeof(DISK_GEOMETRY);

            Status = WdfRequestRetrieveOutputBuffer(Request, sizeof(DISK_GEOMETRY), &outputBuffer, &bufSize);
            if(NT_SUCCESS(Status) )
            {

                RtlCopyMemory(outputBuffer, &(devExt->DiskGeometry), sizeof(DISK_GEOMETRY));
                Status = STATUS_SUCCESS;
            }
        }
        break;

        case IOCTL_DISK_CHECK_VERIFY:
        case IOCTL_DISK_IS_WRITABLE:

        Status = STATUS_SUCCESS;
        break;
    }

    WdfRequestCompleteWithInformation(Request, Status, information);
}
VOID RamDiskEvtDeviceContextCleanup(IN WDFOBJECT Device)
{
    PDEVICE_EXTENSION pDeviceExtension = DeviceGetExtension(Device);

    PAGED_CODE();

    if(pDeviceExtension->DiskImage)
    {
        ExFreePool(pDeviceExtension->DiskImage);
    }
}
// 添加设备
NTSTATUS RamDiskEvtDeviceAdd(IN WDFDRIVER Driver,IN PWDFDEVICE_INIT DeviceInit)
{
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    NTSTATUS                status;
    WDFDEVICE               device;
    WDF_OBJECT_ATTRIBUTES   queueAttributes;
    WDF_IO_QUEUE_CONFIG     ioQueueConfig;
    PDEVICE_EXTENSION       pDeviceExtension;
    PQUEUE_EXTENSION        pQueueContext = NULL;
    WDFQUEUE                queue;
    DECLARE_CONST_UNICODE_STRING(ntDeviceName, NT_DEVICE_NAME);

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Driver);

    //
    // Storage drivers have to name their FDOs. Since we are not unique'fying
    // the device name, we wouldn't be able to install more than one instance
    // of this ramdisk driver.
    //
    status = WdfDeviceInitAssignName(DeviceInit, &ntDeviceName);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_DISK);
    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoDirect);
    WdfDeviceInitSetExclusive(DeviceInit, FALSE);

    //
    // Since this is a pure software only driver, there is no need to register
    // any PNP/Power event callbacks. Framework will respond to these
    // events appropriately.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_EXTENSION);
    deviceAttributes.EvtCleanupCallback = RamDiskEvtDeviceContextCleanup;

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //
    // Now that the WDF device object has been created, set up any context
    // that it requires.
    //

    pDeviceExtension = DeviceGetExtension(device);

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE (&ioQueueConfig,WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoDeviceControl = RamDiskEvtIoDeviceControl;
    ioQueueConfig.EvtIoRead          = RamDiskEvtIoRead;
    ioQueueConfig.EvtIoWrite         = RamDiskEvtIoWrite;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_EXTENSION);

    status = WdfIoQueueCreate( device,&ioQueueConfig,&queueAttributes,&queue );

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // Context is the Queue handle
    pQueueContext = QueueGetExtension(queue);

    //
    // Set the context for our default queue as our device extension.
    //
    pQueueContext->DeviceExtension = pDeviceExtension;

    //
    // Enable forward progress on the queue we just created.
    // NOTE: If you are planning to use this code without forward progress,
    // comment out the call to SetForwardProgressOnQueue below.
    //
    status = SetForwardProgressOnQueue(queue);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //
    // Now do any RAM-Disk specific initialization
    //
    pDeviceExtension->DiskRegInfo.DriveLetter.Buffer = (PWSTR) &pDeviceExtension->DriveLetterBuffer;
    pDeviceExtension->DiskRegInfo.DriveLetter.MaximumLength = sizeof(pDeviceExtension->DriveLetterBuffer);

    //
    // Get the disk parameters from the registry
    //
    RamDiskQueryDiskRegParameters(WdfDriverGetRegistryPath(WdfDeviceGetDriver(device)),&pDeviceExtension->DiskRegInfo);

    //
    // Allocate memory for the disk image.
    //
    pDeviceExtension->DiskImage = ExAllocatePoolWithTag(NonPagedPool,pDeviceExtension->DiskRegInfo.DiskSize,RAMDISK_TAG);

    if (pDeviceExtension->DiskImage)
    {

        UNICODE_STRING deviceName;
        UNICODE_STRING win32Name;

        RamDiskFormatDisk(pDeviceExtension);

        status = STATUS_SUCCESS;

        //
        // Now try to create a symbolic link for the drive letter.
        //
        RtlInitUnicodeString(&win32Name, DOS_DEVICE_NAME);
        RtlInitUnicodeString(&deviceName, NT_DEVICE_NAME);

        pDeviceExtension->SymbolicLink.Buffer = (PWSTR)&pDeviceExtension->DosDeviceNameBuffer;
        pDeviceExtension->SymbolicLink.MaximumLength = sizeof(pDeviceExtension->DosDeviceNameBuffer);
        pDeviceExtension->SymbolicLink.Length = win32Name.Length;

        RtlCopyUnicodeString(&pDeviceExtension->SymbolicLink, &win32Name);
        RtlAppendUnicodeStringToString(&pDeviceExtension->SymbolicLink,&pDeviceExtension->DiskRegInfo.DriveLetter);

        status = WdfDeviceCreateSymbolicLink(device,&pDeviceExtension->SymbolicLink);
    }

    return status;
}
