/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_memory.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

json_t * rpi_memory_get_total(void * buffer)
{
    return json->create_number((double)((rpi_memory_buffer_t *)buffer)->total);
}

json_t * rpi_memory_get_used(void * buffer)
{
    rpi_memory_buffer_t *memory_buffer;
    
    memory_buffer = (rpi_memory_buffer_t *)buffer;
    return json->create_number((double)(memory_buffer->total - memory_buffer->free));
}

json_t * rpi_memory_get_free(void * buffer)
{
    return json->create_number((double)((rpi_memory_buffer_t *)buffer)->free);
}

json_t * rpi_memory_get_buffers(void * buffer)
{
    return json->create_number((double)((rpi_memory_buffer_t *)buffer)->buffers);
}

json_t * rpi_memory_get_cached(void * buffer)
{
    return json->create_number((double)((rpi_memory_buffer_t *)buffer)->cached);
}

json_t * rpi_memory_get_swap_total(void * buffer)
{
    return json->create_number((double)((rpi_memory_buffer_t *)buffer)->swap_total);
}

json_t * rpi_memory_get_swap_used(void * buffer)
{
    rpi_memory_buffer_t *memory_buffer;
    
    memory_buffer = (rpi_memory_buffer_t *)buffer;
    return json->create_number((double)(memory_buffer->swap_total - memory_buffer->swap_free));
}

json_t * rpi_memory_get_swap_free(void * buffer)
{
    return json->create_number((double)((rpi_memory_buffer_t *)buffer)->swap_free);
}

/* find a line in /proc/meminfo, return value */
static int parse_meminfo_entry(FILE *f, const char *key)
{
    int ret;
    int value;
    char current[20];
    
    ret = fscanf(f, " %[^:]: %d kB", current, &value);
    while (ret == 2) {
        if (strcmp(key, current) == 0) {
            return value;
        }
        ret = fscanf(f, " %[^:]: %d kB", current, &value);
    }
    
    return ret;
}

/* process /proc/meminfo into rpi_memory_buffer_t struct */
void * rpi_memory_get_buffer()
{
    rpi_memory_buffer_t *memory_buffer;
    FILE *f;
    
    memory_buffer = mem->alloc_z(sizeof(rpi_memory_buffer_t));
    
    f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        return (void *)memory_buffer;
    }
    
    memory_buffer->total = parse_meminfo_entry(f, "MemTotal");
    memory_buffer->free = parse_meminfo_entry(f, "MemFree");
    memory_buffer->buffers = parse_meminfo_entry(f, "Buffers");
    memory_buffer->cached = parse_meminfo_entry(f, "Cached");
    memory_buffer->swap_total = parse_meminfo_entry(f, "SwapTotal");
    memory_buffer->swap_free = parse_meminfo_entry(f, "SwapFree");
    
    fclose(f);
    return (void *)memory_buffer;
}

void rpi_memory_init(void)
{
    rpi_module_t *module = rpi_modules_module_init("memory", rpi_memory_get_buffer);
    
    rpi_modules_value_init("total", rpi_memory_get_total, &(module->values));
    rpi_modules_value_init("used", rpi_memory_get_used, &(module->values));
    rpi_modules_value_init("free", rpi_memory_get_free, &(module->values));
    rpi_modules_value_init("buffers", rpi_memory_get_buffers, &(module->values));
    rpi_modules_value_init("cached", rpi_memory_get_cached, &(module->values));
    
    rpi_module_value_t *swap = rpi_modules_branch_init("swap", &(module->values));
    rpi_modules_value_init("total", rpi_memory_get_swap_total, &(swap->values));
    rpi_modules_value_init("used", rpi_memory_get_swap_used, &(swap->values));
    rpi_modules_value_init("free", rpi_memory_get_swap_free, &(swap->values));
}