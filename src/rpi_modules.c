/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_modules.h"
#include "rpi_config.h"

static struct mk_list rpi_module_list;

rpi_module_t * rpi_modules_find(mk_pointer find)
{
    struct mk_list * head;
    rpi_module_t * module;
    
    mk_list_foreach(head, &rpi_module_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (strlen(module->name) == find.len) {
            if (memcmp(find.data, module->name, find.len) == 0) {
                return module;
            }
        }
    }
    
    return NULL;
}

int rpi_modules_parse_allow_flag(char *str)
{
    if (strcmp(str, "guests") == 0) {
        return RPI_ALLOW_GUESTS;
    }
    if (strcmp(str, "list") == 0) {
        return RPI_ALLOW_LIST;
    }
    return RPI_ALLOW_ALLUSERS;
}

static void rpi_modules_read_conf()
{
    struct mk_list *entry;
    struct mk_list *entry_next;
    rpi_module_t *module;
    struct duda_config *dconf;
    struct duda_config_section *dconf_sect;
    void *dconf_value;

    dconf = fconf->read_conf("modules.conf");
    if (dconf == NULL) {
        return;
    }

    mk_list_foreach_safe(entry, entry_next, &rpi_module_list) {
        module = mk_list_entry(entry, rpi_module_t, _head);
        
        dconf_sect = fconf->section_get(dconf, module->name);
        if (dconf_sect == NULL) {
            continue;
        }

        /* delete disabled modules */
        if (fconf->section_key(dconf_sect, "Disabled", DUDA_CONFIG_BOOL) == (void *)MK_TRUE) {
            mk_list_del(entry);
            mem->free(module);
        }
        else {
            /* parse access settings */
            dconf_value = fconf->section_key(dconf_sect, "Access", DUDA_CONFIG_STR);
            if (dconf_value != NULL) {
                module->allow_flag = rpi_modules_parse_allow_flag((char *)dconf_value);
                mem->free(dconf_value);
            }

            dconf_value = fconf->section_key(dconf_sect, "AllowedUsers", DUDA_CONFIG_LIST);
            if (dconf_value != NULL) {
                module->allowed_users = (struct mk_list *)dconf_value;
            }
        }
    }
    
    fconf->free_conf(dconf);
}

void rpi_modules_init(void)
{
    mk_list_init(&rpi_module_list);

    /* init test module */
    rpi_module_t * test_module = (rpi_module_t *)mem->alloc(sizeof(rpi_module_t));
    test_module->name = "test";
    mk_list_add(&(test_module->_head), &rpi_module_list);

    /* set defaults from global config */
    struct mk_list *entry;
    rpi_module_t *module;
    mk_list_foreach(entry, &rpi_module_list) {
        module = mk_list_entry(entry, rpi_module_t, _head);
        module->allow_flag = rpi_config.default_allow_flag;
        module->allowed_users = rpi_config.default_allowed_users;
    }

    rpi_modules_read_conf();
}