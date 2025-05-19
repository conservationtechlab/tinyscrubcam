/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_host_config.h"
#if ((defined USB_HOST_CONFIG_DFU) && (USB_HOST_CONFIG_DFU))
#include "usb_host.h"
#include "usb_host_dfu.h"

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief DFU control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostDfuControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief DFU open interface. It is called when set interface request success or open alternate setting 0 interface.
 *
 * @param dfuInstance     DFU instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostDfuOpenInterface(usb_host_dfu_instance_t *dfuInstance);

/*!
 * @brief DFU set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostDfuSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

static void USB_HostDfuControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_dfu_instance_t *dfuInstance = (usb_host_dfu_instance_t *)param;

    dfuInstance->controlTransfer = NULL;
    if (dfuInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostDfuSetInterface
        or USB_HostDfuControl, but is the same function */
        dfuInstance->controlCallbackFn(dfuInstance->controlCallbackParam, transfer->transferBuffer,
                                       transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(dfuInstance->hostHandle, transfer);
}

static usb_status_t USB_HostDfuOpenInterface(usb_host_dfu_instance_t *dfuInstance)
{
    return kStatus_USB_Success;
}

static void USB_HostDfuSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_dfu_instance_t *dfuInstance = (usb_host_dfu_instance_t *)param;

    dfuInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostDfuOpenInterface(dfuInstance); /* dfu open interface */
    }

    if (dfuInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostDfuSetInterface
        or USB_HostDfuControl, but is the same function */
        dfuInstance->controlCallbackFn(dfuInstance->controlCallbackParam, NULL, 0, status);
    }
    USB_HostFreeTransfer(dfuInstance->hostHandle, transfer);
}

usb_status_t USB_HostDfuInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    uint32_t infoValue;
    /* malloc dfu class instance */
    usb_host_dfu_instance_t *dfuInstance =
        (usb_host_dfu_instance_t *)OSA_MemoryAllocate(sizeof(usb_host_dfu_instance_t));

    if (dfuInstance == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    /* initialize dfu instance */
    dfuInstance->deviceHandle    = deviceHandle;
    dfuInstance->interfaceHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &infoValue);
    dfuInstance->hostHandle = (usb_host_handle)infoValue;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &infoValue);
    dfuInstance->controlPipe = (usb_host_pipe_handle)infoValue;

    *classHandle = dfuInstance;
    return kStatus_USB_Success;
}

usb_status_t USB_HostDfuSetInterface(usb_host_class_handle classHandle,
                                     usb_host_interface_handle interfaceHandle,
                                     uint8_t alternateSetting,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    usb_status_t status;
    usb_host_dfu_instance_t *dfuInstance = (usb_host_dfu_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    dfuInstance->interfaceHandle = interfaceHandle;
    /* notify host driver the interface is open */
    status = USB_HostOpenDeviceInterface(dfuInstance->deviceHandle, interfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }

    if (alternateSetting == 0) /* open interface directly */
    {
        if (callbackFn != NULL)
        {
            status = USB_HostDfuOpenInterface(dfuInstance);
            callbackFn(callbackParam, NULL, 0, status);
        }
    }
    else /* send setup transfer */
    {
        /* malloc one transfer */
        if (USB_HostMallocTransfer(dfuInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }

        /* save the application callback function */
        dfuInstance->controlCallbackFn    = callbackFn;
        dfuInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn                 = USB_HostDfuSetInterfaceCallback;
        transfer->callbackParam              = dfuInstance;
        transfer->setupPacket->bRequest      = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex        = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)dfuInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue  = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer       = NULL;
        transfer->transferLength       = 0;
        status                         = USB_HostSendSetup(dfuInstance->hostHandle, dfuInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            dfuInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(dfuInstance->hostHandle, transfer);
        }
    }

    return status;
}

usb_status_t USB_HostDfuDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status                  = kStatus_USB_Success;
    usb_host_dfu_instance_t *dfuInstance = (usb_host_dfu_instance_t *)classHandle;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL) /* class instance has initialized */
    {
        /* cancel control transfer if there is on-going control transfer */
        if ((dfuInstance->controlPipe != NULL) && (dfuInstance->controlTransfer != NULL))
        {
            status =
                USB_HostCancelTransfer(dfuInstance->hostHandle, dfuInstance->controlPipe, dfuInstance->controlTransfer);
        }
        /* notify host driver the interface is closed */
        USB_HostCloseDeviceInterface(deviceHandle, dfuInstance->interfaceHandle);
        OSA_MemoryFree(dfuInstance);
    }
    else
    {
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return status;
}

