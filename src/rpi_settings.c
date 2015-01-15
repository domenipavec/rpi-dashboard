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

#include "rpi_settings.h"

#include "rpi_modules.h"
#include "rpi_string.h"

#include "packages/json/json.h"

/* read json settings from file */
json_t * rpi_settings_get(duda_request_t *dr, int parameter)
{
    char * path = data->locate("settings.dat");
    
    char * data = (fconf->read_file)(path);
    if (data == NULL) {
        return json->create_null();
    }
    
    return json->parse(data);
}

/* store json settings to file */
json_t * rpi_settings_post(duda_request_t *dr, json_t *d, int parameter)
{
    char * path = data->locate("settings.dat");
    
    FILE * f = fopen(path, "w");
    if (f == NULL) {
        return json->create_string(rpi_string_concatN(3, "Could not open '", path, "'!"));
    }
    
    fputs(json->print_unformatted_gc(dr, d), f);
    
    fclose(f);

    return json->create_string("Successful!");
}

/* register and initialize module */
void rpi_settings_init(void)
{
    rpi_modules_module_init("settings", rpi_settings_get, rpi_settings_post);
}
