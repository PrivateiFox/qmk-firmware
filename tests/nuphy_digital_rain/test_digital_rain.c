// SPDX-License-Identifier: GPL-2.0-or-later
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define ENABLE_RGB_MATRIX_DIGITAL_RAIN
#define RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#define RGB_MATRIX_EFFECT(name)
#define MATRIX_ROWS 6
#define MATRIX_COLS 17
#define LED_HITS_TO_REMEMBER 8
static uint8_t g_rgb_frame_buffer[MATRIX_ROWS][MATRIX_COLS];
static struct { struct { uint8_t v; } hsv; } rgb_matrix_config;
typedef struct { bool init; } effect_params_t;
static unsigned writes;
static void rgb_matrix_set_color_all(uint8_t r, uint8_t g, uint8_t b) { (void)r; (void)g; (void)b; writes++; }
static void rgb_matrix_set_color(uint8_t led, uint8_t r, uint8_t g, uint8_t b) { (void)led; (void)r; (void)g; (void)b; writes++; }
static uint8_t rgb_matrix_map_row_column_to_led(uint8_t row, uint8_t col, uint8_t *led) { (void)row; (void)col; led[0] = 0; return 1; }
static uint16_t random16(void) { return 0; }
#include "../../quantum/rgb_matrix/animations/digital_rain_anim.h"
int main(void) {
    effect_params_t params = {.init = true};
    for (unsigned brightness = 0; brightness <= UINT8_MAX; brightness++) {
        rgb_matrix_config.hsv.v = brightness;
        writes = 0;
        assert(!DIGITAL_RAIN(&params));
        assert(writes > 0);
        params.init = false;
        for (unsigned frame = 0; frame < 32; frame++) assert(!DIGITAL_RAIN(&params));
        params.init = true;
    }
    return 0;
}
