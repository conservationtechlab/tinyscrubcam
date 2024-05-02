/**
 * \file des.h
 *
 * \brief DES block cipher
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#ifndef DES_ALT_H
#define DES_ALT_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>
#include <stdint.h>
/* WICED_MBEDTLS Start */
#include "crypto_structures.h"
/* MBEDTLS_DES_ENCRYPT, MBEDTLS_DES_DECRYPT, MBEDTLS_DES_KEY_SIZE
 * and MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH
 * Macros have been moved to crypto_structures.h
 */

/* WICED_MBEDTLS End */

#endif /* des.h */
