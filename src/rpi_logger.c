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
#include <float.h>
#include <math.h>

#define assert_rrd(x) if ((x) != 0) msg->err("RRD ERROR: %s", rrd_get_context()->rrd_error)

static struct mk_list groups_list;
#define RRA_LIST_LEN 5
static rpi_logger_rra_t rra_list[RRA_LIST_LEN];

static const char *DST_GAUGE = "GAUGE";
static const char *DST_COUNTER = "COUNTER";

/* init a group of values */
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
    if (rpi_module == NULL) {
        msg->err("LOGGER: Module '%s' does not exist!", module);
        exit(-1);
    }

    json_object = rpi_modules_json(NULL, &(rpi_module->values_head), g->path, &json_delete, 0);
    if (json_object == NULL) {
        msg->err("LOGGER: Path '%s' does not exist in module '%s'!", path, module);
        exit(-1);
    }

    // combine module and path to name_part
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
static void groups_init(void)
{
    mk_list_init(&groups_list);

    add_group("cpu", "usage", DST_COUNTER);
    add_group("cpu", "temperature", DST_GAUGE);
    add_group("memory", "", DST_GAUGE);
    add_group("storage", "throughput", DST_COUNTER);
    add_group("network", "packets", DST_COUNTER);
    add_group("network", "bytes", DST_COUNTER);
}

/* init rra */
static void add_rra(int i, const char *name, int steps, int rows)
{
    rra_list[i].name = name;
    rra_list[i].steps = steps;
    rra_list[i].rows = rows;
    snprintf(rra_list[i].string, sizeof(rra_list[i].string), "RRA:AVERAGE:0.5:%d:%d", steps, rows);
}

/* initialize a list of RRAs for database */
static void rra_init(void)
{
    add_rra(0, "hour", 1, 61); // an hour, every min
    add_rra(1, "day", 30, 49); // a day, every 30min
    add_rra(2, "week", 180, 57); // 7 days, every 3 hours
    add_rra(3, "month", 720, 61); // 30 days, 2 times a day
    add_rra(4, "year", 8760, 61); // 365 days, 5 times a month
}

/* use rrd_create_r to initialize rrd database */
static void create_rrdfile(const char *fn, const char *dst)
{
    int i;
    const char *params[RRA_LIST_LEN + 1];
    char *ds;
    
    ds = rpi_string_concatN(3, "DS:value:", dst, ":90:U:U");
    params[0] = ds;
    for (i = 0; i < RRA_LIST_LEN; i++) {
        params[i+1] = rra_list[i].string;
    }

    assert_rrd(rrd_create_r(fn, 60, time(NULL), RRA_LIST_LEN + 1, params));

    mem->free(ds);
}

/* get filename for value name */
static char * get_filename(const char *name) {
    return rpi_string_concatN(4, data->get_path(), "logger/", name, ".rrd");
}

/* store value in rrd */
static void parse_value(const json_t *json_value, const char *name, const char *dst)
{
    char *file_name;
    char str_value[30];
    const char *params[1];
    params[0] = str_value;

    // create rrd file if it does not exist
    file_name = get_filename(name);
    if (access(file_name, F_OK) == -1) {
        msg->info("LOGGER: Creating '%s.rrd' file.", name);
        create_rrdfile(file_name, dst);
    }

    // print int for counter or double
    if (dst == DST_COUNTER) {
        snprintf(str_value, sizeof(str_value), "N:%.0f", json_value->valuedouble);
    } else {
        snprintf(str_value, sizeof(str_value), "N:%f", json_value->valuedouble);
    }

    // store in rrd
    assert_rrd(rrd_update_r(file_name, "value", 1, params));

    mem->free(file_name);
}

