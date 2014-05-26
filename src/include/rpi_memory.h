/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_MEMORY_H
#define RPI_MEMORY_H

typedef struct {
    int total;
    int free;
    int buffers;
    int cached;
    int swap_total;
    int swap_free;
} rpi_memory_buffer_t;

void rpi_memory_init(void);

#endif