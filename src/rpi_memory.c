/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_memory.h"

#include "rpi_modules.h"

#include "packages/json/json.h"


/* find a line in /proc/meminfo, return value */
static long parse_meminfo_entry(FILE *f, const char *key)
{
    int ret;
    long value;
    char current[20];
    
    ret = fscanf(f, " %[^:]: %ld kB", current, &value);
    while (ret == 2) {
        if (strcmp(key, current) == 0) {
            return value;
        }
        ret = fscanf(f, " %[^:]: %ld kB", current, &value);
    }
    
    return ret;
}

/* return json object of all values */
json_t * rpi_memory_get(void)
{
    FILE *f;
    json_t *ret;
    json_t *swap;
    long total, free, buffers, cached, swap_total, swap_free;

    ret = json->create_object();
    
    f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        return ret;
    }
    
    total = parse_meminfo_entry(f, "MemTotal");
    free = parse_meminfo_entry(f, "MemFree");
    buffers = parse_meminfo_entry(f, "Buffers");
    cached = parse_meminfo_entry(f, "Cached");
    swap_total = parse_meminfo_entry(f, "SwapTotal");
    swap_free = parse_meminfo_entry(f, "SwapFree");

    fclose(f);

    json->add_to_object(ret, "total", json->create_number((double)total));
    json->add_to_object(ret, "used", json->create_number((double)(total - free)));
    json->add_to_object(ret, "free", json->create_number((double)free));
    json->add_to_object(ret, "buffers", json->create_number((double)buffers));
    json->add_to_object(ret, "cached", json->create_number((double)cached));
    
    swap = json->create_object();
    json->add_to_object(ret, "swap", swap);
    json->add_to_object(swap, "total", json->create_number((double)swap_total));
    json->add_to_object(swap, "used", json->create_number((double)(swap_total - swap_free)));
    json->add_to_object(swap, "free", json->create_number((double)swap_free));

    return ret;
}

/* register and initialize module */
void rpi_memory_init(void)
{
    rpi_modules_module_init("memory", rpi_memory_get);
}