/* parse json object for storing in rrd */
static void parse_json(const json_t *json_object, const char *name_part, const char *dst)
{
    json_t *child_json_object;
    char *child_name_part;
    char *name_part_extend;
    
    // store final value to rrd file
    if (json_object->type == cJSON_Number) {
        parse_value(json_object, name_part, dst);
        return;
    }
    
    // parse json tree and store each value
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

/* populate rrd file with a set of values */
static void rpi_logger_update(void)
{
    rpi_logger_group_t *entry_group;
    struct mk_list *entry;
    json_t *json_object;
    json_t *json_delete;
    
    // get json for each group and parse it
    mk_list_foreach(entry, &groups_list) {
        entry_group = mk_list_entry(entry, rpi_logger_group_t, _head);
        json_object = rpi_modules_json(NULL, 
                                       &(rpi_modules_find(entry_group->module)->values_head), 
                                       entry_group->path, 
                                       &json_delete, 0);

        parse_json(json_object, entry_group->name_part, entry_group->dst);

        mem->free(json_delete);
    };
}

/* log worker function */
static void * rpi_logger_worker(void *arg)
{
    for(;;) {
        /* sleep to time aligned to minute */
        sleep(60 - (time(NULL)%60));

        rpi_logger_update();
    }

    return NULL;
}

/* get log data for rra and value name */
static json_t *rpi_logger_get_value(int rrai, const char *name) 
{
    char *file_name;
    json_t *object;
    json_t *array;
    time_t start, end, ti;
    unsigned long step, ds_cnt, i;
    char **ds_namv;
    rrd_value_t *values;
    rrd_value_t *value;
    double min, max;
    min = NAN;
    max = NAN;

    // check if file accessible
    file_name = get_filename(name);
    if (access(file_name, F_OK) == -1) {
        mem->free(file_name);
        return json->create_null();
    }
    
    object = json->create_object();
    
    // calculate time frame
    step = 60*rra_list[rrai].steps;
    end = time(NULL);
    end -= end%step;
    start = end - (step*rra_list[rrai].rows);
    
    // fetch data from file
    assert_rrd(rrd_fetch_r(file_name, "AVERAGE", &start, &end, &step, &ds_cnt, &ds_namv, &values));
    value = values;
    
    // header data
    json->add_to_object(object, "end", json->create_number((double)(end-step)));
    json->add_to_object(object, "start", json->create_number((double)(start+step)));
    json->add_to_object(object, "step", json->create_number((double)step));
    
    // populate data to json
    array = json->create_array();
    for (ti = start + step; ti < end; ti += step) {
        if (max != max || *value > max) {
            max = *value;
        }
        if (min != min || *value < min) {
            min = *value;
        }
        if (*value != *value) {
            json->add_to_array(array, json->create_null());
        } else {
            json->add_to_array(array, json->create_number(*value));
        }
        value++;
    }
    json->add_to_object(object, "data", array);
    
    // init min and max if enough data
    if (min != min) {
        json->add_to_object(object, "min", json->create_null());
    } else {
        json->add_to_object(object, "min", json->create_number(min));
    }
    if (max != max) {
        json->add_to_object(object, "min", json->create_null());
    } else {
        json->add_to_object(object, "max", json->create_number(max));
    }
    
    // free all rrd objects
    for (i = 0; i < ds_cnt; ++i) {
        free(ds_namv[i]);
    }
    free(ds_namv);
    free(values);

    mem->free(file_name);
    return object;
}

/* get log data for rra */
static json_t *rpi_logger_get(int rrai, duda_request_t *dr)
{
    char *value;
    json_t *object;
    int i, last, len;
    
    object = json->create_object();
    
    // value specified in query string
    value = qs->get(dr, "value");
    if (value == NULL) {
        return object;
    }
    
    // iterate over values, separated by '|'
    len = strlen(value);
    for (last = (i = 0); i < len; i++) {
        if (value[i] == '/') {
            value[i] = '-';
        }
        if (value[i] == '|' || value[i] == '~') {
            value[i] = '\0';
            json->add_to_object(object, value+last, rpi_logger_get_value(rrai, value + last));
            last = i+1;
        }
    }
    json->add_to_object(object, value+last, rpi_logger_get_value(rrai, value + last));
    
    return object;
}

json_t *rpi_logger_get_hour(duda_request_t *dr, int parameter)
{
    return rpi_logger_get(0, dr);
}
json_t *rpi_logger_get_day(duda_request_t *dr, int parameter)
{
    return rpi_logger_get(1, dr);
}
json_t *rpi_logger_get_week(duda_request_t *dr, int parameter)
{
    return rpi_logger_get(2, dr);
}
json_t *rpi_logger_get_month(duda_request_t *dr, int parameter)
{
    return rpi_logger_get(3, dr);
}
json_t *rpi_logger_get_year(duda_request_t *dr, int parameter)
{
    return rpi_logger_get(4, dr);
}

static void module_init()
{
    rpi_module_t *module = rpi_modules_module_init("logger", NULL, NULL);
    
    if (module != NULL) {
        rpi_modules_value_init("hour", rpi_logger_get_hour, NULL, &(module->values_head.values));
        rpi_modules_value_init("day", rpi_logger_get_day, NULL, &(module->values_head.values));
        rpi_modules_value_init("week", rpi_logger_get_week, NULL, &(module->values_head.values));
        rpi_modules_value_init("month", rpi_logger_get_month, NULL, &(module->values_head.values));
        rpi_modules_value_init("year", rpi_logger_get_year, NULL, &(module->values_head.values));
    }
}

void rpi_logger_init(void)
{
    if (data->get_path() == NULL) {
        msg->err("LOGGER: Path for data folder is not specified.");
        exit(-1);
    }

    // init groups of values to be logged
    groups_init();
    // init rras
    rra_init();
    // init web access
    module_init();

    // manually create log thread, duda worker clashes with websockets for some reason
    pthread_t tid;
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&tid, &thread_attr, rpi_logger_worker, NULL) != 0) {
        msg->err("Could not start logger.");
        exit(-1);
    }
}
