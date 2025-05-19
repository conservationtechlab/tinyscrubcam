..
    Copyright 2020 NXP

    SPDX-License-Identifier: Apache-2.0


.. highlight:: c

.. _sss-malloc-macros:

=================================================
 SSS Heap Management
=================================================

For effective heap management, macros ``SSS_MALLOC``, ``SSS_CALLOC`` 
and ``SSS_FREE`` are available in :file:`sss/port/ksdk/fsl_sss_types.h` 
for embedded build and in :file:`sss/port/default/fsl_sss_types.h` 
for PC/Linux build.

.. literalinclude:: ../port/default/fsl_sss_types.h
   :language: c
   :start-after: /* doc:start:sss-heap_mgmt */
   :end-before: /* doc:end:sss-heap_mgmt */

These macros are used in SSS for heap management 
operations and can be defined as platform dependent 
implementations of malloc, calloc and free APIs.
