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

#include "rpi_general.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <wiringPi.h>

json_t *rpi_general_uptime_get(duda_request_t *dr, int parameter)
{
    FILE *f;
    int ret;
    double uptime;
    
    f = fopen("/proc/uptime", "r");
    if (f == NULL) {
        return json->create_number(0.0);
    }

    ret = fscanf(f, " %lf", &uptime);
    if (ret != 1) {
        return json->create_number(0.0);
    }
    
    fclose(f);
    
    return json->create_number(uptime);
}

json_t *rpi_general_board_rev_get(duda_request_t *dr, int parameter)
{
    return json->create_number((double)piBoardRev());
}

/* register and initialize module */
void rpi_general_init(void)
{
    rpi_module_t *module = rpi_modules_module_init("general", NULL, NULL);
    
    rpi_modules_value_init("uptime", rpi_general_uptime_get, NULL, &(module->values_head.values));
    rpi_modules_value_init("board-rev", rpi_general_board_rev_get, NULL, &(module->values_head.values));
}