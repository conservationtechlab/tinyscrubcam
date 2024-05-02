/*
 * Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 * Defines BCM920739 WICED application framework functions
 */
#include "wiced.h"
#include "config.h"
#include "platform.h"
#include "waf_platform.h"
#include "wiced_framework.h"
#include "wiced_apps_common.h"
#include "brcm_fw_types.h"


/******************************************************
 *                      Macros
 ******************************************************/

//! Config item code used to write an arbitrary chunk of data to an arbitrary address.  The two
//! fields are a 32-bit address (named address in ENTRY constructs), and data with arbitrary length.
//! The length is implied by the size of the array specified for data, as in data = {0x01, 0x02};
#define FOUNDATION_CONFIG_ITEM_ID_DATA                                                      0x03

//! Config item code used to call a function directly while processing config data.  It takes a
//! single parameter in such an ENTRY: construct, which is function_addresss.
#define FOUNDATION_CONFIG_ITEM_ID_FUNCTION_CALL                                             0x06

//! Config item code for the FOUNDATION_CONFIG_ITEM_PROCESS_HEADER_TABLE_POINTER.
#define FOUNDATION_CONFIG_ITEM_PROCESS_HEADER_TABLE_POINTER                                0x2D

//! Config item code used to write an arbitrary chunk of data to an arbitrary address.  This will only
//! happen on the first cold boot and not subsequent HID OFF transitions.  Its purpose is for loading AON
//! a single time.  The two fields are a 32-bit address (named address in ENTRY constructs), and data
//! with arbitrary length. The length is implied by the size of the array specified for data, as in
//! data = {0x01, 0x02};
#define FOUNDATION_CONFIG_ITEM_ID_AON_DATA                                                      0x2B


/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
    /** Boot mode */
    typedef enum
    {
        WICED_SLEEP_COLD_BOOT, /**< Cold boot */
        WICED_SLEEP_FAST_BOOT  /**< Fast boot */
    }wiced_sleep_boot_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *              Function Definitions
 ******************************************************/

/******************************************************
 *              Global Variables
 ******************************************************/

/******************************************************
 *              Function Declarations
  ******************************************************/
extern wiced_sleep_boot_type_t wiced_sleep_get_boot_mode(void);

static void ReadRaw( int offset, config_section_id_t which_section, BYTE* buffer, int length );
static void foundation_HandleVoidFunctionCall(  int header_offset,
                                                int payload_offset,
                                                config_section_id_t which_section,
                                                int payload_length,
                                                uint32_t *entry_point);
static int foundation_HandleRawData(   int header_offset,
                                        int payload_offset,
                                        config_section_id_t which_section,
                                        int payload_length );

static int FinishReadingLEBEncodedSequence( int offset_to_read_more_if_needed,
                                            config_section_id_t which_section,
                                            BYTE* data_8_byte_array,
                                            int num_valid_data_bytes,
                                            UINT32* lebs,
                                            int num_lebs_to_read );
static UINT8 ReadSSorDSItemHeader(  int offset,
                                    config_section_id_t which_section,
                                    UINT32* group_id,
                                    UINT32* item_id,
                                    UINT32* payload_len );

static void process_section( config_section_id_t which_section, uint32_t only_seek_crystal_freq, uint32_t offset, uint32_t *entry_point );

void platform_start_app( uint32_t entry_point );
wiced_result_t platform_load_app (uint32_t source, uint32_t* startaddr);

extern int32_t ef_read_buffer_in_byte( uint32_t offset, uint8_t* buffer, uint32_t length );
extern BOOL32 mmu_IsMemoryByteAddressable(IN BYTE* address);
/******************************************************
 *                 DCT Functions
 ******************************************************/

static void ReadRaw( int offset, config_section_id_t which_section, BYTE* buffer, int length )
{

    int32_t ret;

    UNUSED_PARAMETER(which_section);

//read from ocf

//    printf ("%s: reading offset 0x%x len %d\r\n",__func__,offset,length);

    ret = ef_read_buffer_in_byte((UINT32)offset & 0xFFFFF, (UINT8*)buffer, (UINT32)length);

    if(ret!=0)
        printf ("%s: Error ef read - %d\r\n",__func__,(int)ret);

}

