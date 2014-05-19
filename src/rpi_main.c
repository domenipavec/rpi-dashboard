/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */


#include "webservice.h"

DUDA_REGISTER("Duda Raspberry Pi interface", "Raspberry Pi interface");

/* handle all requests to REST api */
void rpi_global_callback(duda_request_t *dr)
{
    response->http_status(dr, 200);

    response->printf(dr, "It works!");

    response->end(dr, NULL);
}

/* initialize webservice */
int duda_main()
{
    map->static_add("", "rpi_global_callback");
    
    return 0;
}