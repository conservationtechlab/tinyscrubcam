/******************************************************************************
 * Copyright 2017 Google
 * Copyright 2019 NXP
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

// [START iot_mqtt_include]
#define _XOPEN_SOURCE 500
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "MQTTClient.h"
#include "jwt.h"
#include "openssl/conf.h"
#include "openssl/ec.h"
#include "openssl/evp.h"
#include "jsmn.h"

#include "mqtt_publish.h"

#define TRACE 1 /* Set to 1 to enable tracing */

void Usage()
{
    printf("watson_imx_linux \\\n");
    printf("\t--payload \"Message to publish\"\\\n");
    printf("\t--deviceid <your device id>\\\n");
    printf("\t--keypath <e.g. ./ec_private.pem>\\\n");
    printf("\t--rootpath root CA file of Watson in PEM format\\\n");
    printf("\t--devcert device certificate in PEM format\\\n");
    printf("\t--username Watson IoT user name\\\n");
    printf("\t--topic Watson IoT Publish topic\\\n");
    printf("\t--org Watson IoT org name\\\n");
    printf("\t--type Device type\\\n");
}

void Usage_json()
{
    printf("./watson_imx_linux ");
    printf("--json <your json file having all requiered parameters>\\\n");
    printf("Sample json as below (one can use same file with updated values)\n");
    printf("\t{\n");
    printf("\t\"hostname\":\"uc3st5.messaging.internetofthings.ibmcloud.com\",\n");
    printf("\t\"keypath\" : \"PATH_TO_KEYREF.PEM\"\n");
    printf("\t\"devcert\" : \"PATH_TO_DEVICE_CERTIFICATE.PEM\", \n");
    printf("\t\"topic\" : \"iot-2/evt/status/fmt/string\", \n");
    printf("\t\"payload\" : \"PAYLOAD TO SEND\", \n");
    printf("\t}\n\n");

}

/**
 * Helper to parse arguments passed to app. Returns false if there are missing
 * or invalid arguments; otherwise, returns true indicating the caller should
 * free the calculated client ID placed on the opts structure.
 *
 * TODO: (class) Consider getopt
 */
// [START iot_mqtt_opts]
bool get_json_tagValue(char *js, const char * key, char * value)
{
    jsmn_parser p;
    jsmntok_t tokens[50]; /* We expect no more than 50 JSON tokens */
    jsmn_init(&p);
    int count = jsmn_parse(&p, js, strlen(js), tokens, 50);
    for (int i = 1; i < count; i += 2)
    {
        jsmntok_t *t = &tokens[i];
        char *tag = js + t->start;
        if (!memcmp(tag, key, t->end - t->start))
        {
            t = &tokens[i + 1];
            memcpy(value, js + t->start, t->end - t->start);
            value[t->end - t->start] = '\0';
            return true;
        }
    }
    printf("Tag %s not found\n", key);
    return false;
}
char hostname[256];
char key[256];
char cert[256];
char payload[256];
char topic[256];

bool get_connection_params_file(char * filename)
{
    FILE *fk = fopen(filename, "rb");
    if (fk == NULL)
    {
        printf("Can not open the file [%s]\n", filename);
        return false;
    }
    fseek(fk, 0, SEEK_END);
    long json_file_len = ftell(fk);
    fseek(fk, 0, SEEK_SET);
    char *json_file = malloc(json_file_len +1);
    fread(json_file, 1, json_file_len, fk);
    fclose(fk);
    json_file[json_file_len] = 0;

    printf("json file contents %s\n", json_file);

    bool calcurl = true;

    if (!get_json_tagValue(json_file, "hostname", hostname) ||
        !get_json_tagValue(json_file, "keypath", key) ||
        !get_json_tagValue(json_file, "devcert", cert) ||
        !get_json_tagValue(json_file, "topic", topic) ||
        !get_json_tagValue(json_file, "payload", payload))
    {
        printf("Json file missing above parameters! See usage below for all required parameters\n");
        Usage_json();
        free(json_file);
        return false;
    }

    free(json_file);

    opts.payload = payload;
    opts.keypath = key;
    opts.devcert = cert;
    strcpy((char *restrict) & opts.clientid, "");
    strcpy((char *restrict) & opts.topic, topic);


    if (calcurl) {
        size_t n = snprintf(opts.address,
            sizeof(opts.address),
            "ssl://%s:8883",
            hostname);
        if (n > sizeof(opts.address)) {
            printf("Error, buffer for storing URL was too small.\n");
            return false;
        }
    }
    return true;
}


