/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_HOST_DFU_H_
#define _USB_HOST_DFU_H_
#include "usb.h"
#include "usb_host.h"
#include "usb_spec.h"

/*******************************************************************************
 * DFU class public structure, enumerations, macros, functions
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup usb_host_dfu_drv
 * @{
 */

/*! @brief DFU class-specific request (DFU_DETACH) */
#define USB_HOST_DFU_DETACH (0x00U)
/*! @brief DFU class-specific request (DFU_DNLOAD) */
#define USB_HOST_DFU_DNLOAD (0x01U)
/*! @brief DFU class-specific request (DFU_UPLOAD) */
#define USB_HOST_DFU_UPLOAD (0x02U)
/*! @brief DFU class-specific request (DFU_GETSTATUS) */
#define USB_HOST_DFU_GETSTATUS (0x03U)
/*! @brief DFU class-specific request (DFU_CLRSTATUS) */
#define USB_HOST_DFU_CLRSTATUS (0x04U)
/*! @brief DFU class-specific request (DFU_GETSTATE) */
#define USB_HOST_DFU_GETSTATE (0x05U)
/*! @brief DFU class-specific request (DFU_ABORT) */
#define USB_HOST_DFU_ABORT (0x06U)

/*! @brief DFU class code */
#define USB_HOST_DFU_CLASS_CODE (0xFEU)
/*! @brief DFU sub-class code */
#define USB_HOST_DFU_SUBCLASS_CODE (0x01U)
/*! @brief DFU class protocol code (runtime) */
#define USB_HOST_DFU_PROTOCOL_RUNTIME (0x01U)
/*! @brief DFU class protocol code (DFU mode) */
#define USB_HOST_DFU_PROTOCOL_DFU_MODE (0x02U)

/*! @brief DFU FUNCTIONAL descriptor type */
#define USB_HOST_DFU_FUNCTIONAL_DESCRIPTOR_TYPE (0x21U)

/*! @brief DFU instance structure and DFU usb_host_class_handle pointer to this structure */
typedef struct _usb_host_dfu_instance
{
    usb_host_handle hostHandle;                /*!< This instance's related host handle*/
    usb_device_handle deviceHandle;            /*!< This instance's related device handle*/
    usb_host_interface_handle interfaceHandle; /*!< This instance's related interface handle*/
    usb_host_pipe_handle controlPipe;          /*!< This instance's related device control pipe*/
    transfer_callback_t controlCallbackFn;     /*!< DFU control transfer callback function pointer*/
    void *controlCallbackParam;                /*!< DFU control transfer callback parameter*/
    usb_host_transfer_t *controlTransfer;      /*!< Ongoing control transfer*/
} usb_host_dfu_instance_t;

/*! @brief DFU descriptor structure according to the 6.2.1 in DFU specification */
typedef struct _usb_host_dfu_functional_descriptor
{
    uint8_t bLength;         /*!< Total size of the DFU descriptor*/
    uint8_t bDescriptorType; /*!< Constant name specifying type of DFU descriptor*/
    union
    {
        uint8_t bmAttributes; /*!< DFU attributes*/
        struct
        {
            uint8_t bitCanDnload : 1;             /*!< download capable*/
            uint8_t bitCanUpload : 1;             /*!< upload capable*/
            uint8_t bitManifestationTolerant : 1; /*!< device is able to communicate via USB after Manifestation phase*/
            /*< device will perform a bus detach-attach sequence when it receives a DFU_DETACH request. The host must
             * not issue a USB Reset */
            uint8_t bitWillDetach : 1;
            uint8_t reserved : 4; /*!< reserved */
        } bitFields;
    } bmAttributesUnion;
    uint8_t wDetachTimeOut[2]; /*!< Time, in milliseconds, that the device will wait after receipt of the DFU_DETACH
                                  request.*/
    uint8_t wTransferSize[2];  /*!< Maximum number of bytes that the device can accept per control-write transaction.*/
    uint8_t bcdDFUVersion[2];  /*!< Numeric expression identifying the version of the DFU Specification release.*/
} usb_host_dfu_functional_descriptor_t;

