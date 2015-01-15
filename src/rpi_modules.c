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

#include "rpi_modules.h"
#include "rpi_config.h"
#include "rpi_security.h"

#include "rpi_general.h"
#include "rpi_memory.h"
#include "rpi_cpu.h"
#include "rpi_network.h"
#include "rpi_storage.h"
#include "rpi_logger.h"
#include "rpi_gpio.h"
#include "rpi_i2c.h"
#include "rpi_spi.h"
#include "rpi_serial.h"
#include "rpi_shift.h"
#include "rpi_settings.h"

static struct mk_list modules_list;
static struct duda_config *modules_config;

/* find module by name */
rpi_module_t * rpi_modules_find(mk_pointer find)
{
    struct mk_list * head;
    rpi_module_t * module;
    
    mk_list_foreach(head, &modules_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (strlen(module->values_head.name) == find.len) {
            if (memcmp(find.data, module->values_head.name, find.len) == 0) {
                return module;
            }
        }
    }
    
    return NULL;
}

/* find all modules that user has permission to access */
json_t * rpi_modules_user_list(char *user)
{
    struct mk_list * head;
    rpi_module_t * module;
    json_t *array;
    
    array = json->create_array();
    
    mk_list_foreach(head, &modules_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (rpi_security_check_permission(user, module, MK_FALSE) == 0) {
            json->add_to_array(array, json->create_string(module->values_head.name));
        }
    }
    
    return array;
}

/* find all modules that user has write access */
json_t * rpi_modules_user_write_list(char *user)
{
    struct mk_list * head;
    rpi_module_t * module;
    json_t *array;
    
    array = json->create_array();
    
    mk_list_foreach(head, &modules_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (rpi_security_check_permission(user, module, MK_TRUE) == 0) {
            json->add_to_array(array, json->create_string(module->values_head.name));
        }
    }
    
    return array;
}

/* takes rpi_module_value_t and constructs its json object */
static json_t * construct_full_json(duda_request_t *dr, rpi_module_value_t *value, int parameter)
{
    json_t *object;
    struct mk_list *entry;
    rpi_module_value_t *entry_value;
    int min, max, i;
    char name_buffer[4];
    
    if (value->get_value != NULL) {
        return value->get_value(dr, parameter);
    }
    
    object = json->create_object();
    
    mk_list_foreach(entry, &(value->values)) {
        entry_value = mk_list_entry(entry, rpi_module_value_t, _head);
        
        // process parameter values
        if (strncmp(entry_value->name, "%d", 2) == 0) {
            if (sscanf(entry_value->name + 2, "%d:%d", &min, &max) == 2 && (max-min) < 100) {
                for (i = min; i <= max; i++) {
                    snprintf(name_buffer, 4, "%d", i);
                    json->add_to_object(object, name_buffer, construct_full_json(dr, entry_value, i));
                }
            }
        } else {
            json->add_to_object(object, entry_value->name, construct_full_json(dr, entry_value, parameter));
        }
    }
    
    return object;
}

/* search for path in json object */
static json_t * json_search(const char *path, json_t *object)
{
    int end;
    json_t *item;
    char *segment;
    
    /* beginning after initial slashes */
    while (path[0] == '/') {
        path++;
    }

    /* end at next slash or end of path */
    end = 0;
    while (path[end] != '/' && path[end] != ' ' && path[end] != '?' && path[end] != '\0') {
        end++;
    }
    
    /* no new segment */
    if (end == 0) {
        return object;
    }
    
    if (object->type != cJSON_Object) {
        return NULL;
    }
    
    segment = mem->alloc(end+1);
    memcpy(segment, path, end);
    segment[end] = '\0';

    item = json->get_object_item(object, segment);
    mem->free(segment);
    if (item == NULL) {
        return NULL;
    }
    return json_search(&path[end], item);
}

/* takes path, and finds corresponding json in value */
json_t * rpi_modules_json(duda_request_t *dr, 
                          rpi_module_value_t *value, 
                          const char *path, 
                          json_t **to_delete,
                          int parameter)
{
    int end, min, max, newpar;
    struct mk_list *entry;
    rpi_module_value_t *entry_value;
    json_t *data;

    /* beginning after initial slashes */
    while (path[0] == '/') {
        path++;
    }

    /* end at next slash or end of path */
    end = 0;
    while (path[end] != '/' && path[end] != ' ' && path[end] != '?' && path[end] != '\0') {
        end++;
    }

    /* no new segment */
    if (end == 0) {
        if (dr == NULL || !request->is_data(dr)) {
            *to_delete = construct_full_json(dr, value, parameter);
        } else {
            if (value->post_value == NULL) {
                return NULL;
            }
            data = json->parse_data(dr);
            if (data == NULL) {
                return NULL;
            }
            *to_delete = value->post_value(dr, data, parameter);
            json->delete(data);
        }
        return *to_delete;
    }

    /* search within current value json if necessary */
    if (value->get_value != NULL) {
        *to_delete = value->get_value(dr, parameter);
        return json_search(path, *to_delete);
    }

    /* find value with name of this path segment */
    mk_list_foreach(entry, &(value->values)) {
        entry_value = mk_list_entry(entry, rpi_module_value_t, _head);

        // parse parameter
        if (strncmp(entry_value->name, "%d", 2) == 0) {
            if (sscanf(entry_value->name + 2, "%d:%d", &min, &max) == 2) {
                newpar = atoi(path);
                if (newpar >= min && newpar <= max) {
                    return rpi_modules_json(dr, entry_value, &path[end], to_delete, newpar);
                }
            }
        }
        
        if (strlen(entry_value->name) == end) {
            if (memcmp(path, entry_value->name, end) == 0) {
                return rpi_modules_json(dr, entry_value, &path[end], to_delete, parameter);
            }
        }
    }

    return NULL;
}

