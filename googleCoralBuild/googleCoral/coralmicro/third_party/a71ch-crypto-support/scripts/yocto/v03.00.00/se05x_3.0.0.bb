DESCRIPTION = "NXP Plug and Trust Middleware"
LICENSE = "CLOSED & MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# ATTENTION: 
# - The NXP-PLUG-TRUST-MW variable defined below
# - The verbatim instantiation of the package name in the function pkg_postinst_ontarget_${PN}
#   defined at the end of this file
# must match the root name (i.e. excluding the .zip extension) of the downloaded Plug&Trust zip file
#
# Please replace the value 'se05x_mw_v03.00.03' - used twice in this script - by the
# exact root name of the package if required.
#
# [Update value]
# **************
NXP-PLUG-TRUST-MW = "se05x_mw_v03.00.03"
#

inherit cmake dos2unix
DEPENDS = "openssl python python3"
RDEPENDS_${PN} += "bash"

RDEPENDS_${PN} += "python3-misc"

# Allow symlink .so in non-dev package
INSANE_SKIP_${PN} += " dev-so"

# FILES_${PN} = "/usr/local/lib/*.so /usr/local/bin/* /home/root/${NXP-PLUG-TRUST-MW}"

# FILES_${PN}-dev = "/usr/local/lib/libpaho-*.so /usr/local/lib/libpaho-*.so.* \
#     /usr/local/include/* /usr/local/share/* /usr/local/lib/cmake/*"

FILES_${PN} = "/usr/local/lib/*.so /usr/local/bin/* /home/root/${NXP-PLUG-TRUST-MW} \
    /home/root/install \
    /usr/local/lib/libpaho-*.so /usr/local/lib/libpaho-*.so.* \
    /etc/ld.so.conf.d/se05x.conf /usr/bin/ssscli"


FILES_${PN}-dev = " \
    /usr/local/include/* /usr/local/share/* /usr/local/lib/cmake/*"

FILES_${PN}-staticdev = "/usr/local/lib/*.a*"

S = "${WORKDIR}/simw-top"
B = "${WORKDIR}/build"


SRC_URI = "file://${NXP-PLUG-TRUST-MW}.zip \
           "

do_configure() {
    cd ${B}

    cmake -S ../simw-top \
      -DWithSharedLIB=ON -DCMAKE_BUILD_TYPE=Debug -DHost=iMXLinux -DHostCrypto=OPENSSL \
      -DSMCOM=T1oI2C -DSE05X_Auth=None -DIOT=None -DApplet=SE05X_C -DSE05X_Ver=03_XX  \
      -DOPENSSL_INSTALL_PREFIX=${WORKDIR}/recipe-sysroot/usr/ \
      -DPAHO_BUILD_DEB_PACKAGE=OFF -DPAHO_BUILD_DOCUMENTATION=OFF -DPAHO_BUILD_SAMPLES=OFF \
      -DPAHO_BUILD_SHARED=ON -DPAHO_BUILD_STATIC=OFF -DPAHO_ENABLE_CPACK=ON \
      -DPAHO_ENABLE_TESTING=OFF \
      -DPAHO_WITH_SSL=ON \
      -DOPENSSL_ROOT_DIR=${WORKDIR}/recipe-sysroot/usr/
}

do_compile() {
    oe_runmake
}

do_install_append() {
    # Install Unzipped Plug&Trust MW package in /home/root
    # Resulting unzipped Package deviates from the one published on www.nxp.com in the following respect:
    # - Source code patches (if any) have been applied (refer to se05x bitbake recipe)
    # - ext/mcu-sdk directory removed
    # - sss/plugin/openssl/bin/libsss_engine.so removed
    # - tools/libsssapisw.so removed
    echo ${S}
    echo ${D}
    install -d ${S} ${D}/home/root/${NXP-PLUG-TRUST-MW}
    cp -r --no-preserve=ownership ${S} ${D}/home/root/${NXP-PLUG-TRUST-MW}
    rm -rf ${D}/home/root/${NXP-PLUG-TRUST-MW}/simw-top/binaries/ex
    rm -rf ${D}/home/root/${NXP-PLUG-TRUST-MW}/simw-top/ext/mcu-sdk
    rm -f ${D}/home/root/${NXP-PLUG-TRUST-MW}/simw-top/sss/plugin/openssl/bin/libsss_engine.so
    rm -f ${D}/home/root/${NXP-PLUG-TRUST-MW}/simw-top/tools/libsssapisw.so
    find ${D}/home/root/${NXP-PLUG-TRUST-MW}/simw-top -name "*.sh" -exec chmod 755 {} \;

    mkdir -p ${D}/home/root/install
    cp --no-preserve=ownership ${D}/home/root/${NXP-PLUG-TRUST-MW}/simw-top/scripts/imx/se05x_mw_install.sh ${D}/home/root/install
    echo "Use se05x_mw_install.sh to install an update of the package" > ${D}/home/root/install/readme.txt

    mkdir -p ${D}/etc/ld.so.conf.d
    echo /usr/local/lib > ${D}/etc/ld.so.conf.d/se05x.conf
}


# Package directory hardcoded: [Update value]
# *******************************************
pkg_postinst_ontarget_${PN} () {
    ldconfig /usr/local/lib
    cd /home/root/se05x_mw_v03.00.03/simw-top/pycli/src
    python3 setup.py develop
}
