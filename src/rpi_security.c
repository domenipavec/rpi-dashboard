/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_security.h"

#include <time.h>

int rpi_validate_user(duda_request_t *dr, rpi_module_t * module)
{
    if (module->allow_flag == RPI_ALLOW_GUEST) {
        return 0;
    }
    
    /* login on second attempt for testing */
    if (session->get(dr, "user") != NULL) {
        return 0;
    } else {
        session->create(dr, "user", "john", time(NULL) + 3600);
        return -1;
    }
}

void rpi_send_auth_request(duda_request_t *dr)
{
    response->http_status(dr, 403);

    response->printf(dr, "Forbidden!");

    response->end(dr, NULL);
}