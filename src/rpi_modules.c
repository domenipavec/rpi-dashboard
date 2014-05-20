/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_modules.h"

struct mk_list rpi_module_list;

rpi_module_t * rpi_module_list_find(mk_pointer find)
{
    struct mk_list * head;
    rpi_module_t * module;
    
    mk_list_foreach(head, &rpi_module_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (strncmp(find.data, module->name, find.len) == 0) {
            return module;
        }
    }
    
    return NULL;
}

void rpi_module_list_read_conf()
{
    struct mk_list *entry;
    struct mk_list *entry_next;
    rpi_module_t *module;
    struct duda_config *dconf;
    struct duda_config_section *dconf_sect;
    char *dconf_string;

    dconf = fconf->read_conf("modules.conf");
    if (dconf == NULL) {
        /* by defualt allow access from all users */
        mk_list_foreach(entry, &rpi_module_list) {
            module = mk_list_entry(entry, rpi_module_t, _head);
            module->allow_flag = RPI_ALLOW_ALLUSERS;
        }
        return;
    }

    mk_list_foreach_safe(entry, entry_next, &rpi_module_list) {
        module = mk_list_entry(entry, rpi_module_t, _head);
        
        dconf_sect = fconf->section_get(dconf, module->name);
        if (dconf_sect == NULL) {
            module->allow_flag = RPI_ALLOW_ALLUSERS;
            continue;
        }

        /* delete disabled modules */
        if (fconf->section_key(dconf_sect, "Disabled", DUDA_CONFIG_BOOL) == (void *)MK_TRUE) {
            mk_list_del(entry);
            mem->free(module);
        }
        else {
            /* parse access settings */
            dconf_string = (char *)fconf->section_key(dconf_sect, "Access", DUDA_CONFIG_STR);
            if (dconf_string == NULL) {
                module->allow_flag = RPI_ALLOW_ALLUSERS;
            }
            else if (strcmp(dconf_string, "guests") == 0) {
                module->allow_flag = RPI_ALLOW_GUESTS;
            }
            else if (strcmp(dconf_string, "list") == 0) {
                module->allow_flag = RPI_ALLOW_LIST;
                
                module->allowed_users = (struct mk_list *)fconf->section_key(dconf_sect, "AllowedUsers", DUDA_CONFIG_LIST);
                if (module->allowed_users == NULL) {
                    module->allowed_users = (struct mk_list *)mem->alloc(sizeof(struct mk_list));
                    mk_list_init(module->allowed_users);
                }
            }
            else {
                module->allow_flag = RPI_ALLOW_ALLUSERS;
            }
            
        }
        
        mem->free(dconf_sect);
    }
    
    fconf->free_conf(dconf);
}