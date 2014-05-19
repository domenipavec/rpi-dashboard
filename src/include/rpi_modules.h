/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_MODULES_H
#define RPI_MODULES_H

#include "webservice.h"

#define RPI_ALLOW_LIST      0
#define RPI_ALLOW_GUEST     1
#define RPI_ALLOW_ALLUSERS  2

typedef struct rpi_module {
    mk_pointer name;

    struct mk_list _head;
} rpi_module_t;

/* find module by name */
rpi_module_t * rpi_module_list_find(mk_pointer find);

/* init list of modules */
void rpi_module_list_init();

/* add module to list */
void rpi_module_list_add(rpi_module_t * add);

#endif