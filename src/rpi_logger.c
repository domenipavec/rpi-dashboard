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
static struct mk_list rra_list;
static int rra_list_len;

#define assert_rrd(x) if ((x) != 0) __assert(rrd_get_context()->rrd_error, __FILE__, __LINE__)

static const char *DST_GAUGE = "GAUGE";
static const char *DST_COUNTER = "COUNTER";

/* add a group of values */
static void add_group(char *module, const char *path, const char *dst)
{
    rpi_logger_group_t *g;
    rpi_module_t *rpi_module;
    json_t *json_object;
    json_t *json_delete;
    char *name_part;
    int i;

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

    /* combine module and path to name_part */
    if (strlen(path) > 0) {
        name_part = rpi_string_concatN(3, module, "-", path);

        for (i = 0; i < strlen(name_part); i++) {
            if (name_part[i] == '/') {
                name_part[i] = '-';
            }
        }
        g->name_part = name_part;
    } else {
        g->name_part = rpi_string_concat(module, "");
    }

    mk_list_add(&(g->_head), &groups_list);

    json->delete(json_delete);
}

/* initialize groups of values to be logged */
static void groups_init()
{
    mk_list_init(&groups_list);

    add_group("cpu", "usage", DST_COUNTER);
    add_group("cpu", "temperature", DST_GAUGE);
    add_group("memory", "", DST_GAUGE);
    add_group("storage", "throughput", DST_COUNTER);
    add_group("network", "packets", DST_COUNTER);
    add_group("network", "bytes", DST_COUNTER);
}

static void add_rra(const char *name, int steps, int rows)
{
    rpi_logger_rra_t *r;
    
    r = (rpi_logger_rra_t *)mem->alloc(sizeof(rpi_logger_rra_t));
    r->name = name;
    r->steps = steps;
    r->rows = rows;
    snprintf(r->string, sizeof(r->string), "RRA:AVERAGE:0.5:%d:%d", steps, rows);
    
    mk_list_add(&(r->_head), &rra_list);
    rra_list_len++;
}

/* initialize a list of RRAs for database */
static void rra_init()
{
    mk_list_init(&rra_list);
    rra_list_len = 0;

    add_rra("hour", 1, 60); // an hour, every min
    add_rra("day", 30, 48); // a day, every 30min
    add_rra("week", 180, 56); // 7 days, every 3 hours
    add_rra("month", 720, 60); // 30 days, 2 times a day
}

/* use rrd_create_r to initialize rrd database */
static void create_rrdfile(const char *fn, const char *dst)
{
    int i = 1;
    struct mk_list *entry;
    rpi_logger_rra_t *entry_rra;
    const char *params[rra_list_len + 1];
    char *ds;
    
    ds = rpi_string_concatN(3, "DS:value:", dst, ":90:U:U");
    params[0] = ds;
    mk_list_foreach(entry, &rra_list) {
        entry_rra = mk_list_entry(entry, rpi_logger_rra_t, _head);
        params[i] = entry_rra->string;
        i++;
    }

    assert_rrd(rrd_create_r(fn, 60, time(NULL), rra_list_len + 1, params));

    mem->free(ds);
}

static void parse_value(const json_t *json_value, const char *name, const char *dst)
{
    char *file_name;
    char str_value[30];
    const char *params[1];
    params[0] = str_value;

    file_name = rpi_string_concatN(3, data->get_path(), name, ".rrd");
    if (access(file_name, F_OK) == -1) {
        create_rrdfile(file_name, dst);
    }

    if (dst == DST_COUNTER) {
        snprintf(str_value, sizeof(str_value), "N:%.0f", json_value->valuedouble);
    } else {
        snprintf(str_value, sizeof(str_value), "N:%f", json_value->valuedouble);
    }

    assert_rrd(rrd_update_r(file_name, "value", 1, params));

    mem->free(file_name);
}

static void parse_json(const json_t *json_object, const char *name_part, const char *dst)
{
    json_t *child_json_object;
    char *child_name_part;
    char *name_part_extend;
    
    if (json_object->type == cJSON_Number) {
        parse_value(json_object, name_part, dst);
        return;
    }
    if (json_object->type == cJSON_Object) {
        name_part_extend = rpi_string_concat(name_part, "-");
        for (child_json_object = json_object->child;
             child_json_object != NULL;
             child_json_object = child_json_object->next) {
            child_name_part = rpi_string_concat(name_part_extend, child_json_object->string);
            parse_json(child_json_object, child_name_part, dst);
            mem->free(child_name_part);
        }
        mem->free(name_part_extend);
    }
}

static void rpi_logger_update()
{
    rpi_logger_group_t *entry_group;
    struct mk_list *entry;
    json_t *json_object;
    json_t *json_delete;
    
    mk_list_foreach(entry, &groups_list) {
        entry_group = mk_list_entry(entry, rpi_logger_group_t, _head);
        json_object = rpi_modules_json(NULL, 
                                       &(rpi_modules_find(entry_group->module)->values_head), 
                                       entry_group->path, 
                                       &json_delete);

        parse_json(json_object, entry_group->name_part, entry_group->dst);

        mem->free(json_delete);
    };
}

static void * rpi_logger_worker(void *arg)
{
    for(;;) {
        /* sleep to time aligned to minute */
        sleep(60 - (time(NULL)%60));

        rpi_logger_update();
    }

    return NULL;
}

void rpi_logger_init(void)
{
    groups_init();
    rra_init();
    
    worker->spawn(rpi_logger_worker, NULL);
}