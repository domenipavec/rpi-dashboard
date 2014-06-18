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

#include "webservice.h"
#include "packages/base64/base64.h"
#include "packages/sha1/sha1.h"

#include "rpi_modules.h"
#include "rpi_security.h"
#include "rpi_config.h"
#include "rpi_logger.h"

DUDA_REGISTER("Duda Raspberry Pi interface", "Raspberry Pi interface");

/* handle all requests to REST api */
void rpi_global_callback(duda_request_t *dr)
{
    rpi_module_t *module;
    int ret;
    json_t *json_object;
    json_t *json_delete;
    char *json_text;
    char *user;
    
    user = rpi_security_get_user(dr);
    
    if (*(dr->interface.data + dr->interface.len + 1) == ' ') {
        response->http_status(dr, 200);
        response->http_content_type(dr, "json");
        
        json_object = rpi_modules_user_list(user);
        json_text = json->print_gc(dr, json_object);
        response->printf(dr, json_text);
        json->delete(json_object);
        
        response->end(dr, NULL);
    } else {
        module = rpi_modules_find(dr->method);
        if (module == NULL) {
            response->http_status(dr, 404);
            response->printf(dr, "Module does not exist!");
            response->end(dr, NULL);
            return;
        }
    }

    ret = rpi_security_check_permission(user, module);
    if (ret == -2) {
        response->http_status(dr, 403);
        response->printf(dr, "You do not have permission to access this module!");
        response->end(dr, NULL);
        return;
    }
    else if (ret == -1) {
        response->http_status(dr, 401);
        response->http_header(dr, "WWW-Authenticate: Basic realm=\"Duda Raspberry Pi interface\"");
        response->printf(dr, "You have to be logged in to access this module!");
        response->end(dr, NULL);
        return;
    }

    json_object = rpi_modules_json(dr, &(module->values_head), dr->method.data + dr->method.len, &json_delete);
    if (json_object == NULL) {
        response->http_status(dr, 404);
        response->printf(dr, "Module does not contain this value!");
        response->end(dr, NULL);
        return;
    }

    response->http_status(dr, 200);
    response->http_content_type(dr, "json");
   
    json_text = json->print_gc(dr, json_object);
    response->printf(dr, json_text);
    
    json->delete(json_delete);

    response->end(dr, NULL);
}

/* initialize webservice */
int duda_main()
{
    duda_load_package(base64, "base64");
    duda_load_package(sha1, "sha1");
    duda_load_package(json, "json");

    rpi_config_init();
    rpi_modules_init();
    rpi_security_init();
    rpi_logger_init();

    map->static_add("/api/", "rpi_global_callback");

    return 0;
}