bool GetOpts(int argc, char **argv)
{
    int pos = 1;
    if (argc < 2) {
        return false;
    }

    if (strcmp(argv[pos], "--json") == 0) {
        if (++pos < argc) {
            char * filename = argv[pos];
            printf("Reading parameters from json file %s\n", filename);
            if (get_connection_params_file(filename))
            {
                return true;
            }
        }
        exit(EXIT_FAILURE);
    }

    char *org = NULL;
    char *type = NULL;
    char *deviceid = NULL;
    bool calcurl = true;
    bool calcclientid = true;
    bool hasPayload = false;

    if (argc < 2) {
        return false;
    }

    while (pos < argc) {
        if (strcmp(argv[pos], "--payload") == 0) {
            if (++pos < argc) {
                opts.payload = argv[pos];
                hasPayload = true;
            }
            else {
                return false;
            }
        }
        if (strcmp(argv[pos], "--deviceid") == 0) {
            if (++pos < argc) {
                deviceid = argv[pos];
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--org") == 0) {
            if (++pos < argc) {
                org = argv[pos];
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--type") == 0) {
            if (++pos < argc) {
                type = argv[pos];
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--keypath") == 0) {
            if (++pos < argc) {
                opts.keypath = argv[pos];
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--devcert") == 0) {
            if (++pos < argc) {
                opts.devcert = argv[pos];
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--rootpath") == 0) {
            if (++pos < argc) {
                opts.rootpath = argv[pos];
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--topic") == 0) {
            if (++pos < argc) {
                strcpy((char *restrict) & opts.topic, argv[pos]);
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--address") == 0) {
            if (++pos < argc) {
                strcpy((char *restrict) & opts.address, argv[pos]);
                calcurl = false;
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--clientid") == 0) {
            if (++pos < argc) {
                strcpy((char *restrict) & opts.clientid, argv[pos]);
                calcclientid = false;
            }
            else {
                return false;
            }
        }
        else if (strcmp(argv[pos], "--username") == 0) {
            if (++pos < argc) {
                strcpy((char *restrict) & opts.username, argv[pos]);
            }
            else
                return false;
        }
        pos++;
    }

    if (!hasPayload) {
        printf("Payload not passed\n");
        return false;
    }
    if (calcurl) {
        size_t n = snprintf(opts.address,
            sizeof(opts.address),
            "ssl://%s.messaging.internetofthings.ibmcloud.com:8883",
            org);
        if (n > sizeof(opts.address)) {
            printf("Error, buffer for storing URL was too small.\n");
            return false;
        }
    }

    if (calcclientid) {
        size_t n = snprintf(opts.clientid,
            sizeof(opts.clientid),
            "d:%s:%s:%s",
            org,
            type,
            deviceid);
        if (n > sizeof(opts.clientid)) {
            printf("Error, buffer for storing client ID was too small.\n");
            return false;
        }
    }

    return true;
}
// [END iot_mqtt_opts]

/**
 * Connects MQTT client and transmits payload.
 */
// [START iot_mqtt_run]
int main(int argc, char *argv[])
{
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    OPENSSL_config(NULL);
#else
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, NULL);
#endif
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_digests();
    OpenSSL_add_all_ciphers();
    int rc = -1;

    if (GetOpts(argc, argv)) {
        printf("address: %s\n", opts.address);
        printf("devCert: %s\n", opts.devcert);
        rc = Publish(opts.payload, strlen(opts.payload));
        printf("Publish done with error code : %d\n", rc);
    }
    else {
        Usage();
    }

    EVP_cleanup();
}
// [END iot_mqtt_run]
