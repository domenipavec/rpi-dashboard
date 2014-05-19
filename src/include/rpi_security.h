/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_SECURITY_H
#define RPI_SECURITY_H

#include "rpi_security.h"
#include "rpi_modules.h"

int rpi_validate_user(duda_request_t *dr, rpi_module_t * module);

void rpi_send_auth_request(duda_request_t *dr);

#endif