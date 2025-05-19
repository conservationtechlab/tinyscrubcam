/****************************************************************************
**
**  Name:       srvc_a4wp_api.h
**  Function    This file contains the definitions for the A4WP API
**
**
**  Copyright (c) 1999-2013, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
******************************************************************************/
#ifndef SRVC_A4WP_API_H
#define SRVC_A4WP_API_H

#if BLE_INCLUDED == TRUE

#include "mpaf/apps/bt_target.h"
#include "gatt_api.h"
#include "gattdefs.h"
#include "legacy_interface/inc/types.h"

#define A4WP_MAX_CHAR_NUM            5

/* A4WP Characteristic value formats */
#define A4WP_ENABLE_BIT(val, mask) ((val) = (val) | (mask))
#define A4WP_DISABLE_BIT(val, mask) ((val) = (val) & (~(mask)))

/* Bit masks for PRU Enables values */
//Enable PRU output (bit 7)
//Enable PRU charge indicator (Bit 6)
//Reduce power command (bits 5 and 4)
//RFU (bits 3 to 0)
#define A4WP_PRU_CTRL_EN_OUTPUT_MASK            0x80
#define A4WP_PRU_CTRL_EN_CH_INDICATOR_MASK      0x40
#define A4WP_PRU_CTRL_EN_POWER_MASK             0x30

/* Enable/disable Output */
#define A4WP_PRU_CTRL_ENABLE_OUTPUT(val)     A4WP_ENABLE_BIT((val), A4WP_PRU_CTRL_EN_OUTPUT_MASK)
#define A4WP_PRU_CTRL_DISABLE_OUTPUT(val)    A4WP_DISABLE_BIT((val), A4WP_PRU_CTRL_EN_OUTPUT_MASK)

/* Enable/disable charge indicator */
#define A4WP_PRU_CTRL_ENABLE_CH_INDICATOR(val)   A4WP_ENABLE_BIT((val), A4WP_PRU_CTRL_EN_CH_INDICATOR_MASK)
#define A4WP_PRU_CTRL_DISABLE_CH_INDICATOR(val)  A4WP_DISABLE_BIT((val), A4WP_PRU_CTRL_EN_CH_INDICATOR_MASK)

/* Power commands */
//00 = Maximum power
//01 = 66% * PRECT_MAX
//10 = 33% * PRECT_MAX
//11 = 2.5W
#define A4WP_PRU_CTRL_SET_MAX_POWER(val)    A4WP_DISABLE_BIT((val), A4WP_PRU_CTRL_EN_POWER_MASK)
#define A4WP_PRU_CTRL_SET_66P_POWER(val)    (A4WP_DISABLE_BIT((val), A4WP_PRU_CTRL_EN_POWER_MASK) | (1 << 4))
#define A4WP_PRU_CTRL_SET_33P_POWER(val)    (A4WP_DISABLE_BIT((val), A4WP_PRU_CTRL_EN_POWER_MASK) | (1 << 5))
#define A4WP_PRU_CTRL_SET_2_5W_POWER(val)   A4WP_ENABLE_BIT((val), A4WP_PRU_CTRL_EN_POWER_MASK)

/* Permission bit values */
#define A4WP_PRU_CTRL_PERM_WO_REASON        0x00    //Permitted without reason
#define A4WP_PRU_CTRL_PERM_WT_WAIT_TIME     0x01    //Permitted with waiting time due to limited affordable power

#define A4WP_PRU_CTRL_DENY_BIT              0x80    // If this bit is set, then permission is denied.
#define A4WP_PRU_CTRL_DENY_WT_SYSY_ERROR    0x80    //Denied with system error 3 described in Section 5.2.8.7.
#define A4WP_PRU_CTRL_DENY_DT_LIMIT_POWER   0x81    //Denied due to limited affordable power 
#define A4WP_PRU_CTRL_DENY_DT_PTU_NO_DEV    0x82    //Denied due to limited PTU Number of Devices 
#define A4WP_PRU_CTRL_DENY_DT_PTU_CLASS_SUP 0x83    //Denied due to limited PTU Class support 

/* Time set values */
// 0000 0000b -> (0)    RFU
// 0000 0001b -> (1)   10ms
// 0000 0010b -> (2)   20ms
// 0000 0011b -> (3)   30ms
// 0000 0100b -> (4)   40ms
// 0000 0101b -> (5)   50ms
// 0000 0110b -> (6)   60ms
// 0000 0111b -> (7)   70ms
// 0000 1000b -> (8)   80ms
// All other value are RFU
#define A4WP_PRU_CTRL_TIME_SET_MAX_VALUE    (8)

