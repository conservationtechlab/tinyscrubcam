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
#include "nx_api.h"
#include "netx_applications/dhcp/nxd_dhcp_client.h"

#define UNUSED_ARG(x) ((void)(x))

extern UINT  _nxe_dhcp_user_option_retrieve(NX_DHCP *dhcp_ptr, UINT option_request, UCHAR *destination_ptr, UINT *destination_size);
/*These functions are stubs for the IPV6 Netx Duo
 * features which are not enabled in the Netx_Duo ROM code */

UINT  nxd_mld_multicast_join(NX_IP *ip_ptr, ULONG* group_address)
{
    UNUSED_ARG(ip_ptr);
    UNUSED_ARG(group_address);
    return NX_NOT_SUPPORTED;
}

UINT  nxd_mld_multicast_leave(NX_IP *ip_ptr, ULONG* group_address)
{
    UNUSED_ARG(ip_ptr);
    UNUSED_ARG(group_address);
    return NX_NOT_SUPPORTED;
}

UINT  nxd_mld_enable(NX_IP *ip_ptr)
{
    UNUSED_ARG(ip_ptr);
    return NX_SUCCESS;
}

/*Netx_Duo ROM function provides reverse order result,we implement this
 * function to correct it*/
UINT  nxe_dhcp_user_option_retrieve(NX_DHCP *dhcp_ptr, UINT option_request, UCHAR *destination_ptr, UINT *destination_size)
{
    UINT    status;
    ULONG reverse_IP_address;

    /*This will be call to the ROM Netx_Duo*/
    status = _nxe_dhcp_user_option_retrieve(dhcp_ptr, option_request, destination_ptr, destination_size);
    if(status != NX_SUCCESS)
        return status;

    // Convert all DNS Server addresses
    for ( UINT i = 0; i < *destination_size; i++ )
    {
        UINT index = i * 4;

        reverse_IP_address = nx_dhcp_user_option_convert( destination_ptr + index );

        destination_ptr[ index + 3 ] = (UCHAR) ( reverse_IP_address >> 24 ) & 0xFF;
        destination_ptr[ index + 2 ] = (UCHAR) ( reverse_IP_address >> 16 ) & 0xFF;
        destination_ptr[ index + 1 ] = (UCHAR) ( reverse_IP_address >> 8 ) & 0xFF;
        destination_ptr[ index + 0 ] = (UCHAR) reverse_IP_address & 0xFF;

    }

    return status;
}


