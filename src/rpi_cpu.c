/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_cpu.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

json_t * rpi_cpu_get_usage(void)
{
    FILE *f;
    int ret;
    long user, nice, system, idle, iowait, irq, softirq;
    json_t *object;
    
    object = json->create_object();
    
    f = fopen("/proc/stat", "r");
    if (f == NULL) {
        return object;
    }
    
    ret = fscanf(f, " cpu %ld %ld %ld %ld %ld %ld %ld", 
                 &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    if (ret != 7) {
        return object;
    }
    
    fclose(f);
    
    long total = user+nice+system+idle+iowait+irq+softirq;
    long busy = user+nice+system+irq+softirq;
    
    json->add_to_object(object, "total", json->create_number((double)total));
    json->add_to_object(object, "busy", json->create_number((double)busy));
    json->add_to_object(object, "user", json->create_number((double)user));
    json->add_to_object(object, "nice", json->create_number((double)nice));
    json->add_to_object(object, "system", json->create_number((double)system));
    json->add_to_object(object, "idle", json->create_number((double)idle));
    json->add_to_object(object, "iowait", json->create_number((double)iowait));
    json->add_to_object(object, "irq", json->create_number((double)irq));
    json->add_to_object(object, "softirq", json->create_number((double)softirq));
    
    return object;
}

json_t * rpi_cpu_get_loadavg(void)
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

json_t *rpi_cpu_get_uptime(void)
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

json_t *rpi_cpu_get_temperature(void)
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