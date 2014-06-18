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

#include "rpi_logger.h"
#include "rpi_string.h"
#include "rpi_modules.h"

#include "webservice.h"
#include "packages/json/json.h"

#include <rrd.h>
#include <time.h>
#include <assert.h>

static struct mk_list groups_list;
static struct mk_list values_list;
static int values_list_len;
static struct mk_list rra_list;
static int rra_list_len;

/* parse json to list of ds_list of names */
static void parse_json_values(json_t *json_object, char *name_part, const char *dst)
{
    if (json_object->type == cJSON_Number) {
        /* add all number types to the values_list */
        rpi_logger_value_t *value = (rpi_logger_value_t *)mem->alloc(sizeof(rpi_logger_value_t));
        value->name = name_part;
        value->dst = dst;
        value->ds_name = (char *)mem->alloc(5*sizeof(char));
        sprintf(value->ds_name, "v%03d", values_list_len);
        mk_list_add(&(value->_head), &values_list);
        values_list_len++;
        return;
    }
    if (json_object->type == cJSON_Object) {
        /* extend name_part and iterate over children */
        char *name_part1 = rpi_string_concat(name_part, "-");
        json_t *child_json_object;
        for (child_json_object = json_object->child;
             child_json_object != NULL;
             child_json_object = child_json_object->next) {
            parse_json_values(child_json_object, rpi_string_concat(name_part1, child_json_object->string), dst);
        }
        mem->free(name_part1);
    }
    mem->free(name_part);
}

/* add a group of values */
static void add_group(char *module, const char *path, const char *dst)
{
    rpi_logger_group_t *g;
    rpi_module_t *rpi_module;
    json_t *json_object;
    json_t *json_delete;
    int i;
    char *name_part;
    char *name_part1;

    /* add group to list if module and path exist */
    g = (rpi_logger_group_t *)mem->alloc(sizeof(rpi_logger_group_t));
    g->module.data = module;
    g->module.len = strlen(module);
    g->path = path;
    g->dst = dst;

    rpi_module = rpi_modules_find(g->module);
    assert(rpi_module != NULL);

    json_object = rpi_modules_json(NULL, &(rpi_module->values_head), g->path, &json_delete);
    assert(json_object != NULL);
    
    mk_list_add(&(g->_head), &groups_list);
    
    /* combine module and path to name_part */
    if (strlen(path) > 0) {
        name_part1 = rpi_string_concat(module, "-");
        name_part = rpi_string_concat(name_part1, path);
        mem->free(name_part1);

        for (i = 0; i < strlen(name_part); i++) {
            if (name_part[i] == '/') {
                name_part[i] = '-';
            }
        }
    } else {
        name_part = rpi_string_concat(module, "");
    }

    /* parse json for values */
    parse_json_values(json_object, name_part, dst);
    json->delete(json_delete);
}

/* initialize groups of values to be logged */
static void groups_init()
{
    mk_list_init(&groups_list);
    mk_list_init(&values_list);
    values_list_len = 0;
    
    add_group("cpu", "usage", DST_COUNTER);
    add_group("cpu", "temperature", DST_GAUGE);
    add_group("memory", "", DST_GAUGE);
    add_group("storage", "throughput", DST_COUNTER);
    //add_group("network", "packets", DST_COUNTER);
    //add_group("network", "bytes", DST_COUNTER);
}

static void add_rra(const char *name, int steps, int rows)
{
    rpi_logger_rra_t *r;
    
    r = (rpi_logger_rra_t *)mem->alloc(sizeof(rpi_logger_rra_t));
    r->name = name;
    r->steps = steps;
    r->rows = rows;
    sprintf(r->string, "RRA:AVERAGE:0.5:%d:%d", steps, rows);
    
    mk_list_add(&(r->_head), &rra_list);
    rra_list_len++;
}

/* initialize a list of RRAs for database */
static void rra_init()
{
    mk_list_init(&rra_list);
    rra_list_len = 0;

    add_rra("minute", 1, 60); // a minute, every s
    add_rra("hour", 60, 60); // an hour, every min
    add_rra("day", 1800, 48); // a day, every 30min
    add_rra("week", 10800, 56); // 7 days, every 3 hours
    add_rra("month", 43200, 60); // 30 days, 2 times a day
}

/* use rrd_create_r to initialize rrd database */
static void rrdfile_init()
{
    char *path;
    int params_count = values_list_len + rra_list_len;
    int i = 0;
    int ret_rrd_create_r;
    struct mk_list *entry;
    rpi_logger_value_t *entry_value;
    rpi_logger_rra_t *entry_rra;
    const char *params[params_count];
    
    mk_list_foreach(entry, &values_list) {
        entry_value = mk_list_entry(entry, rpi_logger_value_t, _head);
        params[i] = rpi_string_concatN(5, "DS:", entry_value->ds_name, ":", entry_value->dst, ":1:U:U");
        i++;
    }
    mk_list_foreach(entry, &rra_list) {
        entry_rra = mk_list_entry(entry, rpi_logger_rra_t, _head);
        params[i] = entry_rra->string;
        i++;
    }
    
    path = rpi_string_concat(data->get_path(), "log.rrd");
    
    ret_rrd_create_r = rrd_create_r(path, 1, time(NULL), params_count, params);
    assert(ret_rrd_create_r == 0);

    mem->free(path);
}

void rpi_logger_init(void)
{
    groups_init();
    rra_init();
    rrdfile_init();
}