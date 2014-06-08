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

int rpi_security_check_permission(const char *user, rpi_module_t * module);
char * rpi_security_get_user(duda_request_t *dr);

void rpi_security_init(void);

#endif