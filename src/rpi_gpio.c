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

#include "rpi_gpio.h"

#include "rpi_modules.h"
#include "rpi_websocket.h"

#include "packages/json/json.h"
#include "packages/websocket/websocket.h"

#include <wiringPi.h>
#include <softPwm.h>
#include <softTone.h>

static gpio_pin_t pins[MAX_PINS];
static int npins;
static const char *gpio_mode_str[] = {
    "undefined",
    "input",
    "output",
    "pwm",
    "tone"
};
static const char *gpio_pull_str[] = {
    "off",
    "down",
    "up"
};

static int gpio_channel;

static void init_pin(int i)
{
    pins[i].mode = GPIO_UNDEFINED;
    pins[i].pull = PUD_OFF;
    pins[i].value = 0;
    pins[i].frequency = 1000;
    pins[i].range = 100;
}

static void recalculate_hw_pwm_clock()
{
    double calc = 19200000.0;
    calc /= pins[HWPWM].frequency;
    calc /= pins[HWPWM].range;
    int div = (int)(calc+0.5);
    if (div < 2) {
        div = 2;
    }
    pwmSetClock(div);
}

static void pin_changed(int i)
{
    json_t *object;
    char key[3];
    char *response;

    sprintf(key, "%d", i);

    object = json->create_object();
    json->add_to_object(object, key, json->create_number((double)digitalRead(i)));
    response = json->print(object);

    websocket->broadcast_all((unsigned char *)response, strlen(response), WS_OPCODE_TEXT, gpio_channel);

    json->delete(object);
    mem->free(response);
}

static void isr0(void) { pin_changed(0); }
static void isr1(void) { pin_changed(1); }
static void isr2(void) { pin_changed(2); }
static void isr3(void) { pin_changed(3); }
static void isr4(void) { pin_changed(4); }
static void isr5(void) { pin_changed(5); }
static void isr6(void) { pin_changed(6); }
static void isr7(void) { pin_changed(7); }
static void isr8(void) { pin_changed(8); }
static void isr9(void) { pin_changed(9); }
static void isr10(void) { pin_changed(10); }
static void isr11(void) { pin_changed(11); }
static void isr12(void) { pin_changed(12); }
static void isr13(void) { pin_changed(13); }
static void isr14(void) { pin_changed(14); }
static void isr15(void) { pin_changed(15); }
static void isr16(void) { pin_changed(16); }
static void isr17(void) { pin_changed(17); }
static void isr18(void) { pin_changed(18); }
static void isr19(void) { pin_changed(19); }
static void isr20(void) { pin_changed(20); }

json_t * rpi_gpio_mode_get(duda_request_t *dr, int parameter)
{
    return json->create_string(gpio_mode_str[pins[parameter].mode]);
}

json_t * rpi_gpio_mode_post(duda_request_t *dr, json_t *data, int parameter)
{
    int i, mode;
    if (data->type != cJSON_String) {
        return NULL;
    }
    for (mode = -1, i = 1; i < sizeof(gpio_mode_str)/sizeof(const char *); i++) {
        if (strcmp(data->valuestring, gpio_mode_str[i]) == 0) {
            mode = i;
            break;
        }
    }
    if (mode == -1) {
        return NULL;
    }
    
    if (mode == pins[parameter].mode) {
        return json->create_string("Successful!");
    }
    if (pins[parameter].mode == GPIO_PWM && parameter != HWPWM) {
        softPwmStop(parameter);
    }
    if (pins[parameter].mode == GPIO_TONE) {
        softToneStop(parameter);
    }
    pins[parameter].mode = mode;
    
    switch (mode) {
        case GPIO_INPUT:
            pinMode(parameter, INPUT);
            pullUpDnControl(parameter, pins[parameter].pull);
            break;
        case GPIO_OUTPUT:
            pinMode(parameter, OUTPUT);
            digitalWrite(parameter, pins[parameter].value);
            break;
        case GPIO_PWM:
            if (parameter == HWPWM) {
                pinMode(parameter, PWM_OUTPUT);
                pwmSetMode(PWM_MODE_MS);
                pwmWrite(parameter, pins[parameter].value);
                pwmSetRange(pins[parameter].range);
                recalculate_hw_pwm_clock();
            } else {
                pinMode(parameter, OUTPUT);
                softPwmCreate(parameter, pins[parameter].value, pins[parameter].range);
            }
            break;
        case GPIO_TONE:
            pinMode(parameter, OUTPUT);
            softToneCreate(parameter);
            if (pins[parameter].value == 0) {
                softToneWrite(parameter, 0);
            } else {
                softToneWrite(parameter, pins[parameter].frequency);
            }
            break;
    }
    
    return json->create_string("Successful!");
}

