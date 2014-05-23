/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_modules.h"
#include "rpi_config.h"

#include "rpi_memory.h"

static struct mk_list modules_list;
static struct duda_config *modules_config;

/* find module by name */
rpi_module_t * rpi_modules_find(mk_pointer find)
{
    struct mk_list * head;
    rpi_module_t * module;
    
    mk_list_foreach(head, &modules_list) {
        module = mk_list_entry(head, rpi_module_t, _head);
        if (strlen(module->name) == find.len) {
            if (memcmp(find.data, module->name, find.len) == 0) {
                return module;
            }
        }
    }
    
    return NULL;
}

/* takes list of rpi_module_value_t and construct json object of all subvalues */
static json_t * construct_full_json(struct mk_list *values)
{
    json_t *object;
    struct mk_list *entry;
    rpi_module_value_t *value;
    
    object = json->create_object();
    
    mk_list_foreach(entry, values) {
        value = mk_list_entry(entry, rpi_module_value_t, _head);
        if (value->get_value == NULL) {
            json->add_to_object(object, value->name, construct_full_json(&(value->values)));
        } else {
            json->add_to_object(object, value->name, value->get_value());
        }
    }
    
    return object;
}

/* takes path, and constructs corresponding json */
json_t * rpi_modules_json(rpi_module_t *module, char *path)
{
    int i, last;
    rpi_module_value_t *current_value = NULL;
    struct mk_list *entry;
    rpi_module_value_t *entry_value;
    struct mk_list *search_list;
    
    /* root of module */
    if (path[0] == ' ' || path[1] == ' ') {
        return construct_full_json(&(module->values));
    }
    
    for (i = 1, last = 0; ; ++i) {
        /* search slash or end of path */
        if (path[i] != '/' && path[i] != ' ') {
            continue;
        }
        /* take care of multiple slashes */
        if (i == last + 1) {
            if (path[i+1] == ' ') {
                break;
            }
            last = i;
            continue;
        }
        
        /* search module or current value */
        if (current_value == NULL) {
            search_list = &(module->values);
        }
        else {
            if (current_value->get_value != NULL) {
                return NULL;
            }
            search_list = &(current_value->values);
            current_value = NULL;
        }
        
        /* find value with name of this path segment */
        mk_list_foreach(entry, search_list) {
            entry_value = mk_list_entry(entry, rpi_module_value_t, _head);
            if (strlen(entry_value->name) == (i - last - 1)) {
                if (memcmp(path + last + 1, entry_value->name, (i - last - 1)) == 0) {
                    current_value = entry_value;
                    break;
                }
            }
        }
        if (current_value == NULL) {
            return NULL;
        }
        
        /* check for end of path */
        if (path[i] == ' ' || path[i+1] == ' ') {
            break;
        }
        
        last = i;
    }
    
    /* construct json or return value */
    if (current_value->get_value != NULL) {
        return current_value->get_value();
    }
    return construct_full_json(&(current_value->values));
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
rpi_module_t * rpi_modules_module_init(const char *name)
{
    struct duda_config_section *config_sect;
    void *config_value;
    rpi_module_t *module;
    
    /* initialize module */
    module = (rpi_module_t *)mem->alloc(sizeof(rpi_module_t));
    module->name = name;
    mk_list_init(&(module->values));
    mk_list_add(&(module->_head), &modules_list);
    
    /* set defaults from global config */
    module->allow_flag = rpi_config.default_allow_flag;
    module->allowed_users = rpi_config.default_allowed_users;
    
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
    return module;
}

/* initialize module value */
rpi_module_value_t * rpi_modules_value_init(const char *name, 
                                            rpi_module_get_value_t gv, 
                                            struct mk_list *parent)
{
    rpi_module_value_t *value;
    
    value = (rpi_module_value_t *)mem->alloc(sizeof(rpi_module_value_t));
    value->name = name;
    value->get_value = gv;
    mk_list_add(&(value->_head), parent);
    
    return value;
}

/* initialize module branch */
rpi_module_value_t * rpi_modules_branch_init(const char *name,
                                             struct mk_list *parent)
{
    rpi_module_value_t *branch;
    
    branch = (rpi_module_value_t *)mem->alloc(sizeof(rpi_module_value_t));
    branch->name = name;
    branch->get_value = NULL;
    mk_list_init(&(branch->values));
    mk_list_add(&(branch->_head), parent);
    
    return branch;
}

/* initialize modules */
void rpi_modules_init(void)
{
    mk_list_init(&modules_list);
    
    /* config only available during init */
    modules_config = fconf->read_conf("modules.conf");

    /* init modules */
    rpi_memory_init();
    
    fconf->free_conf(modules_config);
    modules_config = NULL;
}