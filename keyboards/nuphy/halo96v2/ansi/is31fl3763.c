/* Copyright 2017 Jason Williams
 * Copyright 2018 Jack Humbert
 * Copyright 2018 Yiancar
 * Copyright 2021 Doni Crosby
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "is31fl3763.h"

#include <string.h>

#include "config.h"
#include "gpio.h"
#include "i2c_master.h"
#include "wait.h"

#define ISSI_ADDR_DEFAULT 0x6C

#define ISSI_COMMANDREGISTER 0xFE
#define ISSI_COMMANDREGISTER_WRITELOCK 0xFF
#define ISSI_INTERRUPTMASKREGISTER 0xF0
#define ISSI_INTERRUPTSTATUSREGISTER 0xF1

#define ISSI_PAGE_PWM 0x00         // PG0
#define ISSI_PAGE_PWM_CONTROL 0x01 // PG1
#define ISSI_PWM_CONFIGURATION 0x60
#define ISSI_PWM_CONFIGURATION_SSD 0x01
#define ISSI_PWM_CONFIGURATION_BASE 0x08

#define ISSI_PAGE_AUTOBREATH 0x02 // PG2
#define ISSI_PAGE_FUNCTION 0x03   // PG3

#define ISSI_REG_CONFIGURATION 0x00 // PG3
#define ISSI_REG_GLOBALCURRENT 0x01 // PG3
#define ISSI_REG_RESET 0x11         // PG3
#define ISSI_REG_SWPULLUP 0x0F      // PG3
#define ISSI_REG_CSPULLUP 0x10      // PG3

#ifndef ISSI_TIMEOUT
#    define ISSI_TIMEOUT 100
#endif

#ifndef ISSI_PERSISTENCE
#    define ISSI_PERSISTENCE 0
#endif

#ifndef ISSI_PWM_FREQUENCY
#    define ISSI_PWM_FREQUENCY 0b000
#endif

#ifndef ISSI_SWPULLUP
#    define ISSI_SWPULLUP PUR_0R
#endif

#ifndef ISSI_CSPULLUP
#    define ISSI_CSPULLUP PUR_0R
#endif

#ifndef ISSI_GLOBALCURRENT
#    define ISSI_GLOBALCURRENT 0x80
#endif

const uint8_t i2c_addresses[DRIVER_COUNT] = {
    IS31FL3763_I2C_ADDRESS_1,
    IS31FL3763_I2C_ADDRESS_2,
};

uint8_t g_twi_transfer_buffer[20];

uint8_t g_pwm_buffer[DRIVER_COUNT][328];
bool    g_pwm_buffer_update_required[DRIVER_COUNT] = {false};

uint8_t g_led_control_registers[DRIVER_COUNT][24]             = {0};
bool    g_led_control_registers_update_required[DRIVER_COUNT] = {false};

void is31fl3763_write_register(uint8_t index, uint8_t reg, uint8_t data) {
#if ISSI_PERSISTENCE > 0
    for (uint8_t i = 0; i < ISSI_PERSISTENCE; i++) {
        if (i2c_write_register(i2c_addresses[index] << 1, reg, &data, 1, ISSI_TIMEOUT) != 0) {
            return;
        }
    }
#else
    i2c_write_register(i2c_addresses[index] << 1, reg, &data, 1, ISSI_TIMEOUT);
#endif
}

void is31fl3763_write_pwm_buffer(uint8_t index) {
    uint16_t addr_sart = 0;

    for (int i = 0; i < 18; i += 1) {
        if (i >= 14) {
            is31fl3763_write_register(index, ISSI_COMMANDREGISTER_WRITELOCK, 0xC5);
            is31fl3763_write_register(index, ISSI_COMMANDREGISTER, ISSI_PAGE_PWM_CONTROL);

            addr_sart = ((i - 14) * 18);

            i2c_write_register(i2c_addresses[index] << 1, addr_sart, &g_pwm_buffer[index][(addr_sart | 0X100)], 18, ISSI_TIMEOUT);

        } else {
            is31fl3763_write_register(index, ISSI_COMMANDREGISTER_WRITELOCK, 0xC5);
            is31fl3763_write_register(index, ISSI_COMMANDREGISTER, ISSI_PAGE_PWM);

            addr_sart = (i * 18);

            i2c_write_register(i2c_addresses[index] << 1, addr_sart, &g_pwm_buffer[index][addr_sart], 18, ISSI_TIMEOUT);
        }
    }

    is31fl3763_write_register(index, 0xFD, 0x00);
}

void is31fl3763_init_drivers(void) {
    i2c_init();

    for (uint8_t i = 0; i < DRIVER_COUNT; i++) {
        is31fl3763_init(i);
    }

    for (int i = 0; i < DRIVER_COUNT; i++) {
        is31fl3763_set_led_control_register(i, true, true, true);
    }

    for (uint8_t i = 0; i < DRIVER_COUNT; i++) {
        is31fl3763_update_led_control_registers(i);
        memset(g_pwm_buffer[i], 0, sizeof(g_pwm_buffer[i]));
        is31fl3763_write_pwm_buffer(i);
        g_pwm_buffer_update_required[i] = false;
        is31fl3763_write_register(i, ISSI_COMMANDREGISTER_WRITELOCK, 0xC5);
        is31fl3763_write_register(i, ISSI_COMMANDREGISTER, ISSI_PAGE_PWM_CONTROL);
        is31fl3763_write_register(i, ISSI_PWM_CONFIGURATION, ISSI_PWM_CONFIGURATION_BASE | ISSI_PWM_CONFIGURATION_SSD);
    }
}

void is31fl3763_init(uint8_t index) {
    is31fl3763_write_register(index, ISSI_COMMANDREGISTER_WRITELOCK, 0xC5);
    is31fl3763_write_register(index, ISSI_COMMANDREGISTER, ISSI_PAGE_PWM_CONTROL);
    is31fl3763_write_register(index, ISSI_PWM_CONFIGURATION, ISSI_PWM_CONFIGURATION_BASE);

    is31fl3763_write_register(index, 0x62, 0x66);
    is31fl3763_write_register(index, 0x61, ISSI_GLOBALCURRENT);

    is31fl3763_write_register(index, 0x63, 0x00);

    for (uint8_t sl_set = 0x49; sl_set <= 0x5A; sl_set++) {
        is31fl3763_write_register(index, sl_set, 0x80);
    }
    is31fl3763_write_register(index, 0xFD, 0x00);

    wait_ms(10);
}

void is31fl3763_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    is31_led led;

    if (index >= 0 && index < RGB_MATRIX_LED_COUNT) {
        memcpy_P(&led, &g_is31_leds[index], sizeof(led));

        g_pwm_buffer[led.driver][led.r]          = red;
        g_pwm_buffer[led.driver][led.g]          = green;
        g_pwm_buffer[led.driver][led.b]          = blue;
        g_pwm_buffer_update_required[led.driver] = true;
    }
}

void is31fl3763_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        is31fl3763_set_color(i, red, green, blue);
    }
}

void is31fl3763_set_led_control_register(uint8_t index, bool red, bool green, bool blue) {
    is31_led led;
    memcpy_P(&led, &g_is31_leds[index], sizeof(led));

    uint8_t control_register_r = led.r / 8;
    uint8_t control_register_g = led.g / 8;
    uint8_t control_register_b = led.b / 8;
    uint8_t bit_r              = led.r % 8;
    uint8_t bit_g              = led.g % 8;
    uint8_t bit_b              = led.b % 8;

    if (red) {
        g_led_control_registers[led.driver][control_register_r] |= (1 << bit_r);
    } else {
        g_led_control_registers[led.driver][control_register_r] &= ~(1 << bit_r);
    }
    if (green) {
        g_led_control_registers[led.driver][control_register_g] |= (1 << bit_g);
    } else {
        g_led_control_registers[led.driver][control_register_g] &= ~(1 << bit_g);
    }
    if (blue) {
        g_led_control_registers[led.driver][control_register_b] |= (1 << bit_b);
    } else {
        g_led_control_registers[led.driver][control_register_b] &= ~(1 << bit_b);
    }

    g_led_control_registers_update_required[led.driver] = true;
}

void is31fl3763_update_pwm_buffers(uint8_t index) {
    if (g_pwm_buffer_update_required[index]) {
        is31fl3763_write_register(index, ISSI_COMMANDREGISTER_WRITELOCK, 0xC5);
        is31fl3763_write_register(index, ISSI_COMMANDREGISTER, ISSI_PAGE_PWM);

        is31fl3763_write_pwm_buffer(index);

        g_pwm_buffer_update_required[index] = false;
    }
}

void is31fl3763_update_led_control_registers(uint8_t index) {
    if (g_led_control_registers_update_required[index]) {
        is31fl3763_write_register(index, ISSI_COMMANDREGISTER_WRITELOCK, 0xC5);
        is31fl3763_write_register(index, ISSI_COMMANDREGISTER, 0x01);
        for (int i = 0; i < 24; i++) {
            is31fl3763_write_register(index, i, g_led_control_registers[index][i]);
        }
    }
    g_led_control_registers_update_required[index] = false;
}

void is31fl3763_flush(void) {
    for (uint8_t i = 0; i < DRIVER_COUNT; i++) {
        is31fl3763_update_pwm_buffers(i);
    }
}
