/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_modules.h"

static struct mk_list modules_list;

void rpi_module_list_init()
{
    mk_list_init(&modules_list);
}

void rpi_module_list_add(rpi_module_t * add)
{
    mk_list_add(&(add->_head), &modules_list);
}

rpi_module_t * rpi_module_list_find(mk_pointer find)
{
    struct mk_list * head;
    rpi_module_t * module;
    mk_list_foreach(head, &modules_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (find.len == module->name.len && 
            strncmp(find.data, module->name.data, find.len) == 0) {
            return module;
        }
    }
    return NULL;
}