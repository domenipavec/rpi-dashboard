/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */


#include "webservice.h"
#include "packages/base64/base64.h"
#include "packages/sha1/sha1.h"

#include "rpi_modules.h"
#include "rpi_security.h"
#include "rpi_config.h"

DUDA_REGISTER("Duda Raspberry Pi interface", "Raspberry Pi interface");

/* takes list of rpi_module_value_t and construct json object of all subvalues */
static json_t * construct_full_json(struct mk_list *values)
{
    json_t *object;
    struct mk_list *entry;
    rpi_module_value_t *value;
    
    object = json->create_object();
    
    mk_list_foreach(entry, values) {
        value = mk_list_entry(entry, rpi_module_value_t, _head);
        if (value->get_value == NULL) {
            json->add_to_object(object, value->name, construct_full_json(&(value->values)));
        } else {
            json->add_to_object(object, value->name, value->get_value());
        }
    }
    
    return object;
}

/* handle all requests to REST api */
void rpi_global_callback(duda_request_t *dr)
{
    rpi_module_t *module;
    int ret;
    char *json_text
    
    module = rpi_modules_find(dr->method);
    if (module == NULL) {
        response->http_status(dr, 404);
        response->printf(dr, "Module does not exist!");
        response->end(dr, NULL);
        return;
    }

    ret = rpi_security_check_permission(dr, module);
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

    response->http_status(dr, 200);

    /* start testing */
    response->printf(dr, "Module exists!\n\nRequest path: %s", dr->method.data + dr->method.len);
    
    json_text = json->print_gc(dr, construct_full_json(&(module->values)));
    
    /* end testing */
    
    response->printf(dr, json_text);

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

    map->static_add("/api/", "rpi_global_callback");

    return 0;
}