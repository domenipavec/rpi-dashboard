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

#include "rpi_cpu.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

json_t * rpi_cpu_get_usage(duda_request_t *dr)
{
    FILE *f;
    int ret;
    double user, nice, system, idle, iowait, irq, softirq, total, busy;
    json_t *object;
    
    object = json->create_object();
    
    f = fopen("/proc/stat", "r");
    if (f == NULL) {
        return object;
    }
    
    ret = fscanf(f, " cpu %lf %lf %lf %lf %lf %lf %lf", 
                 &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    if (ret != 7) {
        return object;
    }
    
    fclose(f);
    
    total = user+nice+system+idle+iowait+irq+softirq;
    busy = user+nice+system+irq+softirq;
    
    json->add_to_object(object, "total", json->create_number(total));
    json->add_to_object(object, "busy", json->create_number(busy));
    json->add_to_object(object, "user", json->create_number(user));
    json->add_to_object(object, "nice", json->create_number(nice));
    json->add_to_object(object, "system", json->create_number(system));
    json->add_to_object(object, "idle", json->create_number(idle));
    json->add_to_object(object, "iowait", json->create_number(iowait));
    json->add_to_object(object, "irq", json->create_number(irq));
    json->add_to_object(object, "softirq", json->create_number(softirq));
    
    return object;
}

json_t * rpi_cpu_get_loadavg(duda_request_t *dr)
{
    FILE *f;
    int ret;
    double one,five,ten;
    json_t *object;
    
    object = json->create_object();
    
    f = fopen("/proc/loadavg", "r");
    if (f == NULL) {
        return object;
    }

    ret = fscanf(f, " %lf %lf %lf", &one, &five, &ten);
    if (ret != 3) {
        return object;
    }
    
    fclose(f);
    
    json->add_to_object(object, "one", json->create_number(one));
    json->add_to_object(object, "five", json->create_number(five));
    json->add_to_object(object, "ten", json->create_number(ten));
    
    return object;
}

json_t *rpi_cpu_get_uptime(duda_request_t *dr)
{
    FILE *f;
    int ret;
    double uptime;
    
    f = fopen("/proc/uptime", "r");
    if (f == NULL) {
        return json->create_number(0.0);
    }

    ret = fscanf(f, " %lf", &uptime);
    if (ret != 1) {
        return json->create_number(0.0);
    }
    
    fclose(f);
    
    return json->create_number(uptime);
}

json_t *rpi_cpu_get_temperature(duda_request_t *dr)
{
    FILE *f;
    int ret;
    double temp;
    
    f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (f == NULL) {
        return json->create_number(0.0);
    }

    ret = fscanf(f, " %lf", &temp);
    if (ret != 1) {
        return json->create_number(0.0);
    }
    
    fclose(f);
    
    return json->create_number(temp/1000.0);
}

/* register and initialize module */
void rpi_cpu_init(void)
{
    rpi_module_t *module = rpi_modules_module_init("cpu", NULL);
    
    rpi_modules_value_init("usage", rpi_cpu_get_usage, &(module->values_head.values));
    rpi_modules_value_init("loadavg", rpi_cpu_get_loadavg, &(module->values_head.values));
    rpi_modules_value_init("uptime", rpi_cpu_get_uptime, &(module->values_head.values));
    rpi_modules_value_init("temperature", rpi_cpu_get_temperature, &(module->values_head.values));
}