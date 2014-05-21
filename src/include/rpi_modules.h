/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_MODULES_H
#define RPI_MODULES_H

#include "webservice.h"

#define RPI_ALLOW_LIST      0
#define RPI_ALLOW_GUESTS    1
#define RPI_ALLOW_ALLUSERS  2

typedef struct rpi_module {
    const char *name;

    /* access permissions */
    int allow_flag;
    struct mk_list *allowed_users;

    struct mk_list _head;
} rpi_module_t;

/* find module by name */
rpi_module_t * rpi_modules_find(mk_pointer find);

/* parse allow flag from string */
int rpi_modules_parse_allow_flag(char *str);

/* initialize modules */
void rpi_modules_init(void);

#endif