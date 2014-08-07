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

#include "rpi_shift.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <wiringPi.h>
#include <wiringShift.h>

typedef struct {
    uint8_t order;
    uint8_t data;
    uint8_t clock;
    uint8_t value;
} parameters_t;

static int npins;

static int parse_parameters(json_t *data, parameters_t *parameters)
{
    json_t *dataitem;
    if (data->type != cJSON_Object) {
        return -1;
    }
    
    dataitem = json->get_object_item(data, "order");
    if (dataitem == NULL || dataitem->type != cJSON_String) {
        return -1;
    }
    if (strcmp(dataitem->valuestring, "lsbfirst") == 0) {
        parameters->order = LSBFIRST;
    } else if (strcmp(dataitem->valuestring, "msbfirst") == 0) {
        parameters->order = MSBFIRST;
    } else {
        return -1;
    }
    
    dataitem = json->get_object_item(data, "data");
    if (dataitem == NULL || dataitem->type != cJSON_Number) {
        return -1;
    }
    if (dataitem->valueint < 0 || dataitem->valueint > npins) {
        return -1;
    }
    parameters->data = dataitem->valueint;
    
    dataitem = json->get_object_item(data, "clock");
    if (dataitem == NULL || dataitem->type != cJSON_Number) {
        return -1;
    }
    if (dataitem->valueint < 0 || dataitem->valueint > npins) {
        return -1;
    }
    parameters->clock = dataitem->valueint;
    
    dataitem = json->get_object_item(data, "value");
    if (dataitem == NULL) {
        return 1;
    }
    if (dataitem->type != cJSON_Number) {
        return -1;
    }
    if (dataitem->valueint < 0 || dataitem->valueint > 255) {
        return -1;
    }
    parameters->value = dataitem->valueint;

    return 0;
}

json_t * rpi_shift_get(duda_request_t *dr, int parameter)
{
    return json->create_string("Use POST method for shift/in or shift/out.");
}

json_t * rpi_shift_in_post(duda_request_t *dr, json_t *data, int parameter)
{
    parameters_t parameters;
    if (parse_parameters(data, &parameters) != 1) {
        return NULL;
    }

    return json->create_number((double)shiftIn(parameters.data, parameters.clock, parameters.order));
}

json_t *rpi_shift_out_post(duda_request_t *dr, json_t *data, int parameter)
{
    parameters_t parameters;
    if (parse_parameters(data, &parameters) != 0) {
        return NULL;
    }
    
    shiftOut(parameters.data, parameters.clock, parameters.order, parameters.value);
    return json->create_string("Successful!");
}

/* register and initialize module */
void rpi_shift_init(void)
{
    if (piBoardRev() == 1) {
        npins = 16;
    } else {
        npins = 20;
    }
    
    rpi_module_t *module = rpi_modules_module_init("shift", NULL, NULL);

    rpi_modules_value_init("in", rpi_shift_get, rpi_shift_in_post, &(module->values_head.values));
    rpi_modules_value_init("out", rpi_shift_get, rpi_shift_out_post, &(module->values_head.values));
}