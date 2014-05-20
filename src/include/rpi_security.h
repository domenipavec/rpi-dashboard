/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_SECURITY_H
#define RPI_SECURITY_H

#include "rpi_security.h"
#include "rpi_modules.h"

#define SHA1_DIGEST_LEN 20

/* 
 * a list of users
 */
struct user {
    char user[128];
    char passwd_raw[256];
    unsigned char *passwd_decoded;

    struct mk_list _head;
};

int rpi_security_check_permission(duda_request_t *dr, rpi_module_t * module);

void rpi_security_init(void);

#endif