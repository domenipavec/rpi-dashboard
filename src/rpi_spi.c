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

#include "rpi_i2c.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <assert.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static int spifd[2];
static int spispeed[2];

static int get_fd(int i)
{
    if (spifd[i] == -1) {
        spifd[i] = wiringPiSPISetup(i, spispeed[i]);
    }
    return spifd[i];
}

json_t * rpi_spi_post(duda_request_t *dr, json_t *data, int parameter)
{
    unsigned char *buffer;
    int len, fd;

    fd = get_fd(parameter);
    if (fd == -1) {
        return json->create_string("SPI setup problem.");
    }

    if (data->type != cJSON_String) {
        return NULL;
    }

    len = strlen(data->valuestring);
    buffer = mem->alloc(len+1);
    gc->add(dr, buffer);
    memcpy(buffer, data->valuestring, len+1);

    // code from wiringPiSPI, need it here to change speed
    struct spi_ioc_transfer spi;
    spi.tx_buf        = (unsigned long)buffer;
    spi.rx_buf        = (unsigned long)buffer;
    spi.len           = len;
    spi.delay_usecs   = 0;
    spi.speed_hz      = spispeed[parameter];
    spi.bits_per_word = 8;

    if (ioctl (fd, SPI_IOC_MESSAGE(1), &spi) < 0) {
        return json->create_string("SPI bus write failed");
    }

    return json->create_string(buffer);
}

json_t * rpi_spi_frequency_get(duda_request_t *dr, int parameter)
{
    return json->create_number((double)spispeed[parameter]);
}

json_t * rpi_spi_frequency_post(duda_request_t *dr, json_t *data, int parameter)
{
    int fd = get_fd(parameter);
    if (fd == -1) {
        return json->create_string("SPI setup problem.");
    }

    if (data->type != cJSON_Number) {
        return NULL;
    }
    int value = data->valueint;
    if (value < 0 || value > 100000000) {
        return NULL;
    }

    // determine lower bound for speed
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &value) < 0) {
        return json->create_string("SPI speed change problem.");
    }
    
    spispeed[parameter] = value;
    
    return json->create_string("Successful!");
}

/* register and initialize module */
void rpi_spi_init(void)
{
    spifd[0] = -1;
    spispeed[0] = 500000;
    spifd[1] = -1;
    spispeed[1] = 500000;
    
    rpi_module_t *module = rpi_modules_module_init("spi", NULL, NULL);

    rpi_module_value_t *branch = rpi_modules_branch_init("%d0:1", rpi_spi_post, &(module->values_head.values));
    rpi_modules_value_init("frequency", rpi_spi_frequency_get, rpi_spi_frequency_post, &(branch->values));
}