static void foundation_HandleVoidFunctionCall(  int header_offset,
                                                int payload_offset,
                                                config_section_id_t which_section,
                                                int payload_length,
                                                uint32_t *entry_point)
{

#ifdef FLASH_BUILD
    // For OCF build, we use Function Call to branch from ROM.  When parsing config from OCF, do not branch again.
    return;
#else
    UINT32 address;
    UNUSED_PARAMETER(header_offset);
//    printf ("%s: Entry !!! \r\n",__func__);
    *entry_point = 0;
    if(payload_length != 4)
    {
        printf ("Bad void function call\r\n");
        return;

        //hsr_raise_critical_flag(HSR_FCG_CRIT_0, FCG_C0_BADVOID_VOIDFUNCTIONCALL);
        //FATAL("Bad void function call");
    }
    ReadRaw( payload_offset, which_section, (BYTE*)&address, 4 );

    //address -= 1;//HACK
//    printf (" function ptr1: 0x%x\r\n",address);
    *entry_point = address;

#if 0
    // Ensure that the pointer is for thumb mode
    address |= 1;

    //printf (" function ptr2: 0x%x\r\n",address);

    ( ( void(*)(void) )address)();
#endif
#endif
}

static int foundation_HandleRawData(   int header_offset,
                                        int payload_offset,
                                        config_section_id_t which_section,
                                        int payload_length )
{


    UINT32 address;
    UNUSED_PARAMETER(header_offset);
//    printf ("%s: Entry !!! \r\n",__func__);
    if(payload_length < 5)
    {
        printf ("Bad raw data\r\n");
        return -1;
    }
    ReadRaw( payload_offset, which_section, (BYTE*)&address, 4 );
    payload_offset += 4;
    payload_length -= 4;

//    printf ("address to copy 0x%x length: %d \r\n",address, payload_length);

    if( mmu_IsMemoryByteAddressable( (BYTE*)address ) )
    {
        ReadRaw( payload_offset, which_section, (BYTE*)address, payload_length );
//        wiced_bt_trace_array("copypayload ",(uint8_t*)address,8);
    }
    else
    {
//        printf ("NOT in RAM ADDRESS [0x%x] [%d] \r\n",address,payload_length);
        if(address % 4 || payload_length % 4)
        {
            printf ("Bad raw data\r\n");
            return -1;
            //hsr_raise_critical_flag(HSR_FCG_CRIT_0, FCG_C0_BADRAWDATA_RAWDATA_2);
            //FATAL("Bad raw data");
        }
        while(payload_length)
        {
            UINT32 buffer[256/4];
            UINT32 chunk_length = MIN( (uint32_t)payload_length, sizeof(buffer) );
            UINT32 i;

            ReadRaw( (int)payload_offset, (int)which_section, (BYTE*)buffer, (int)chunk_length );
//            wiced_bt_trace_array("copypayload ",(uint8_t*)buffer,8);
            for( i = 0; i < chunk_length/4; i++ )
            {
                REG32(address + 4*i) = buffer[i];
            }
            address += chunk_length;
            payload_offset += (int)chunk_length;
            payload_length -= (int)chunk_length;
        }
    }
    return 0;
}


