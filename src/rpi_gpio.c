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

json_t * rpi_gpio(duda_request_t *dr, int parameter)
{
    return json->create_number((double)parameter);
}

/* register and initialize module */
void rpi_gpio_init(void)
{
    wiringPiSetup();
    
    rpi_module_t *module = rpi_modules_module_init("gpio", NULL);

    const char *valueHandle;
    if (piBoardRev() == 1) {
        valueHandle = "%d0:16";
    } else {
        valueHandle = "%d0:20";
    }
    rpi_modules_value_init(valueHandle, rpi_gpio, &(module->values_head.values));
}