json_t * rpi_gpio_pull_get(duda_request_t *dr, int parameter)
{
    return json->create_string(gpio_pull_str[pins[parameter].pull]);
}

json_t * rpi_gpio_pull_post(duda_request_t *dr, json_t *data, int parameter)
{
    int i, pull;
    if (data->type != cJSON_String) {
        return NULL;
    }
    for (pull = -1, i = 0; i < sizeof(gpio_pull_str)/sizeof(const char *); i++) {
        if (strcmp(data->valuestring, gpio_pull_str[i]) == 0) {
            pull = i;
            break;
        }
    }
    if (pull == -1) {
        return NULL;
    }
    
    if (pull == pins[parameter].pull) {
        return json->create_string("Successful!");
    }
    pins[parameter].pull = pull;
    
    if (pins[parameter].mode == GPIO_INPUT) {
        pullUpDnControl(parameter, pull);
    }
    
    return json->create_string("Successful!");
}

json_t * rpi_gpio_value_get(duda_request_t *dr, int parameter)
{
    if (pins[parameter].mode <= GPIO_INPUT) {
        return json->create_number((double)digitalRead(parameter));
    }
    return json->create_number((double)pins[parameter].value);
}

json_t * rpi_gpio_value_post(duda_request_t *dr, json_t *data, int parameter)
{
    if (data->type != cJSON_Number) {
        return NULL;
    }
    int value = data->valueint;
    if (value < 0 || value > pins[parameter].range) {
        return NULL;
    }
    
    if (value == pins[parameter].value) {
        return json->create_string("Successful!");
    }
    pins[parameter].value = value;
    
    switch (pins[parameter].mode) {
        case GPIO_UNDEFINED:
        case GPIO_INPUT:
            break;
        case GPIO_OUTPUT:
            digitalWrite(parameter, value);
            break;
        case GPIO_PWM:
            if (parameter == HWPWM) {
                pwmWrite(parameter, value);
            } else {
                softPwmWrite(parameter, value);
            }
            break;
        case GPIO_TONE:
            if (value == 0) {
                softToneWrite(parameter, 0);
            } else {
                softToneWrite(parameter, pins[parameter].frequency);
            }
            break;
    }
    
    return json->create_string("Successful!");
}

json_t * rpi_gpio_frequency_get(duda_request_t *dr, int parameter)
{
    return json->create_number((double)pins[parameter].frequency);
}

json_t * rpi_gpio_frequency_post(duda_request_t *dr, json_t *data, int parameter)
{
    if (data->type != cJSON_Number) {
        return NULL;
    }
    int frequency = data->valueint;
    if (frequency < 1) {
        return NULL;
    }
    
    if (frequency == pins[parameter].frequency) {
        return json->create_string("Successful!");
    }
    pins[parameter].frequency = frequency;
    
    if (pins[parameter].mode == GPIO_TONE && pins[parameter].value != 0) {
        softToneWrite(parameter, frequency);
    }
    
    if (pins[parameter].mode == GPIO_PWM && parameter == HWPWM) {
        recalculate_hw_pwm_clock();
    }
    
    return json->create_string("Successful!");
}

json_t * rpi_gpio_range_get(duda_request_t *dr, int parameter)
{
    return json->create_number((double)pins[parameter].range);
}

json_t * rpi_gpio_range_post(duda_request_t *dr, json_t *data, int parameter)
{
    if (data->type != cJSON_Number) {
        return NULL;
    }
    int range = data->valueint;
    if (range < 1) {
        return NULL;
    }
    
    if (range == pins[parameter].range) {
        return json->create_string("Successful!");
    }
    pins[parameter].range = range;
    
    if (pins[parameter].value > range) {
        pins[parameter].value = range;
    }

    if (pins[parameter].mode == GPIO_PWM) {
        if (parameter == HWPWM) {
            pwmSetRange(range);
            recalculate_hw_pwm_clock();
        } else {
            softPwmStop(parameter);
            softPwmCreate(parameter, pins[parameter].value, range);
        }
    }

    return json->create_string("Successful!");
}

