// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <stdint.h>

// One 48-step pulse traverses six LEDs; a dark interval separates colors.
#define COLOR_WAVE_PERIOD 192
#define COLOR_WAVE_WIDTH 48
#define COLOR_WAVE_LED_SPACING 16
#define COLOR_WAVE_HUE_CHANGE_PHASE 80

static inline uint8_t color_wave_advance(uint8_t phase, uint8_t *hue) {
    phase = (uint8_t)((phase + 1u) % COLOR_WAVE_PERIOD);
    // At this phase all six LEDs are dark, so a new color never appears mid-pulse.
    if (phase == COLOR_WAVE_HUE_CHANGE_PHASE) *hue += 32;
    return phase;
}

static inline uint8_t color_wave_intensity(uint8_t phase, uint8_t led) {
    uint8_t pos = (uint8_t)((phase + (uint16_t)led * COLOR_WAVE_LED_SPACING) % COLOR_WAVE_PERIOD);
    if (pos >= COLOR_WAVE_WIDTH) return 0;
    // Fade in and out instead of hard edges.
    return (uint8_t)((pos < COLOR_WAVE_WIDTH / 2 ? pos : COLOR_WAVE_WIDTH - 1 - pos) * 255u / (COLOR_WAVE_WIDTH / 2 - 1));
}
