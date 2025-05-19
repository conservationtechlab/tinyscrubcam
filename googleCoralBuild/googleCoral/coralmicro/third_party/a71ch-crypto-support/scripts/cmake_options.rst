.. Copyright 2018,2020 NXP
..
.. SPDX-License-Identifier: Apache-2.0
..

.. _se05x-cmake-options:

============================================
 CMake Options
============================================

.. include:: cmake_options_values.rst.txt


.. option:: WithNXPNFCRdLib

    - Compile in NXP NFC RdLib support
    - Default is OFF

    - Use NXP NFC RdLib. This is used mainly for RC663 + SAM Use Cases. Package available under NDA is needed to use this feature



.. option:: WithOPCUA_open62541

    - Compile With open62541 Support
    - Default is OFF

    - Compile with OPC UA.  By default it is disabled from compilation.



.. option:: WithSharedLIB

    - Create and use shared libraries
    - Default is OFF

    - Create shared libraries.  Applicable for Engine DLL and other use cases.



NXP Internal Options
============================================

These options are not supported outside NXP.

.. option:: NXPInternal

    - NXP Internal
    - Default is OFF. (ON only within NXP)

    .. note:: For deliveries outside NXP, this option is disabled.



.. option:: WithCodeCoverage

    - Compile with Code Coverage
    - Default is OFF

    .. For internal testing.




.. Automatic generated options

Other Variables
============================================


.. option:: WithExtCustomerCode

    - Include code from ../customer
    - Default is OFF

    - Include code from external folder. This way, experimental code can be included in build from outside the simw-top repository.



.. option:: SIMW_INSTALL_INC_DIR

    - Location where library header files are installed for linux based targets. (Used for iMX Linux)
    - Default location is ``</usr/local/>include/se05x``


.. option:: SIMW_INSTALL_SHARE_DIR

    - Location where miscellaneous scripts
      get copiled for linux based targets. (Used for iMX Linux)
    - e.g. ``cmake_options.mak`` which has current cmake build settings.
    - Default location is ``</usr/local/>share/se05x``


.. # End of documented part .....


.. # WithLargePrograms: Large enough programs that may
.. # not fit in smaller size, e.g. small boot loaders.

.. To build shared libraries

.. If using NDA Version of WithNXPNFCRdLib




.. for Legacy reasons, setting SSS_HAVE_OpenSSL_1_1_1 = ON























.. Checks and balances ########






















.. By default, we don't have it
.. SET(SSS_HAVE_FIPS
..     OFF
..     CACHE BOOL "Compile Tests with/without FIPS Mode aware"
.. )

.. Based on variant, we enable and expose that feature.