static usb_status_t USB_HostDfuControl(usb_host_class_handle classHandle,
                                       uint8_t requestType,
                                       uint8_t request,
                                       uint8_t wvalue,
                                       uint16_t wlength,
                                       uint8_t *data,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam)
{
    usb_host_dfu_instance_t *dfuInstance = (usb_host_dfu_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(dfuInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Busy;
    }
    /* save the application callback function */
    dfuInstance->controlCallbackFn    = callbackFn;
    dfuInstance->controlCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer             = data;
    transfer->transferLength             = wlength;
    transfer->callbackFn                 = USB_HostDfuControlCallback;
    transfer->callbackParam              = dfuInstance;
    transfer->setupPacket->bmRequestType = requestType;
    transfer->setupPacket->bRequest      = request;
    transfer->setupPacket->wValue        = wvalue;
    transfer->setupPacket->wIndex        = USB_SHORT_TO_LITTLE_ENDIAN(
        ((usb_host_interface_t *)dfuInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(wlength);

    /* call host driver api */
    if (USB_HostSendSetup(dfuInstance->hostHandle, dfuInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failed for USB_HostSendSetup\r\n");
#endif
        USB_HostFreeTransfer(dfuInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    dfuInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

usb_status_t USB_HostDfuGetFunctionalDescriptor(usb_host_class_handle classHandle,
                                                uint8_t **dfuFunctionalDesc,
                                                uint32_t *dfuFunctionalDescLength)
{
    usb_host_dfu_instance_t *dfuInstance = (usb_host_dfu_instance_t *)classHandle;
    uint8_t *descriptor;
    uint32_t endPosition;

    descriptor = (uint8_t *)((usb_host_interface_t *)dfuInstance->interfaceHandle)->interfaceExtension;
    endPosition =
        (uint32_t)descriptor + ((usb_host_interface_t *)dfuInstance->interfaceHandle)->interfaceExtensionLength;

    while ((uint32_t)descriptor < endPosition)
    {
        /* descriptor type */
        if (*(descriptor + 1) == USB_HOST_DFU_FUNCTIONAL_DESCRIPTOR_TYPE)
        {
            *dfuFunctionalDesc       = (uint8_t *)descriptor;
            *dfuFunctionalDescLength = *descriptor;
            return kStatus_USB_Success;
        }
        else
        {
            /* next descriptor */
            descriptor = (uint8_t *)((uint32_t)descriptor + (*descriptor));
        }
    }

    return kStatus_USB_Error;
}

usb_status_t USB_HostDfuDetach(usb_host_class_handle classHandle,
                               uint16_t timeout,
                               transfer_callback_t callbackFn,
                               void *callbackParam)
{
    return USB_HostDfuControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_DFU_DETACH, timeout, 0, NULL, callbackFn, callbackParam);
}

usb_status_t USB_HostDfuDnload(usb_host_class_handle classHandle,
                               uint16_t blockNum,
                               uint8_t *firmwareData,
                               uint32_t firmwareLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam)
{
    return USB_HostDfuControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_DFU_DNLOAD, blockNum, firmwareLength, firmwareData, callbackFn, callbackParam);
}

usb_status_t USB_HostDfuUpload(usb_host_class_handle classHandle,
                               uint8_t blockNum,
                               uint8_t *firmwareData,
                               uint32_t firmwareLength,
                               transfer_callback_t callbackFn,
                               void *callbackParam)
{
    return USB_HostDfuControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_DFU_UPLOAD, blockNum, firmwareLength, firmwareData, callbackFn, callbackParam);
}

usb_status_t USB_HostDfuGetStatus(usb_host_class_handle classHandle,
                                  uint8_t *statusData,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    return USB_HostDfuControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_DFU_GETSTATUS, 0, 6, statusData, callbackFn, callbackParam);
}

usb_status_t USB_HostDfuClrStatus(usb_host_class_handle classHandle,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    return USB_HostDfuControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_DFU_CLRSTATUS, 0, 0, NULL, callbackFn, callbackParam);
}

usb_status_t USB_HostDfuGetState(usb_host_class_handle classHandle,
                                 uint8_t *state,
                                 transfer_callback_t callbackFn,
                                 void *callbackParam)
{
    return USB_HostDfuControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_DFU_GETSTATUS, 0, 1, state, callbackFn, callbackParam);
}

usb_status_t USB_HostDfuAbort(usb_host_class_handle classHandle, transfer_callback_t callbackFn, void *callbackParam)
{
    return USB_HostDfuControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_DFU_ABORT, 0, 0, NULL, callbackFn, callbackParam);
}

#endif /* USB_HOST_CONFIG_DFU */
