/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Raspberry Pi Dasboard
 *  =====================
 *  Copyright 2014 Domen Ipavec <domen.ipavec@z-v.si>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "rpi_i2c.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <wiringPi.h>
#include <wiringSerial.h>

#include <assert.h>

#define SERIAL_DEVICE "/dev/ttyAMA0"

static int fd;
static int baud;

json_t * rpi_serial_baud_post(duda_request_t *dr, json_t *data, int parameter)
{
    if (data->type != cJSON_Number) {
        return NULL;
    }
    int value = data->valueint;
//     if (value < 0 || value > 255) {
//         return NULL;
//     }
    
    if (baud == value) {
        return json->create_string("Successful!");
    }
    
    if (fd != -1) {
        serialClose(fd);
    }
    
    fd = serialOpen(SERIAL_DEVICE, value);
    if (fd == -1) {
        return json->create_string("Could not open serial device.");
    }
    
    baud = value;
    
    return json->create_string("Successful!");
}

json_t * rpi_serial_baud_get(duda_request_t *dr, int parameter)
{
    return json->create_number((double)baud);
}

json_t * rpi_serial_port_post(duda_request_t *dr, json_t *data, int parameter)
{
    if (fd == -1) {
        fd = serialOpen(SERIAL_DEVICE, baud);
    }
    if (fd == -1) {
        return json->create_string("Could not open serial device.");
    }
    
    if (data->type != cJSON_String) {
        return NULL;
    }
    
    serialPuts(fd, data->valuestring);
    
    return json->create_string("Successful!");
}

json_t * rpi_serial_port_get(duda_request_t *dr, int parameter)
{
    int len, i;
    char *buffer;
    
    if (fd == -1) {
        fd = serialOpen(SERIAL_DEVICE, baud);
    }
    if (fd == -1) {
        return json->create_string("");
    }
    
    len = serialDataAvail(fd);
    if (len < 1) {
        return json->create_string("");
    }

    buffer = mem->alloc(len+1);
    gc->add(dr, buffer);
    buffer[len] = '\0';
    
    for (i = 0; i < len; i++) {
        buffer[i] = (char)serialGetchar(fd);
    }
    
    return json->create_string(buffer);
}

/* register and initialize module */
void rpi_serial_init(void)
{
    fd = -1;
    baud = 9600;

    rpi_module_t *module = rpi_modules_module_init("serial", NULL, NULL);

    rpi_modules_value_init("baud", rpi_serial_baud_get, rpi_serial_baud_post, &(module->values_head.values));
    rpi_modules_value_init("port", rpi_serial_port_get, rpi_serial_port_post, &(module->values_head.values));
}