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

#ifndef RPI_LOGGER_H
#define RPI_LOGGER_H

#include "mk_memory.h"
#include "mk_list.h"

typedef struct {
    mk_pointer module;
    const char *path;
    const char *dst; /* data source type for rrd */
    const char *name_part;
    
    struct mk_list _head;
} rpi_logger_group_t;

typedef struct {
    const char *name;
    char string[30];
    int steps;
    int rows;
    
    struct mk_list _head;
} rpi_logger_rra_t;

void rpi_logger_init(void);

#endif