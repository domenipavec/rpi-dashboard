/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RPI_CONFIG_H
#define RPI_CONFIG_H

typedef struct {
    /* default access permissions */
    int default_allow_flag;
    struct mk_list *default_allowed_users;
} rpi_config_t;

extern rpi_config_t rpi_config;

/* initialize rpi_config */
void rpi_config_init(void);

#endif