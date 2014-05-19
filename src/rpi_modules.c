/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_modules.h"

struct mk_list rpi_module_list;

rpi_module_t * rpi_module_list_find(mk_pointer find)
{
    struct mk_list * head;
    rpi_module_t * module;
    mk_list_foreach(head, &rpi_module_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (find.len == module->name.len && 
            strncmp(find.data, module->name.data, find.len) == 0) {
            return module;
        }
    }
    return NULL;
}