json_t * rpi_gpio_pin_post(duda_request_t *dr, json_t *data, int parameter)
{
    json_t *item;
    json_t *ret;
    int success = 1;
    
    if (data->type != cJSON_Object) {
        return NULL;
    }

    item = json->get_object_item(data, "mode");
    if (item != NULL) {
        if ((ret = rpi_gpio_mode_post(dr, item, parameter)) != NULL) {
            json->delete(ret);
        } else {
            success = 0;
        }
    }

    item = json->get_object_item(data, "pull");
    if (item != NULL) {
        if ((ret = rpi_gpio_pull_post(dr, item, parameter)) != NULL) {
            json->delete(ret);
        } else {
            success = 0;
        }
    }

    item = json->get_object_item(data, "range");
    if (item != NULL) {
        if ((ret = rpi_gpio_range_post(dr, item, parameter)) != NULL) {
            json->delete(ret);
        } else {
            success = 0;
        }

    }

    item = json->get_object_item(data, "value");
    if (item != NULL) {
        if ((ret = rpi_gpio_value_post(dr, item, parameter)) != NULL) {
            json->delete(ret);
        } else {
            success = 0;
        }
    }

    item = json->get_object_item(data, "frequency");
    if (item != NULL) {
        if ((ret = rpi_gpio_frequency_post(dr, item, parameter)) != NULL) {
            json->delete(ret);
        } else {
            success = 0;
        }
    }

    if (success == 0) {
        return NULL;
    }

    return json->create_string("Successful!");
}

json_t * rpi_gpio_post(duda_request_t *dr, json_t *data, int parameter)
{
    json_t *child;
    json_t *ret = json->create_object();
    json_t *child_ret;

    for (child = data->child; child; child = child->next) {
        parameter = atoi(child->string);
        if (parameter >= 0 && parameter < npins) {
            child_ret = rpi_gpio_pin_post(dr, child, parameter);
            if (child_ret == NULL) {
                json->add_to_object(ret, child->string, json->create_string("Unsupported action or invalid parameters!"));
            } else {
                json->add_to_object(ret, child->string, child_ret);
            }
        } else {
            json->add_to_object(ret, child->string, json->create_string("Unsupported action or invalid parameters!"));
        }
    }
    
    return ret;
}

json_t * rpi_gpio_ws(duda_request_t *dr, int parameter)
{
    return rpi_websocket_handshake(dr, gpio_channel);
}

/* register and initialize module */
void rpi_gpio_init(void)
{
    int i;
    const char *valueHandle;
    
    gpio_channel = rpi_websocket_get_channel();

    wiringPiSetup();

    wiringPiISR(0, INT_EDGE_BOTH, &isr0);
    wiringPiISR(1, INT_EDGE_BOTH, &isr1);
    wiringPiISR(2, INT_EDGE_BOTH, &isr2);
    wiringPiISR(3, INT_EDGE_BOTH, &isr3);
    wiringPiISR(4, INT_EDGE_BOTH, &isr4);
    wiringPiISR(5, INT_EDGE_BOTH, &isr5);
    wiringPiISR(6, INT_EDGE_BOTH, &isr6);
    wiringPiISR(7, INT_EDGE_BOTH, &isr7);
    wiringPiISR(8, INT_EDGE_BOTH, &isr8);
    wiringPiISR(9, INT_EDGE_BOTH, &isr9);
    wiringPiISR(10, INT_EDGE_BOTH, &isr10);
    wiringPiISR(11, INT_EDGE_BOTH, &isr11);
    wiringPiISR(12, INT_EDGE_BOTH, &isr12);
    wiringPiISR(13, INT_EDGE_BOTH, &isr13);
    wiringPiISR(14, INT_EDGE_BOTH, &isr14);
    wiringPiISR(15, INT_EDGE_BOTH, &isr15);
    wiringPiISR(16, INT_EDGE_BOTH, &isr16);

    if (piBoardRev() == 1) {
        valueHandle = "%d0:16";
        npins = 17;
    } else {
        valueHandle = "%d0:20";
        npins = 21;

        wiringPiISR(17, INT_EDGE_BOTH, &isr17);
        wiringPiISR(18, INT_EDGE_BOTH, &isr18);
        wiringPiISR(19, INT_EDGE_BOTH, &isr19);
        wiringPiISR(20, INT_EDGE_BOTH, &isr20);
    }

    for (i = 0; i < npins; i++) {
        init_pin(i);
    }

    rpi_module_t *module = rpi_modules_module_init("gpio", NULL, rpi_gpio_post);

    rpi_modules_value_init("ws", rpi_gpio_ws, NULL, &(module->values_head.values));
    
    rpi_module_value_t *pins = rpi_modules_branch_init("pins", NULL, &(module->values_head.values));
    rpi_module_value_t *branch = rpi_modules_branch_init(valueHandle, rpi_gpio_pin_post, &(pins->values));

    rpi_modules_value_init("mode", rpi_gpio_mode_get, rpi_gpio_mode_post, &(branch->values));
    rpi_modules_value_init("pull", rpi_gpio_pull_get, rpi_gpio_pull_post, &(branch->values));
    rpi_modules_value_init("value", rpi_gpio_value_get, rpi_gpio_value_post, &(branch->values));
    rpi_modules_value_init("frequency", rpi_gpio_frequency_get, rpi_gpio_frequency_post, &(branch->values));
    rpi_modules_value_init("range", rpi_gpio_range_get, rpi_gpio_range_post, &(branch->values));
}