#pragma pack(1)
/* PRU Control characteristic value format (5 octets) */
PACKED struct A4WP_PRU_CTRL_CHAR_VAL
{
    PACKED union
    {
        PACKED struct 
        {
            UINT8  rfu:4;
            UINT8  pwrcmd:2;
            UINT8  charging:1;
            UINT8  output:1;
        }s;
        UINT8  byte;
    }enables;
    UINT8  permission;
    UINT8  time_set;  //(0 - 255)
    UINT8  rfu[2];
};
typedef struct A4WP_PRU_CTRL_CHAR_VAL tA4WP_PRU_CTRL_CHAR_VAL;

#pragma pack()

/* Bit masks for PTU Static parameter optional field support */
//Max Impedance (bit 7)
//Max Resistance (Bit 6)
//RFU (bits 5 to 0)
#define A4WP_PTU_STATIC_MAX_IMPEDANCE_MASK      0x80
#define A4WP_PTU_STATIC_MAX_RESISTANCE_MASK     0x40

/* PTU Power  */
// The bits 7 to 2 are used for PTU power. Bits 1 and 0 are RFU.
/* Supported values:
|   0   ->  0.5     |   1   ->  1.0     |   2   ->  1.5     |
|   3   ->  2.0     |   4   ->  2.5     |   5   ->  3.0     |
|   6   ->  3.5     |   7   ->  4.0     |   8   ->  4.5     |
|   9   ->  5.0     |   10  ->  5.5     |   11  ->  6.0     |
|   12  ->  6.5     |   13  ->  7.0     |   14  ->  7.5     |
|   15  ->  8.0     |   16  ->  8.5     |   17  ->  9.0     |
|   18  ->  9.5     |   19  ->  10.0    |   20  ->  11.0    |
|   21  ->  12.0    |   22  ->  13.0    |   23  ->  14.0    |
|   24  ->  15.0    |   25  ->  16.0    |   26  ->  17.0    |
|   27  ->  18.0    |   28  ->  19.0    |   29  ->  20.0    |
|   30  ->  21.0    |   31  ->  22.0    |   32 to 61 -> RFU |
*/
#define A4WP_PTU_STATIC_SET_POWER(val) (val) = (val) << 2

/* PTU max source impedance */
// The bits 7 to 3 are used for PTU max source impedance. Bits 2 to 0 are RFU.
/* Supported values:
|   0   ->  50      |   1   ->  60      |   2   ->  70      |
|   3   ->  80      |   4   ->  90      |   5   ->  100     |
|   6   ->  110     |   7   ->  120     |   8   ->  130     |
|   9   ->  140     |   10  ->  150     |   11  ->  175     |
|   12  ->  200     |   13  ->  225     |   14  ->  250     |
|   15  ->  275     |   16  ->  300     |   17  ->  350     |
|   18  ->  375     |   19 to 31 -> RFU |
*/
#define A4WP_PTU_STATIC_MAX_SRC_IMPEDANCE(val) (val) = (val) << 3

/* PTU max load resistance */
// The bits 7 to 4 are used for PTU max load resistance. Bits 3 to 0 are RFU.
/* Supported values:
|   0   ->  5       |   1   ->  10      |   2   ->  15      |
|   3   ->  20      |   4   ->  25      |   5   ->  30      |
|   6   ->  35      |   7   ->  40      |   8   ->  45      |
|   9   ->  50      |   10  ->  55      |   11 to 15 -> RFU |
*/
#define A4WP_PTU_STATIC_MAX_LOAD_RESISTANCE(val) (val) = (val) << 4

/* PTU Number of devices */
/* Supported values:
|   0   ->  1       |   1   ->  2       |   2   ->  3       |
|   4   ->  5       |   5   ->  6       |   6   ->  7       |
|   7   ->  8       |   8 to 15 -> RFU  |
*/

/* PTU Class */
#define A4WP_PTU_CLASS_1        0x00
#define A4WP_PTU_CLASS_2        0x01
#define A4WP_PTU_CLASS_3        0x02
#define A4WP_PTU_CLASS_4        0x03
#define A4WP_PTU_CLASS_5        0x04

