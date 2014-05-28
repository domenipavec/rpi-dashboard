/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_modules.h"
#include "rpi_config.h"

#include "rpi_memory.h"
#include "rpi_cpu.h"
#include "rpi_network.h"

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

/* takes rpi_module_value_t and constructs its json object */
static json_t * construct_full_json(duda_request_t *dr, rpi_module_value_t *value)
{
    json_t *object;
    struct mk_list *entry;
    rpi_module_value_t *entry_value;
    
    if (value->get_value != NULL) {
        return value->get_value(dr);
    }
    
    object = json->create_object();
    
    mk_list_foreach(entry, &(value->values)) {
        entry_value = mk_list_entry(entry, rpi_module_value_t, _head);
        json->add_to_object(object, entry_value->name, construct_full_json(dr, entry_value));
    }
    
    return object;
}

/* search for path in json object */
static json_t * json_search(char *path, json_t *object)
{
    int end, beginning;
    json_t *item;
    char *segment;

    /* beginning after initial slashes */
    beginning = 0;
    while (path[beginning] == '/') {
        beginning++;
    }

    /* end at next slash or end of path */
    end = beginning;
    while (path[end] != '/' && path[end] != ' ' && path[end] != '?') {
        end++;
    }
    
    /* no new segment */
    if (end == beginning) {
        return object;
    }
    
    if (object->type != cJSON_Object) {
        return NULL;
    }
    
    segment = mem->alloc(end-beginning+1);
    memcpy(segment, &path[beginning], end-beginning);
    segment[end-beginning] = '\0';
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
                          char *path, 
                          json_t **to_delete)
{
    int end;
    struct mk_list *entry;
    rpi_module_value_t *entry_value;
    
    path = dr->method.data + dr->method.len;

    /* beginning after initial slashes */
    while (path[0] == '/') {
        path++;
    }

    /* end at next slash or end of path */
    end = 0;
    while (path[end] != '/' && path[end] != ' ' && path[end] != '?') {
        end++;
    }

    /* no new segment */
    if (end == 0) {
        *to_delete = construct_full_json(dr, value);
        return *to_delete;
    }

    /* search within current value json if necessary */
    if (value->get_value != NULL) {
        *to_delete = value->get_value(dr);
        return json_search(path, *to_delete);
    }

    /* find value with name of this path segment */
    mk_list_foreach(entry, &(value->values)) {
        entry_value = mk_list_entry(entry, rpi_module_value_t, _head);
        if (strlen(entry_value->name) == end) {
            if (memcmp(path, entry_value->name, end) == 0) {
                return rpi_modules_json(dr, entry_value, &path[end], to_delete);
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
rpi_module_t * rpi_modules_module_init(const char *name, rpi_module_get_value_t gv)
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
    else {
        module->values_head.get_value = gv;
    }
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
    rpi_cpu_init();
    rpi_network_init();
    
    fconf->free_conf(modules_config);
    modules_config = NULL;
}