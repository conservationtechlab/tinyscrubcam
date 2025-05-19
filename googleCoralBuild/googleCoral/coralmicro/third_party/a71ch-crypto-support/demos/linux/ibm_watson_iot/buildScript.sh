#!/bin/sh
#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

# Determine directory where script is stored
DEMO_DIR="$( cd "$(dirname "$0")" ; pwd -P )"
echo ${DEMO_DIR}
cd ${DEMO_DIR}

COMMON_DIR=${DEMO_DIR}/../common/
cd ${COMMON_DIR}
# building jansson
if [ ! -d jansson ]; then
	git clone https://github.com/akheron/jansson.git
	cd jansson
	autoreconf -i
	./configure
	make
	sudo make install
	cd ..
fi

sudo ldconfig /usr/local/lib

# building libjwt
if [ ! -d libjwt ]; then
	export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
	git clone https://github.com/benmcollins/libjwt.git
	cd libjwt
	autoreconf -i
	./configure
	make all
	sudo make install
	cd ..
fi


#building the app
sudo ldconfig /usr/local/lib
cd ${DEMO_DIR}
make