static int FinishReadingLEBEncodedSequence( int offset_to_read_more_if_needed,
                                            config_section_id_t which_section,
                                            BYTE* data_8_byte_array,
                                            int num_valid_data_bytes,
                                            UINT32* lebs,
                                            int num_lebs_to_read )
{
    int header_len = 0;
    for(;;)
    {
        for(;;)
        {
            // See if we have a complete LEB-encoded sequence
            UINT8 byte_i, leb_len;
            UINT32 result;

            for( byte_i = 0; byte_i < num_valid_data_bytes; byte_i++ )
            {
                if(data_8_byte_array[byte_i] & 0x80)
                {
                    // Incomplete at this byte
                    if(byte_i == 4)
                    {
                        printf ("%s: LEB encoded sequence is too large for a UINT32\r\n",__func__);
                        // LEB encoded sequence is too large for a UINT32
                        return -1;
                    }
                    continue;
                }
                // Complete at this byte
                break;
            }

            if(byte_i == num_valid_data_bytes)
            {
                // There isn't enough data available yet to decode an LEB-128 encoded sequence into
                // a UINT32.
                int extend_bytes;

                // We can safely extend by up to the lesser of num_lebs_to_read or the remaining
                // unfilled size of the 8 byte data array.
                extend_bytes = MIN( 8-num_valid_data_bytes, num_lebs_to_read );
                ReadRaw( offset_to_read_more_if_needed,
                                which_section,
                                data_8_byte_array+num_valid_data_bytes,
                                extend_bytes );

//                wiced_bt_trace_array("[LEBS] ",(data_8_byte_array+num_valid_data_bytes),(uint16_t)extend_bytes);

                num_valid_data_bytes += extend_bytes;
                offset_to_read_more_if_needed += extend_bytes;
                continue;
            }

            // We have a complete sequence.  byte_i indicates where the first byte was found
            // without bit 7 set.
            leb_len = (uint8_t)(byte_i + 1);
            if(leb_len == 1)
            {
                // Quick case
                result = data_8_byte_array[0];
            }
            else
            {
                int shift;

                if( leb_len == 5 && (data_8_byte_array[4] & 0xf0) )
                {
                    // LEB encoded sequence is too large for a UINT32
                    return -1;
                }

                result = 0;
                shift = 0;
                for( byte_i = 0; byte_i < leb_len; byte_i++ )
                {
                    UINT32 byteval = data_8_byte_array[byte_i];
                    result |= ( (byteval & 0x7f) << shift );
                    shift += 7;
                }
            }

            // Record the result, update the state for the remainder of the sequence
            *lebs++ = result;
            header_len += leb_len;
            num_lebs_to_read--;
            if(!num_lebs_to_read)
            {
                return header_len;
            }
            num_valid_data_bytes -= leb_len;
            if(num_valid_data_bytes)
            {
                memmove( data_8_byte_array, data_8_byte_array+leb_len, (size_t)num_valid_data_bytes );
            }
            // Break from the loop that makes sure we have a complete sequence and retries
            // (continues) if insufficient data is available.  This will break us out to the loop
            // which reads LEB-encoded values.  That loop continues until num_lebs_to_read is 0.
            break;
        }
    }
    return -1;
}



static UINT8 ReadSSorDSItemHeader(  int offset,
                                    config_section_id_t which_section,
                                    UINT32* group_id,
                                    UINT32* item_id,
                                    UINT32* payload_len )
{
    UINT8 data[8];
    int valid_bytes;
    UINT32 lebs[3]={0,0,0};
    int len;

    // Media will always be a multiple of 1K.  If we're approaching a 1K boundary, read only as much
    // as is safe up to the page boundary.  A typical header is 3 bytes, but if the item ID and/or
    // length is greater than 127, which is often the case, we'll end up needing one or two extra
    // bytes to read the complete item header.  Reading up to 4 bytes is a safe bet for what will be
    // optimal.  If the header is only 3 bytes, reading the extra byte won't take much time.  If the
    // header is 5 bytes, that's a rare enough occurrance to not bother reading all 5 upfront, since
    // the two extra bytes read for every item header, when most headers will be 3 or 4 bytes, will
    // add up.
    valid_bytes = 1024 - offset%1024;
    valid_bytes = MIN( valid_bytes, 3 );
    ReadRaw( offset, which_section, data, valid_bytes );

//    wiced_bt_trace_array("[header] ",data,(uint16_t)valid_bytes);

    // if crossing the page boundary, need to read up the remaining bytes to ensure 3 bytes header
    if( valid_bytes < 3)
    {
        ReadRaw( offset+valid_bytes, which_section, &data[valid_bytes], 3-valid_bytes );
        valid_bytes = 3;
    }

    // item Id will be the first byte and then group Id.
    *item_id = data[0];
    *group_id = data[1];

    // LEB-128 decoding the variable length
    len = FinishReadingLEBEncodedSequence(  offset+valid_bytes,
                                                    which_section,
                                                    &data[2],
                                                    1,
                                                    lebs,
                                                    1 );

    if(len == -1)
    {
        printf("Corrupt SS/DS item header");
    }

    len = len + 2;

    *payload_len = lebs[0];

    return (UINT8)len;
}