/* parse allow flag from string */
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

/* initialize one module */
rpi_module_t * rpi_modules_module_init(const char *name,
                                       rpi_module_get_value_t gv,
                                       rpi_module_post_value_t pv)
{
    struct duda_config_section *config_sect;
    void *config_value;
    rpi_module_t *module;
    
    /* initialize module */
    module = (rpi_module_t *)mem->alloc(sizeof(rpi_module_t));
    module->values_head.name = name;
    if (gv == NULL) {
        mk_list_init(&(module->values_head.values));
    }
    module->values_head.get_value = gv;
    module->values_head.post_value = pv;
    mk_list_add(&(module->_head), &modules_list);
    
    /* set defaults from global config */
    module->allow_flag = rpi_config.default_allow_flag;
    module->allowed_users = rpi_config.default_allowed_users;
    module->allow_write_flag = rpi_config.default_allow_write_flag;
    module->allowed_write_users = rpi_config.default_allowed_write_users;

    /* try to load config */
    if (modules_config == NULL) {
        return module;
    }
    config_sect = fconf->section_get(modules_config, name);
    if (config_sect == NULL) {
        return module;
    }

    /* delete disabled modules */
    if (fconf->section_key(config_sect, "Disabled", DUDA_CONFIG_BOOL) == (void *)MK_TRUE) {
        mk_list_del(&(module->_head));
        mem->free(module);
        return NULL;
    }

    /* parse settings */
    config_value = fconf->section_key(config_sect, "Access", DUDA_CONFIG_STR);
    if (config_value != NULL) {
        module->allow_flag = rpi_modules_parse_allow_flag((char *)config_value);
        mem->free(config_value);
    }
    config_value = fconf->section_key(config_sect, "AllowedUsers", DUDA_CONFIG_LIST);
    if (config_value != NULL) {
        module->allowed_users = (struct mk_list *)config_value;
    }
    config_value = fconf->section_key(config_sect, "WriteAccess", DUDA_CONFIG_STR);
    if (config_value != NULL) {
        module->allow_write_flag = rpi_modules_parse_allow_flag((char *)config_value);
        mem->free(config_value);
    }
    config_value = fconf->section_key(config_sect, "AllowedWriteUsers", DUDA_CONFIG_LIST);
    if (config_value != NULL) {
        module->allowed_write_users = (struct mk_list *)config_value;
    }

    return module;
}

/* initialize module value */
rpi_module_value_t * rpi_modules_value_init(const char *name, 
                                            rpi_module_get_value_t gv,
                                            rpi_module_post_value_t pv,
                                            struct mk_list *parent)
{
    rpi_module_value_t *value;
    
    value = (rpi_module_value_t *)mem->alloc(sizeof(rpi_module_value_t));
    value->name = name;
    value->get_value = gv;
    value->post_value = pv;
    mk_list_add(&(value->_head), parent);
    
    return value;
}

/* initialize module branch */
rpi_module_value_t * rpi_modules_branch_init(const char *name,
                                             rpi_module_post_value_t pv,
                                             struct mk_list *parent)
{
    rpi_module_value_t *branch;
    
    branch = (rpi_module_value_t *)mem->alloc(sizeof(rpi_module_value_t));
    branch->name = name;
    branch->get_value = NULL;
    branch->post_value = pv;
    mk_list_init(&(branch->values));
    mk_list_add(&(branch->_head), parent);
    
    return branch;
}

/* initialize modules */
void rpi_modules_init(void)
{
    mk_list_init(&modules_list);

    if (fconf->get_path() == NULL) {
        msg->err("Path for configuration folder not specified");
        exit(-1);
    }

    /* config only available during init */
    modules_config = fconf->read_conf("modules.conf");
    if (modules_config == NULL) {
        msg->err("Could not read 'modules.conf' configuration file.");
        exit(-1);
    }

    /* init modules */
    rpi_general_init();
    rpi_memory_init();
    rpi_cpu_init();
    rpi_network_init();
    rpi_storage_init();
    rpi_gpio_init();
    rpi_i2c_init();
    rpi_spi_init();
    rpi_serial_init();
    rpi_shift_init();
    rpi_settings_init();
    
    /* init logger, after all other modules */
    rpi_logger_init();
    
    fconf->free_conf(modules_config);
    modules_config = NULL;
}