typedef struct _usb_host_dfu_status
{
    uint8_t bStatus; /*!< An indication of the status resulting from the execution of the most recent request.*/
    uint8_t bwPollTimeout[3]; /*!< Minimum time, in milliseconds, that the host should wait before sending a subsequent
                                 DFU_GETSTATUS request.*/
    uint8_t bState; /*!< An indication of the state that the device is going to enter immediately following transmission
                       of this response.*/
    uint8_t iString; /*!< Index of status description in string table. */
} usb_host_dfu_status_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name USB host DFU class APIs
 * @{
 */

/*!
 * @brief Initializes the DFU instance.
 *
 * This function allocate the resource for the DFU instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[out] classHandle   Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostDfuInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief Sets the interface.
 *
 * This function binds the interface with the DFU instance.
 *
 * @param[in] classHandle      The class handle.
 * @param[in] interfaceHandle  The interface handle.
 * @param[in] alternateSetting The alternate setting value.
 * @param[in] callbackFn       This callback is called after this function completes.
 * @param[in] callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Busy           Callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  Callback return status, the transfer is stalled by the device.
 * @retval kStatus_USB_Error          Callback return status, open pipe fail. See the USB_HostOpenPipe.
 */
extern usb_status_t USB_HostDfuSetInterface(usb_host_class_handle classHandle,
                                            usb_host_interface_handle interfaceHandle,
                                            uint8_t alternateSetting,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Deinitializes the DFU instance.
 *
 * This function frees the resources for the DFU instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[in] classHandle    The class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
extern usb_status_t USB_HostDfuDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief DFU get report descriptor.
 *
 * This function implements the DFU report descriptor request.
 *
 * @param[in] classHandle         The class handle.
 * @param[out] dfuFunctionalDesc        The return descriptor.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_Error          Don't find dfu functional desciptor.
 */
extern usb_status_t USB_HostDfuGetFunctionalDescriptor(usb_host_class_handle classHandle,
                                                       uint8_t **dfuFunctionalDesc,
                                                       uint32_t *dfuFunctionalDescLength);

/*!
 * @brief DFU detach.
 *
 * This function implements the DFU class-specific request (DFU_DETACH).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] timeout       The timeout field is specified in units of milliseconds and represents the amount of time
 that the device should wait for the pending USB reset before giving up and terminating the operation. timeout should
 not contain a value larger than the value specified in wDetachTimeout.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostDfuDetach(usb_host_class_handle classHandle,
                                      uint16_t timeout,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam);

/*!
 * @brief DFU download.
 *
 * This function implements the DFU class-specific request (DFU_DNLOAD).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[in] blockNum      It increments each time a block is transferred,
                            wrapping to zero from 65,535. It is used to provide useful context to the DFU loader in the
 device.
 * @param[in] firmwareData  The data buffer.
 * @param[in] firmwareLength The data length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostDfuDnload(usb_host_class_handle classHandle,
                                      uint16_t blockNum,
                                      uint8_t *firmwareData,
                                      uint32_t firmwareLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam);

/*!
 * @brief DFU upload.
 *
 * This function implements the DFU class-specific request (DFU_UPLOAD).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] blockNum      It increments each time a block is transferred,
                            wrapping to zero from 65,535. It is used to provide useful context to the DFU loader in the
 device.
 * @param[in] firmwareData  The received data buffer.
 * @param[in] firmwareLength The data length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostDfuUpload(usb_host_class_handle classHandle,
                                      uint8_t blockNum,
                                      uint8_t *firmwareData,
                                      uint32_t firmwareLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam);

/*!
 * @brief DFU get status.
 *
 * This function implements the DFU class-specific request (DFU_GETSTATUS).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] statusData    The received data buffer, the data length is 6.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostDfuGetStatus(usb_host_class_handle classHandle,
                                         uint8_t *statusData,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*!
 * @brief DFU clear status.
 *
 * This function implements the DFU class-specific request (DFU_CLRSTATUS).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostDfuClrStatus(usb_host_class_handle classHandle,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*!
 * @brief DFU get state.
 *
 * This function implements the DFU class-specific request (DFU_GETSTATE).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] state         The received state data.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostDfuGetState(usb_host_class_handle classHandle,
                                        uint8_t *state,
                                        transfer_callback_t callbackFn,
                                        void *callbackParam);

/*!
 * @brief DFU abort.
 *
 * This function implements the DFU class-specific request (DFU_ABORT).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostDfuAbort(usb_host_class_handle classHandle,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam);

/*! @}*/

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* _USB_HOST_DFU_H_ */
