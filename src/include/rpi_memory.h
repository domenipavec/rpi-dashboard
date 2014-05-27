/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_MEMORY_H
#define RPI_MEMORY_H

typedef struct {
    long total;
    long free;
    long buffers;
    long cached;
    long swap_total;
    long swap_free;
} rpi_memory_buffer_t;

void rpi_memory_init(void);

#endif