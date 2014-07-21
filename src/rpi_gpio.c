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

#include "rpi_gpio.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <wiringPi.h>

static gpio_pin_t pins[NPINS];
static const char *gpio_mode_str[] = {
    "undefined",
    "input",
    "output",
    "pwm",
    "tone"
};
static const char *gpio_pull_str[] = {
    "off",
    "down",
    "up"
};

static void init_pin(int i) {
    pins[i].mode = GPIO_UNDEFINED;
    pins[i].pull = PUD_OFF;
    pins[i].value = 0;
    pins[i].frequency = 1000;
    pins[i].range = 1024;
}

json_t * rpi_gpio(duda_request_t *dr, int parameter)
{
    json_t *object;

    object = json->create_object();
    json->add_to_object(object, "mode", json->create_string(gpio_mode_str[pins[parameter].mode]));
    json->add_to_object(object, "pull", json->create_string(gpio_pull_str[pins[parameter].pull]));
    if (pins[parameter].mode < GPIO_INPUT) {
        json->add_to_object(object, "value", json->create_number((double)digitalRead(parameter)));
    } else {
        json->add_to_object(object, "value", json->create_number((double)pins[parameter].value));
    }
    json->add_to_object(object, "frequency", json->create_number((double)pins[parameter].frequency));
    json->add_to_object(object, "range", json->create_number((double)pins[parameter].range));

    return object;
}

/* register and initialize module */
void rpi_gpio_init(void)
{
    int i;

    wiringPiSetup();

    for (i = 0; i < NPINS; i++) {
        init_pin(i);
    }
    
    rpi_module_t *module = rpi_modules_module_init("gpio", NULL);

    const char *valueHandle;
    if (piBoardRev() == 1) {
        valueHandle = "%d0:16";
    } else {
        valueHandle = "%d0:20";
    }
    rpi_modules_value_init(valueHandle, rpi_gpio, &(module->values_head.values));
}