#pragma pack(1)
/* PTU Static Characteristic value format (17 octets) */
PACKED struct A4WP_PTU_STATIC_CHAR_VAL
{
    UINT8  opt_fields;          //Defines which fields are valid
    UINT8  power;               //Power of PTU
    UINT8  max_src_impedance;   //Maximum source impedance of the PTU
    UINT8  max_load_resistance; //Maximum load resistance of the PTU
    UINT16 rfu_1;               //RFU
    UINT8  class;               //PTU class
    UINT8  hw_rev;              //Revision of the PTU HW
    UINT8  fw_rev;              //Revision of the PTU SW
    UINT8  protocol_rev;        //Revision of the protocol
    UINT8  num_dev;             //Max # devices
    UINT8  rfu[6];              //Undefined
};
typedef struct A4WP_PTU_STATIC_CHAR_VAL tA4WP_PTU_STATIC_CHAR_VAL;

#pragma pack()

/* Bit masks for PRU Static Parameter optional fields */
//Delta R1 (bit 7)
//RFU (bits 6 to 0)
#define A4WP_PRU_STATIC_DELTA_R1_MASK        0x80

/* PRU Capability/Information (Bit masks) */
#define A4WP_PRU_INFO_NFC_RECEIVER                  0x80
#define A4WP_PRU_INFO_SEP_BTLE_RADIO                0x40

//  0 -> VRECT_MIN_ERROR Algorithm
//  1 -> Maximum System Efficiency
#define A4WP_PRU_INFO_POWER_CTRL_ALGO_PREF          0x20

//  0 -> Not supported.
//  1 -> Adjust power capability supported
#define A4WP_PRU_INFO_ADJ_POWER_CAPABILITY          0x10

//  0 -> Not supported.
//  1 -> Charge complete connected mode supported
#define A4WP_PRU_INFO_CHARGE_COMPLETE_CONN_MODE     0x08

/* PRU Category */
/* Supported values:
|   0   ->  undefined       |   1   ->  category 1  |
|   2   ->  category 2      |   3   ->  category 3  |
|   4   ->  category 4      |   5   ->  category 5  |
|   6 to 255 -> undefined   |
*/

#pragma pack(1)
/* PRU Static Characteristic value format (20 octets) */
PACKED struct A4WP_PRU_STATIC_CHAR_VAL
{
    UINT8  opt_fields;          //Defines which fields are valid
    UINT8  protocol_rev;        //Protocol revision
    UINT8  rfu_1;               //RFU
    UINT8  category;            //Category of PRU
    UINT8  capability;          //Capabilities/Information of PRU (bit field)
    UINT8  hw_rev;              //Revision of the PRU HW
    UINT8  fw_rev;              //Revision of the PRU SW
    UINT8  max_desired_power;   //Maximum power desired by PRU
    UINT16 v_rect_min;          //VRECT_MIN (static, first estimate)
    UINT16 v_rect_high;         //VRECT_HIGH (static, first estimate)
    UINT16 v_rect_set;          //VRECT_SET
    UINT16 delta_r1;            //Delta R1 caused by PRU
    UINT8  rfu_2[4];            //RFU
};
typedef struct A4WP_PRU_STATIC_CHAR_VAL tA4WP_PRU_STATIC_CHAR_VAL;
#pragma pack()

/* Bit masks for PRU Dynamic Parameter optional fields */
//RFU (bit 1 and 0)
#define A4WP_PRU_DYN_V_OUT_MASK             0x80
#define A4WP_PRU_DYN_I_OUT_MASK             0x40
#define A4WP_PRU_DYN_TEMP_MASK              0x20
#define A4WP_PRU_DYN_V_RECT_MIN_MASK        0x10
#define A4WP_PRU_DYN_V_RECT_SET_MASK        0x08
#define A4WP_PRU_DYN_V_RECT_HIGH_MASK       0x04

#pragma pack(1)

PACKED struct A4WP_PRU_DYN_CHAR_FROM_HAL
{
    UINT16 v_rect;              //Voltage in mV at diode output
    UINT16 i_rect;              //Current in mV at diode output
    UINT16 v_out;               //Voltage mV at charge/battery port
    UINT16 i_out;               //Current mA at charge/battery port
    UINT8  temperature;        //Temperature of PRU (-40C to Max)
};
typedef struct A4WP_PRU_DYN_CHAR_FROM_HAL tA4WP_PRU_DYN_CHAR_FROM_HAL;

