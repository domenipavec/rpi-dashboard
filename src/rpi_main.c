/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */


#include "webservice.h"
#include "packages/base64/base64.h"
#include "packages/sha1/sha1.h"

#include "rpi_modules.h"
#include "rpi_security.h"
#include "rpi_config.h"

DUDA_REGISTER("Duda Raspberry Pi interface", "Raspberry Pi interface");

/* handle all requests to REST api */
void rpi_global_callback(duda_request_t *dr)
{
    rpi_module_t *module;
    int ret;
    
    module = rpi_modules_find(dr->interface);
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

    response->printf(dr, "Module exists!");

    response->end(dr, NULL);
}

/* initialize webservice */
int duda_main()
{
    duda_load_package(base64, "base64");
    duda_load_package(sha1, "sha1");

    rpi_config_init();
    rpi_modules_init();
    rpi_security_init();

    map->static_add("/test", "rpi_global_callback");

    return 0;
}