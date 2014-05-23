/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_modules.h"

#include "packages/json/json.h"

json_t * rpi_memory_get_total()
{
    return json->create_number(1000.0);
}

json_t * rpi_memory_get_used()
{
    return json->create_number(375.0);
}

json_t * rpi_memory_get_free()
{
    return json->create_number(625.0);
}

json_t * rpi_memory_get_buffers()
{
    return json->create_number(100.0);
}

json_t * rpi_memory_get_cached()
{
    return json->create_number(25.0);
}

json_t * rpi_memory_get_swap_total()
{
    return json->create_number(2000.0);
}

json_t * rpi_memory_get_swap_used()
{
    return json->create_number(0.0);
}

json_t * rpi_memory_get_swap_free()
{
    return json->create_number(2000.0);
}

void rpi_memory_init(void)
{
    rpi_module_t *module = rpi_modules_module_init("memory");
    
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