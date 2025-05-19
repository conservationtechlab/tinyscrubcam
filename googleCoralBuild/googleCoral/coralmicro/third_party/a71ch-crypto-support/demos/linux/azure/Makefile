CC=gcc # gcc || clang
CCOPTIONS=-std=c11 -Wall -Wextra -pedantic -Wstrict-overflow -fno-strict-aliasing -g -MMD -MP -lpthread

JSMN_DIR := ../../../ext/amazon-freertos/libraries/3rdparty/jsmn

SSLPATH=-L/openssl
INCLUDEDIR=-I../common/ -I$(JSMN_DIR) -I/usr/local_azure/include/

all: azure_imx_register azure_imx_connect

azure_imx_register: azure_imx_register.c Makefile
	$(CC) $(CCOPTIONS) $(SSLPATH) azure_imx_register.c mqtt_azure.c $(JSMN_DIR)/jsmn.c $(INCLUDEDIR) $(BUILDARGS)  -ljwt -lcrypto -lssl -lpaho-mqtt3cs -o azure_imx_register

azure_imx_connect: azure_imx_connect.c Makefile
	$(CC) $(CCOPTIONS) $(SSLPATH) azure_imx_connect.c mqtt_azure.c $(JSMN_DIR)/jsmn.c $(INCLUDEDIR) $(BUILDARGS)  -ljwt -lcrypto -lssl -lpaho-mqtt3cs -o azure_imx_connect

clean:
	-rm azure_imx_connect azure_imx_register *.d

.PHONY: all
