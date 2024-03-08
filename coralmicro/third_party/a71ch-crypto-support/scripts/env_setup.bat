@echo off
@REM Copyright 2018,2019 NXP
@REM
@REM SPDX-License-Identifier: Apache-2.0
@REM

IF DEFINED SE050_ENV_IS_SETUP GOTO :ENV_DEFINED

    SET PATH=C:\MinGW\bin;%PATH%
    SET MCUX_DIR=NO_MCUXPRESSO
    @REM Put latest version at the end, oldest version at top.
    IF EXIST C:\nxp\MCUXpressoIDE_10.0.2_411 SET MCUX_DIR=C:\nxp\MCUXpressoIDE_10.0.2_411
    IF EXIST C:\nxp\MCUXpressoIDE_10.2.0_759 SET MCUX_DIR=C:\nxp\MCUXpressoIDE_10.2.0_759
    IF EXIST C:\nxp\MCUXpressoIDE_10.2.1_795  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_10.2.1_795
    IF EXIST C:\nxp\MCUXpressoIDE_10.3.0_2200  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_10.3.0_2200
    IF EXIST C:\nxp\MCUXpressoIDE_10.3.1_2233  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_10.3.1_2233
    IF EXIST C:\nxp\MCUXpressoIDE_11.0.0_2516  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_11.0.0_2516
    IF EXIST C:\nxp\MCUXpressoIDE_11.0.1_2563  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_11.0.1_2563
    IF EXIST C:\nxp\MCUXpressoIDE_11.1.0_3209  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_11.1.0_3209
    IF EXIST C:\nxp\MCUXpressoIDE_11.1.1_3241  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_11.1.1_3241
    IF EXIST C:\nxp\MCUXpressoIDE_11.2.0_4120  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_11.2.0_4120
    IF EXIST C:\nxp\MCUXpressoIDE_11.2.1_4149  SET MCUX_DIR=C:\nxp\MCUXpressoIDE_11.2.1_4149

    IF NOT "%MCUX_DIR%" EQU "NO_MCUXPRESSO" ECHO # INFO: Using MCUXPresso from '%MCUX_DIR%' for Kinetis/LPC/iMX.RT Controllers.
    IF "%MCUX_DIR%" EQU "NO_MCUXPRESSO" ECHO !! WARNING !! MCUXPresso was not found.
    IF "%MCUX_DIR%" EQU "NO_MCUXPRESSO" ECHO !! WARNING !! You may have to update %~0 to compile for Kinties/LPC/iMXRT controllers.

    @REM Default is visual studio 2015 if available.
    @REM For other visual studio release, we select latest version first
    @REM and then older versions.
    IF EXIST "%VS140COMNTOOLS%vsvars32.bat" SET MSVC_DIR="%VS140COMNTOOLS%"
    IF EXIST "%VS140COMNTOOLS%vsvars32.bat" CALL "%VS140COMNTOOLS%vsvars32.bat"
    IF EXIST "%VS140COMNTOOLS%vsvars32.bat" GOTO :MSVC_SET

    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat" SET MSVC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat" GOTO :MSVC_SET

    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" SET MSVC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" GOTO :MSVC_SET

    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" SET MSVC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" GOTO :MSVC_SET

    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat" SET MSVC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat" GOTO :MSVC_SET

    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" SET MSVC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" GOTO :MSVC_SET

    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat" SET MSVC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
    IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat" GOTO :MSVC_SET


    ECHO !! WARNING!!  Visual Studio not found/autodetected.
    ECHO !! WARNING!!  You may have to update %~0 to compile for Visual Studio.

:MSVC_SET

    IF DEFINED MSVC_DIR ECHO # INFO: Using MSVC from '%MSVC_DIR%'

    SET JAVA_HOME=JAVA_NOT_INSTALLED
    IF EXIST "C:\Program Files\Java\jdk1.8.0_191\bin\java"  SET JAVA_HOME=C:\Program Files\Java\jdk1.8.0_191\bin\java
    IF EXIST "C:\Program Files\Java\jdk1.8.0_201\bin\java"  SET JAVA_HOME=C:\Program Files\Java\jdk1.8.0_201\bin\java

    REM IF EXIST C:\opt\android-ndk-r18b-windows-x86_64\android-ndk-r18b SET ANDROID_NDK_ROOT=C:\opt\android-ndk-r18b-windows-x86_64\android-ndk-r18b

    SET PYTHON_DIR=NO_Python
    IF EXIST "%LOCALAPPDATA%\Programs\Python\Python38-32\python.exe" SET PYTHON_DIR=%LOCALAPPDATA%\Programs\Python\Python38-32
    IF EXIST "%LOCALAPPDATA%\Programs\Python\Python38-32\python.exe" GOTO :PYTHON_SET
    IF EXIST "%LOCALAPPDATA%\Programs\Python\Python37-32\python.exe" SET PYTHON_DIR=%LOCALAPPDATA%\Programs\Python\Python37-32
    IF EXIST "%LOCALAPPDATA%\Programs\Python\Python37-32\python.exe" GOTO :PYTHON_SET
    IF EXIST "C:\Python38-32\python.exe" SET PYTHON_DIR=C:\Python38-32
    IF EXIST "C:\Python38-32\python.exe" GOTO :PYTHON_SET
    IF EXIST "C:\Python37-32\python.exe" SET PYTHON_DIR=C:\Python37-32
    IF EXIST "C:\Python37-32\python.exe" GOTO :PYTHON_SET
    IF EXIST C:\Python37\python.exe SET PYTHON_DIR=C:\Python37
    IF EXIST C:\Python37\python.exe GOTO :PYTHON_SET

    ECHO "!WARNING! Python37 32bit not detected.  Python scripts and ssscli Tool may not work."
    ECHO "!WARNING! You may have to update %~0 to add extend detection of Python37 32bit."

:PYTHON_SET
    ECHO # INFO: Using Python from '%PYTHON_DIR%'

    SET CMAKE_DIR=NO_CMAKE
    IF EXIST "C:\Program Files\CMake\bin" SET CMAKE_DIR="C:\Program Files\CMake\bin"
    IF EXIST "C:\Program Files\CMake\bin" GOTO :CMAKE_SET
    IF EXIST C:\opt\cmake\bin SET CMAKE_DIR=C:\opt\cmake\bin
    IF EXIST C:\opt\cmake\bin GOTO :CMAKE_SET

    @REM Without CMake, only the MCUXPresso projects would work.
    ECHO "!WARNING! CMake not autodetected."
    ECHO "!WARNING! You may have to update %~0 to add extend detection of CMake."

:CMAKE_SET
    ECHO # INFO: Using CMake from '%CMAKE_DIR%'

    SET PATH=%MCUX_DIR%\ide\msys\bin;%PYTHON_DIR%;%PYTHON_DIR%\scripts;%CMAKE_DIR%;%MCUX_DIR%\ide\tools\bin;%PATH%;

    IF EXIST %MCUX_DIR%\ide\buildtools\bin SET PATH=%MCUX_DIR%\ide\buildtools\bin;%PATH%

    SET PATH=%~dp0..\ext\openssl\bin;%PATH%

    SET ARMGCC_DIR=%MCUX_DIR%\ide\tools
    SET MCUX_DIR=
    SET PYTHON_DIR=
    SET MAKE=make.exe -Rr
    SET SE050_ENV_IS_SETUP=1

GOTO :EOF

:ENV_DEFINED
     echo ENV Already setup.

