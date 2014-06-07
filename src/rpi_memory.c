/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_memory.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#define KB_MULTIPLIER (1024.0)

/* find a line in /proc/meminfo, return value */
static double parse_meminfo_entry(FILE *f, const char *key)
{
    int ret;
    double value;
    char current[20];
    
    ret = fscanf(f, " %[^:]: %lf kB", current, &value);
    while (ret == 2) {
        if (strcmp(key, current) == 0) {
            return value;
        }
        ret = fscanf(f, " %[^:]: %lf kB", current, &value);
    }
    
    return -1;
}

/* return json object of all values */
json_t * rpi_memory_get(duda_request_t *dr)
{
    FILE *f;
    json_t *ret;
    json_t *swap;
    double total, free, buffers, cached, swap_total, swap_free;

    ret = json->create_object();
    
    f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        return ret;
    }
    
    total = parse_meminfo_entry(f, "MemTotal")*KB_MULTIPLIER;
    free = parse_meminfo_entry(f, "MemFree")*KB_MULTIPLIER;
    buffers = parse_meminfo_entry(f, "Buffers")*KB_MULTIPLIER;
    cached = parse_meminfo_entry(f, "Cached")*KB_MULTIPLIER;
    swap_total = parse_meminfo_entry(f, "SwapTotal")*KB_MULTIPLIER;
    swap_free = parse_meminfo_entry(f, "SwapFree")*KB_MULTIPLIER;

    fclose(f);

    json->add_to_object(ret, "total", json->create_number(total));
    json->add_to_object(ret, "used", json->create_number(total - free));
    json->add_to_object(ret, "free", json->create_number(free));
    json->add_to_object(ret, "buffers", json->create_number(buffers));
    json->add_to_object(ret, "cached", json->create_number(cached));
    
    swap = json->create_object();
    json->add_to_object(ret, "swap", swap);
    json->add_to_object(swap, "total", json->create_number(swap_total));
    json->add_to_object(swap, "used", json->create_number(swap_total - swap_free));
    json->add_to_object(swap, "free", json->create_number(swap_free));

    return ret;
}

/* register and initialize module */
void rpi_memory_init(void)
{
    rpi_modules_module_init("memory", rpi_memory_get);
}