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

extern struct mk_list rpi_module_list;

/* find module by name */
rpi_module_t * rpi_module_list_find(mk_pointer find);

/* read module config from file */
void rpi_module_list_read_conf();

#endif