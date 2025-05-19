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


#include "bignum.h"

/*
 * Greatest common divisor: G = gcd(A, B)  (HAC 14.54)
 */
int32_t mpi_gcd(mpi *G, const mpi *A, const mpi *B)
{
    int32_t ret, lz, lzt;
    mpi TG, TA, TB;

    mpi_init(&TG); mpi_init(&TA); mpi_init(&TB);

    MPI_CHK(mpi_copy(&TA, A));
    MPI_CHK(mpi_copy(&TB, B));

    lz = (int32_t)mpi_lsb(&TA);
    lzt = (int32_t)mpi_lsb(&TB);

    if (lzt < lz)
        lz = lzt;

    MPI_CHK(mpi_shift_r(&TA, lz));
    MPI_CHK(mpi_shift_r(&TB, lz));

    TA.s = TB.s = 1;

    while (mpi_cmp_int(&TA, 0) != 0) {
        MPI_CHK(mpi_shift_r(&TA, (int32_t)mpi_lsb(&TA)));
        MPI_CHK(mpi_shift_r(&TB, (int32_t)mpi_lsb(&TB)));

        if (mpi_cmp_mpi(&TA, &TB) >= 0) {
            MPI_CHK(mpi_sub_abs(&TA, &TA, &TB));
            MPI_CHK(mpi_shift_r(&TA, 1));
        } else {
            MPI_CHK(mpi_sub_abs(&TB, &TB, &TA));
            MPI_CHK(mpi_shift_r(&TB, 1));
        }
    }

    MPI_CHK(mpi_shift_l(&TB, lz));
    MPI_CHK(mpi_copy(G, &TB));

cleanup:

    mpi_free(&TB); mpi_free(&TA); mpi_free(&TG);

    return (ret);
}
