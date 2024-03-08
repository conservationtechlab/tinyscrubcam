CC=gcc # gcc || clang
CCOPTIONS=-std=c11 -Wall -Wextra -pedantic -Wstrict-overflow -fno-strict-aliasing -g

JSMN_DIR := ../../../ext/amazon-freertos/libraries/3rdparty/jsmn

SSLPATH=-L/openssl
INCLUDEDIR=-I../common/ -I$(JSMN_DIR)

all: watson_imx_linux.c
	$(CC) $(CCOPTIONS) $(SSLPATH) watson_imx_linux.c ../common/mqtt_publish.c $(JSMN_DIR)/jsmn.c $(INCLUDEDIR) $(BUILDARGS)  -ljwt -lcrypto -lssl -lpaho-mqtt3cs -o watson_imx_linux