PACKED struct A4WP_PRU_DYN_CHAR_FROM_ALGO
{
    UINT16 v_rect_min;          //VRECT_LOW_LIMIT (dynamic value)
    UINT16 v_rect_set;          //Desired VRECT (dynamic value)
    UINT16 v_rect_high;         //VRECT_HIGH_LIMIT (dynamic value)
};
typedef struct A4WP_PRU_DYN_CHAR_FROM_ALGO tA4WP_PRU_DYN_CHAR_FROM_ALGO;

/* PRU Dynamic characteristic parameter format (20 octets) */
PACKED struct A4WP_PRU_DYN_CHAR_VAL
{
    UINT8  opt_fields;          //Defines which optional fields are populated
    tA4WP_PRU_DYN_CHAR_FROM_HAL  fromHal;
    tA4WP_PRU_DYN_CHAR_FROM_ALGO fromAlgo;
    UINT8  pru_alert;           //Same alert value as alert notification.
    UINT8  rfu[3];              //Undefined
};
typedef struct A4WP_PRU_DYN_CHAR_VAL tA4WP_PRU_DYN_CHAR_VAL;
#pragma pack()

/* Bit masks for PRU Alert Notification and PRU alert value in the dynamic parameter */
#define A4WP_PRU_ALERT_OVER_VOLTAGE_MASK        0x80
#define A4WP_PRU_ALERT_OVER_CURRENT_MASK        0x40
#define A4WP_PRU_ALERT_OVER_TEMP_MASK           0x20
#define A4WP_PRU_ALERT_SELF_PROTECTION_MASK     0x10
#define A4WP_PRU_ALERT_CHARGE_COMPLETE_MASK     0x08
#define A4WP_PRU_ALERT_TA_DETECT_MASK           0x04
#define A4WP_PRU_ALERT_CHARGE_PORT              0x02
#define A4WP_PRU_ALERT_ADJ_POWER_RSP_MASK       0x01

/* Transition alerts are valid only in Alert Characteristic */
#define A4WP_PRU_ALERT_TRANSITION_3S_MASK       0x02
#define A4WP_PRU_ALERT_TRANSITION_2S_MASK       0x01
#define A4WP_PRU_ALERT_TRANSITION_6S_MASK       (A4WP_PRU_ALERT_TRANSITION_3S_MASK | A4WP_PRU_ALERT_TRANSITION_2S_MASK)
/* The last bit (0) in dynamic parameter alert value is not defined
 * and should be RFU (0)
 */
#define A4WP_PRU_DYN_ALERT_RFU_MASK             0x01
#define A4WP_PRU_ALERT_ALL_MASK                 0xFF

/* Macros to Set/Clear the alert bit fields */
#define A4WP_PRU_SET_ALERT(val, mask)           A4WP_ENABLE_BIT((val), (mask))
#define A4WP_PRU_CLEAR_ALERT(val, mask)         A4WP_DISABLE_BIT((val), (mask))

/* PRU Alert characteristic parameter format (1 octet) */
typedef UINT8 tA4WP_PRU_ALERT_CHAR_VAL;

typedef tGATT_STATUS tA4WP_STATUS;

/* A4WP callback events */
enum
{
#if (A4WP_PRU_ENABLE == TRUE)
    /* Server events */
    A4WP_EVT_READ_CH_PRU_STATIC_REQ = 1,
    A4WP_EVT_READ_CH_PRU_DYN_REQ,
    A4WP_EVT_READ_CH_PRU_ALERT_REQ,
    A4WP_EVT_READ_ALERT_CLT_CFG_REQ,
    A4WP_EVT_WRITE_CH_PRU_CTRL_REQ,
    A4WP_EVT_WRITE_CH_PTU_STATIC_REQ,
    A4WP_EVT_WRITE_ALERT_CLT_CFG_REQ,
    A4WP_EVT_HANDLE_VAL_CNF,
#endif /* (A4WP_PRU_ENABLE == TRUE) */

#if (A4WP_PTU_ENABLE == TRUE)
    /* Client events */
    A4WP_EVT_DISC_COMPLETE_IND,
    A4WP_EVT_READ_RSP,
    A4WP_EVT_WRITE_RSP,
    A4WP_EVT_ALERT_IND,
#endif /* (A4WP_PTU_ENABLE == TRUE) */

    A4WP_EVT_MAX
};

