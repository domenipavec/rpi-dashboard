/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */


#include "webservice.h"

#include "rpi_modules.h"
#include "rpi_security.h"

DUDA_REGISTER("Duda Raspberry Pi interface", "Raspberry Pi interface");

/* handle all requests to REST api */
void rpi_global_callback(duda_request_t *dr)
{
    rpi_module_t *module = rpi_module_list_find(dr->interface);
    if (module == NULL) {
        response->http_status(dr, 404);
        response->printf(dr, "Module does not exist!");
        response->end(dr, NULL);
        return;
    }

    if (rpi_validate_user(dr, module) == -1) {
        rpi_send_auth_request(dr);
        return;
    }

    response->http_status(dr, 200);

    response->printf(dr, "Module exists!");

    response->end(dr, NULL);
}

/* initialize webservice */
int duda_main()
{
    map->static_add("", "rpi_global_callback");
    
    session->init("rpi");
    
    mk_list_init(&rpi_module_list);
    
    /* init test module */
    rpi_module_t * test_module = (rpi_module_t *)mem->alloc(sizeof(rpi_module_t));
    test_module->name = "test";
    mk_list_add(&(test_module->_head), &rpi_module_list);
    
    rpi_module_list_read_conf();
    
    return 0;
}