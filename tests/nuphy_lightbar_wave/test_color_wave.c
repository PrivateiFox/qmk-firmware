// SPDX-License-Identifier: GPL-2.0-or-later
#include <assert.h>
#include "../../keyboards/nuphy/air75v2/ansi/lightbar_color_wave.h"
int main(void) {
    color_wave_state_t wave;
    color_wave_init(&wave);
    uint8_t pulse;
    for (unsigned i = 0; i < 6; i++) assert(color_wave_intensity(&wave, i, &pulse) == 0);
    color_wave_advance(&wave);
    assert(color_wave_intensity(&wave, 5, &pulse) > 0 && pulse == 0);
    for (unsigned i = 0; i < 5; i++) assert(color_wave_intensity(&wave, i, &pulse) == 0);
    unsigned overlaps = 0, changes[2] = {0, 0};
    for (unsigned n = 0; n < 3 * COLOR_WAVE_PERIOD; n++) {
        uint8_t before[] = {wave.hue[0], wave.hue[1]};
        color_wave_advance(&wave);
        unsigned lit[2] = {0, 0};
        for (unsigned i = 0; i < 6; i++) {
            if (color_wave_intensity(&wave, i, &pulse)) lit[pulse]++;
        }
        if (lit[0] && lit[1]) overlaps++;
        for (unsigned i = 0; i < 2; i++) {
            if (wave.hue[i] != before[i]) { assert(lit[i] == 0); changes[i]++; }
        }
    }
    assert(overlaps && changes[0] == 3 && changes[1] == 3);
    for (unsigned i = 0; i < 224; i++) assert(rainbow_wave_offset(i, 32) == (i + 32) % 224);
    return 0;
}
