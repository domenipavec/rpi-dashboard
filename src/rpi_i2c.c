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
#include <wiringPiI2C.h>

#include <assert.h>

static int i2cfd[NI2C+1];

static int get_fd(duda_request_t *dr)
{
    int devid;
    char *devid_str;
    devid_str = qs->get(dr, "device");
    if (devid_str == NULL) {
        return -1;
    }
    
    // convert hex or decimal
    if (strncmp(devid_str, "0x", 2) == 0) {
        devid = strtol(devid_str + 2, NULL, 16);
    } else {
        devid = atoi(devid_str);
    }
    
    // check range
    if (devid < 3 || devid > NI2C) {
        return -1;
    }
    
    // init if not yet
    if (i2cfd[devid] == -1) {
        i2cfd[devid] = wiringPiI2CSetup(devid);
    }

    return i2cfd[devid];
}

json_t *rpi_i2c_byte_get(duda_request_t *dr, int parameter)
{
    int fd = get_fd(dr);
    if (fd == -1) {
        return json->create_null();
    }
    
    return json->create_number((double)wiringPiI2CRead(fd));
}

int rpi_i2c_byte_post(duda_request_t *dr, json_t *data, int parameter)
{
    int fd = get_fd(dr);
    if (fd == -1) {
        return -1;
    }
    if (data->type != cJSON_Number) {
        return -1;
    }
    int value = data->valueint;
    if (value < 0 || value > 255) {
        return -1;
    }
    
    if (wiringPiI2CWrite(fd, value) != 0) {
        return -1;
    }
    
    return 0;
}

json_t *rpi_i2c_register8_n_get(duda_request_t *dr, int parameter)
{
    int fd = get_fd(dr);
    if (fd == -1) {
        return json->create_null();
    }
    
    return json->create_number((double)wiringPiI2CReadReg8(fd, parameter));
}

int rpi_i2c_register8_n_post(duda_request_t *dr, json_t *data, int parameter)
{
    int fd = get_fd(dr);
    if (fd == -1) {
        return -1;
    }
    if (data->type != cJSON_Number) {
        return -1;
    }
    int value = data->valueint;
    if (value < 0 || value > 255) {
        return -1;
    }
    
    if (wiringPiI2CWriteReg8(fd, parameter, value) != 0) {
        return -1;
    }
    
    return 0;
}

json_t *rpi_i2c_register16_n_get(duda_request_t *dr, int parameter)
{
    int fd = get_fd(dr);
    if (fd == -1) {
        return json->create_null();
    }
    
    return json->create_number((double)wiringPiI2CReadReg16(fd, parameter));
}

int rpi_i2c_register16_n_post(duda_request_t *dr, json_t *data, int parameter)
{
    int fd = get_fd(dr);
    if (fd == -1) {
        return -1;
    }
    if (data->type != cJSON_Number) {
        return -1;
    }
    int value = data->valueint;
    if (value < 0 || value > 65535) {
        return -1;
    }
    
    if (wiringPiI2CWriteReg16(fd, parameter, value) != 0) {
        return -1;
    }
    
    return 0;
}

int rpi_i2c_register8_post(duda_request_t *dr, json_t *data, int parameter)
{
    json_t *child;
    int ret = 0;
    for (child = data->child; child; child = child->next) {
        parameter = atoi(child->string);
        if (parameter >= 0 && parameter <= 255) {
            if (rpi_i2c_register8_n_post(dr, child, parameter) != 0) {
                ret = -1;
            }
        } else {
            ret = -1;
        }
    }
    return ret;
}

int rpi_i2c_register16_post(duda_request_t *dr, json_t *data, int parameter)
{
    json_t *child;
    int ret = 0;
    for (child = data->child; child; child = child->next) {
        parameter = atoi(child->string);
        if (parameter >= 0 && parameter <= 255) {
            if (rpi_i2c_register16_n_post(dr, child, parameter) != 0) {
                ret = -1;
            }
        } else {
            ret = -1;
        }
    }
    return ret;
}

/* register and initialize module */
void rpi_i2c_init(void)
{
    int i;

    for (i = 0; i <= NI2C; i++) {
        i2cfd[i] = -1;
    }
    
    rpi_module_t *module = rpi_modules_module_init("i2c", NULL, NULL);

    rpi_modules_value_init("byte", rpi_i2c_byte_get, rpi_i2c_byte_post, &(module->values_head.values));
    rpi_module_value_t *r8branch = rpi_modules_branch_init("register8", rpi_i2c_register8_post, &(module->values_head.values));
    rpi_module_value_t *r16branch = rpi_modules_branch_init("register16", rpi_i2c_register16_post, &(module->values_head.values));

    rpi_modules_value_init("%d0:255", rpi_i2c_register8_n_get, rpi_i2c_register8_n_post, &(r8branch->values));
    rpi_modules_value_init("%d0:255", rpi_i2c_register16_n_get, rpi_i2c_register16_n_post, &(r16branch->values));
}