/* Response data */
typedef struct
{
    UINT16 val_len;
    UINT8 *p_val;
}tA4WP_RSP_DATA;

/* Event data */
typedef struct
{
    BD_ADDR_PTR p_bda;
    tA4WP_STATUS status;

    // TBD: Union of the next 2 fields required for space optimization
#if (A4WP_PRU_ENABLE == TRUE)
    tGATT_WRITE_REQ *p_data;
#endif
#if (A4WP_PTU_ENABLE == TRUE)
    tGATT_CL_COMPLETE *p_cl_data;
    UINT16 uuid;
#endif
}tA4WP_EVT_DATA;

/* Callback function for A4WP events */
typedef void (tA4WP_CBACK)(UINT16 event, tA4WP_EVT_DATA *p_data);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#if (A4WP_PRU_ENABLE == TRUE)
/*****************************************************************************
**  A4WP PRU (Server) Function
*****************************************************************************/

/*******************************************************************************
**
** Function         A4WP_PRU_GetAttrHandleList
**
** Description      Gets the A4WP Service characteristic handle list.
**                  Note: This has to be called only after initialising the A4WP
**                        Service. The characteristic handles are updated in CB
**                        only with A4WP_PRU_Instantiate() API.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PRU_GetAttrHandleList (UINT8 *attr_hdl);

/*******************************************************************************
**
** Function         A4WP_PRU_Instantiate
**
** Description      Initialize the A4WP PRU Service.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PRU_Instantiate (tA4WP_CBACK *p_cback);

/*******************************************************************************
**
** Function         A4WP_PRU_DeInstantiate
**
** Description      De-Initialize the A4WP PRU Service.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PRU_DeInstantiate (void);

/*******************************************************************************
**
** Function         A4WP_PRU_SendAlertIndication
**
** Description      Send the indication to the PTU device.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PRU_SendAlertIndication(BD_ADDR bda,
                                                              BOOL32 need_cfm,
                                                              tA4WP_RSP_DATA *p_data);

/*******************************************************************************
**
** Function         A4WP_PRU_SendReadResponse
**
** Description      Send the response to the Read request from PTU.
** Note:
**      Always in GATT, only one request may be pending which needs response.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PRU_SendResponse(UINT16 event,
                                                       tA4WP_STATUS st,
                                                       tA4WP_RSP_DATA *p_data);

/*******************************************************************************
**
** Function         A4WP_PRU_Disconnect
**
** Description      Disconnect the PRU from the PTU.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PRU_Disconnect(BD_ADDR bda);

#endif /* (A4WP_PRU_ENABLE == TRUE) */

#if (A4WP_PTU_ENABLE == TRUE)
/*****************************************************************************
**  A4WP PTU (Client) Function
*****************************************************************************/
/*******************************************************************************
**
** Function         A4WP_PTU_Initialize
**
** Description      Initialize the A4WP PTU Instance.
** Note:
**      This function should be the first one to be called before any client
**      APIs to register the callback.
**
**      The application should establish the GATT connection in case of A4WP to
**      maintain the connection. The Read/Write APIs will logically establish
**      GATT channels and release them after the operation is completed.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PTU_Initialize (tA4WP_CBACK *p_cback);

/*******************************************************************************
**
** Function         A4WP_PTU_DeInitialize
**
** Description      De-Initialize the A4WP PTU Instance.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PTU_DeInitialize (void);

/*******************************************************************************
**
** Function         A4WP_PTU_Discover
**
** Description      Discover all the services of the A4WP PRU service and registers
**                  for the Alert notification.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PTU_Discover(BD_ADDR peer_bda,
                                                   BOOL32 reg_notfy);

/*******************************************************************************
**
** Function         A4WP_PTU_Read
**
** Description      Read the specific attribute value from the PRU device.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PTU_Read(BD_ADDR peer_bda,
                                               UINT16 uuid);

/*******************************************************************************
**
** Function         A4WP_PTU_Write
**
** Description      Write the specific attribute value in the PRU device.
**
*******************************************************************************/
    GATT_API extern tA4WP_STATUS A4WP_PTU_Write(BD_ADDR peer_bda,
                                                UINT16 uuid,
                                                tA4WP_RSP_DATA *p_data);
                                                
#endif /* (A4WP_PTU_ENABLE == TRUE) */

#ifdef __cplusplus
}
#endif

#endif /* #if BLE_INCLUDED == TRUE */
#endif
