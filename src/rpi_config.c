/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_config.h"
#include "rpi_modules.h"

#include "webservice.h"

rpi_config_t rpi_config;

static void load_default_values(void)
{
    rpi_config.session_expires = 0;
    rpi_config.default_allow_flag = RPI_ALLOW_ALLUSERS;
    rpi_config.default_allowed_users = (struct mk_list *)mem->alloc(sizeof(struct mk_list));
    mk_list_init(rpi_config.default_allowed_users);
}

void rpi_config_init(void)
{
    struct duda_config *dconf;
    struct duda_config_section *dconf_sect;
    void *dconf_value;
    
    load_default_values();
    
    dconf = fconf->read_conf("rpi.conf");
    if (dconf == NULL) {
        return;
    }
    
    dconf_sect = fconf->section_get(dconf, "GLOBAL");
    if (dconf_sect == NULL) {
        return;
    }
    
    dconf_value = fconf->section_key(dconf_sect, "SessionExpires", DUDA_CONFIG_NUM);
    rpi_config.session_expires = (long int)dconf_value;
    
    dconf_value = fconf->section_key(dconf_sect, "DefaultAccess", DUDA_CONFIG_STR);
    if (dconf_value != NULL) {
        rpi_config.default_allow_flag = rpi_modules_parse_allow_flag((char *)dconf_value);
        mem->free(dconf_value);
    }
    
    dconf_value = fconf->section_key(dconf_sect, "DefaultAllowedUsers", DUDA_CONFIG_LIST);
    if (dconf_value != NULL) {
        mem->free(rpi_config.default_allowed_users);
        rpi_config.default_allowed_users = (struct mk_list *)dconf_value;
    }
}