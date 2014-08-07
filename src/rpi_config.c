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

#include "rpi_config.h"
#include "rpi_modules.h"

#include "webservice.h"

rpi_config_t rpi_config;

static void load_default_values(void)
{
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
        msg->warn("Could not read 'rpi.conf' configuration files, using defaults.");
        return;
    }
    
    dconf_sect = fconf->section_get(dconf, "GLOBAL");
    if (dconf_sect == NULL) {
        fconf->free_conf(dconf);
        return;
    }
    
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
    
    fconf->free_conf(dconf);
}