static void process_section( config_section_id_t which_section, uint32_t only_seek_crystal_freq, uint32_t offset, uint32_t *entry_point )
{
    uint32_t curr_item_sec_offset;

    /* DEBUG */
    BYTE sig_check[8];
//    printf ("%s: Entry !!! \n\r",__func__);

        *entry_point = 0;
    ReadRaw( (int)offset, (int)which_section, sig_check, 8 );
//    wiced_bt_trace_array("[debug header] ",sig_check,8);
    if( memcmp( sig_check,
                    (which_section == CONFIG_STATIC) ? SS_SIGNATURE : DS_SIGNATURE,
                    8 ) != 0 )
    {
        printf ("%s: Invalid content !!! section [%d] \r\n",__func__,which_section);
    }
/* END DEBUG */

    // Skip past the signature
    curr_item_sec_offset = offset + 8;
    //curr_item_sec_offset = 8;

    // Skip past the checksum and section length
    curr_item_sec_offset += 8;

    for(;;)
    {
        // Each item starts with three LEB-128 encoded values: group ID, item ID, length
        uint32_t group_id, item_id, payload_len;
        uint32_t header_size = ReadSSorDSItemHeader(   (int)curr_item_sec_offset,
                                                            which_section,
                                                            &group_id,
                                                            &item_id,
                                                            &payload_len );

//        printf ("%s: grpid[%d] itemid[%d] payloadlen[%d] headersize[%d] \r\n",__func__,group_id,item_id,payload_len,header_size);
        // See if that was the end of the section
        if((group_id == CONFIG_GROUP_SECTION_END) && (item_id == CONFIG_ITEM_SECTION_END))
        {
            if (only_seek_crystal_freq)
            {
                printf ("Error !!! \r\n");
            }
            break;
        }

        //TODO: handle the config item FOUNDATION_CONFIG_ITEM_PROCESS_HEADER_TABLE_POINTER

        // Find out how to handle this item
        // only supported foundation group id
        switch (group_id)
        {
            case CONFIG_GROUP_ID_FOUNDATION:
            {
                switch (item_id)
                {
                    case FOUNDATION_CONFIG_ITEM_ID_DATA:
                        foundation_HandleRawData((int)curr_item_sec_offset,(int)(curr_item_sec_offset+header_size),(int)which_section,(int)payload_len);
                        break;
                    case FOUNDATION_CONFIG_ITEM_ID_AON_DATA:
                        if(wiced_sleep_get_boot_mode() != WICED_SLEEP_FAST_BOOT )
                        {
                            foundation_HandleRawData((int)curr_item_sec_offset,(int)(curr_item_sec_offset+header_size),(int)which_section,(int)payload_len);
                        }
                        break;
                    case FOUNDATION_CONFIG_ITEM_ID_FUNCTION_CALL:
                        foundation_HandleVoidFunctionCall((int)curr_item_sec_offset,(int)(curr_item_sec_offset+header_size),which_section,(int)payload_len, entry_point);
                        break;
                    default:
                        printf ("item id %d is not handled \r\n",(int)item_id);
                }
            }
                break;
            default:
                printf ("group id %d is not handled \r\n",(int)group_id);
        }



        // Move to the next item, unless we found crystal frequency info and that's all we needed
        // for now
        curr_item_sec_offset = curr_item_sec_offset + header_size + payload_len;
    }
}

void platform_start_app( uint32_t entry_point )
{
    // Ensure that the pointer is for thumb mode
    entry_point |= 1;

    //printf (" function ptr2: 0x%x\r\n",address);

    ( ( void(*)(void) )entry_point)();
}



wiced_result_t platform_load_app (uint32_t source, uint32_t* startaddr)
{
    process_section(CONFIG_DYNAMIC, WICED_FALSE, source, startaddr);
    return WICED_SUCCESS;
}

