/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_network.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <dirent.h>

static const char *dir_name = "/sys/class/net/";

#define RX "/statistics/rx_"
#define TX "/statistics/tx_"

static double rpi_network_read_file(const char *if_name,
                                    const char *rxtx,
                                    const char *bytes_packets)
{
    FILE *f;
    int ret;
    double value;
    char path[50];
    
    strcpy(path, dir_name);
    strcat(path, if_name);
    strcat(path, rxtx);
    strcat(path, bytes_packets);

    f = fopen(path, "r");
    if (f == NULL) {
        return 1.0;
    }

    ret = fscanf(f, " %lf", &value);
    if (ret != 1) {
        return 2.0;
    }
    
    fclose(f);
    
    return value;
}

static json_t *rpi_network_get(duda_request_t *dr, const char *bytes_packets)
{
    DIR *d;
    struct dirent *de;
    json_t *object;
    json_t *rx_object;
    json_t *tx_object;
    json_t *total_object;
    double rx_value, tx_value;

    object = json->create_object();
    
    d = opendir(dir_name);
    if (d == NULL) {
        return object;
    }
    
    rx_object = json->create_object();
    json->add_to_object(object, "rx", rx_object);
    
    tx_object = json->create_object();
    json->add_to_object(object, "tx", tx_object);

    total_object = json->create_object();
    json->add_to_object(object, "total", total_object);

    for (de = readdir(d); de != NULL; de = readdir(d)) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }
        // TODO: check iface filter
        
        rx_value = rpi_network_read_file(de->d_name, RX, bytes_packets);
        tx_value = rpi_network_read_file(de->d_name, TX, bytes_packets);
        
        json->add_to_object(rx_object, de->d_name, json->create_number(rx_value));
        json->add_to_object(tx_object, de->d_name, json->create_number(tx_value));
        json->add_to_object(total_object, de->d_name, json->create_number(rx_value + tx_value));
    }
    closedir(d);
    
    return object;
}

json_t *rpi_network_get_bytes(duda_request_t *dr)
{
    return rpi_network_get(dr, "bytes");
}

json_t *rpi_network_get_packets(duda_request_t *dr)
{
    return rpi_network_get(dr, "packets");
}

/* register and initialize module */
void rpi_network_init(void)
{
    rpi_module_t *module = rpi_modules_module_init("network", NULL);
    
    rpi_modules_value_init("packets", rpi_network_get_packets, &(module->values_head.values));
    rpi_modules_value_init("bytes", rpi_network_get_bytes, &(module->values_head.values));
}