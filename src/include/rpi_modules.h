/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_MODULES_H
#define RPI_MODULES_H

#include "webservice.h"
#include "packages/json/json.h"

#define RPI_ALLOW_LIST      0
#define RPI_ALLOW_GUESTS    1
#define RPI_ALLOW_ALLUSERS  2

/* function called to get value */
typedef json_t* (*rpi_module_get_value_t)(duda_request_t *);

/* values tree structure */
typedef struct {
    const char *name;
    
    /* leaf nodes have non NULL get_value */
    /* other nodes have a list of subnodes */
    rpi_module_get_value_t get_value;
    struct mk_list values;
    
    struct mk_list _head;
} rpi_module_value_t;

/* module list structure */
typedef struct rpi_module {
    /* access permissions */
    int allow_flag;
    struct mk_list *allowed_users;

    /* values tree */
    rpi_module_value_t values_head;
    
    struct mk_list _head;
} rpi_module_t;

/* find module by name */
rpi_module_t * rpi_modules_find(mk_pointer find);

/* takes path, and constructs corresponding json */
json_t * rpi_modules_json(duda_request_t *dr, 
                          rpi_module_value_t *value, 
                          char *path, 
                          json_t **to_delete);

/* parse allow flag from string */
int rpi_modules_parse_allow_flag(char *str);

/* initialize modules */
void rpi_modules_init(void);

/* initialize one module */
rpi_module_t * rpi_modules_module_init(const char *name, rpi_module_get_value_t gv);

/* initialize module value */
rpi_module_value_t * rpi_modules_value_init(const char *name, 
                                            rpi_module_get_value_t gv, 
                                            struct mk_list *parent);

/* initialize module branch */
rpi_module_value_t * rpi_modules_branch_init(const char *name,
                                             struct mk_list *parent);

#endif