// SPDX-License-Identifier: GPL-2.0-or-later
#include <assert.h>
#include "../../keyboards/nuphy/air75v2/ansi/lightbar_color_wave.h"
int main(void) {
    uint8_t hue = 0, phase = COLOR_WAVE_START_PHASE;
    for (unsigned i = 0; i < 6; i++) assert(color_wave_intensity(phase, i) == 0);
    phase = color_wave_advance(phase, &hue);
    assert(color_wave_intensity(phase, 5) > 0);
    for (unsigned i = 0; i < 5; i++) assert(color_wave_intensity(phase, i) == 0);
    for (unsigned i = 0; i < 224; i++) assert(rainbow_wave_offset(i, 32) == (i + 32) % 224);
    phase = 0;
    unsigned dark_frames = 0, lit_frames = 0, changes = 0;
    for (unsigned n = 0; n < 2 * COLOR_WAVE_PERIOD; n++) {
        uint8_t previous_hue = hue;
        phase = color_wave_advance(phase, &hue);
        unsigned lit = 0;
        for (unsigned i = 0; i < 6; i++) lit += color_wave_intensity(phase, i) != 0;
        if (lit) lit_frames++; else dark_frames++;
        if (hue != previous_hue) { assert(lit == 0); changes++; }
    }
    assert(lit_frames > 0 && dark_frames > 0 && changes == 2);
    // Advancing time moves any fixed brightness feature toward lower LED indices.
    assert(color_wave_intensity(20, 1) == color_wave_intensity(36, 0));
    return 0;
}
