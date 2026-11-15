// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <stdbool.h>
#include <stdint.h>

#define COLOR_WAVE_PERIOD 192
#define COLOR_WAVE_WIDTH 48
#define COLOR_WAVE_LED_SPACING 16
#define COLOR_WAVE_PULSE_SPACING 96
#define COLOR_WAVE_START_PHASE 112

typedef struct {
    uint8_t phase;
    uint8_t hue[2];
    bool second_enabled;
} color_wave_state_t;

static inline void color_wave_init(color_wave_state_t *state) {
    state->phase = COLOR_WAVE_START_PHASE;
    state->hue[0] = 0;
    state->hue[1] = 224; // first off-screen update makes the next color 32
    state->second_enabled = false;
}

static inline void color_wave_advance(color_wave_state_t *state) {
    state->phase = (uint8_t)((state->phase + 1u) % COLOR_WAVE_PERIOD);
    // Introduce the second pulse only after it has passed off-screen on startup.
    if (state->phase == 144) state->second_enabled = true;
    // Change each pulse's color only while that pulse is off the bar.
    if (state->phase == 80) state->hue[0] += 64;
    if (state->phase == 176) state->hue[1] += 64;
}

// The follower copies the leader while synchronized and resumes from that
// phase at its own speed when synchronization ends.
static inline bool color_wave_update_follower(color_wave_state_t *wave, bool *active, uint8_t *elapsed, uint8_t delay, const color_wave_state_t *leader) {
    if (leader) {
        *wave = *leader;
        *active = true;
        *elapsed = 0;
        return true;
    }
    if (!*active) {
        color_wave_init(wave);
        *active = true;
    }
    if (*elapsed <= delay) return false;
    *elapsed -= delay;
    if (*elapsed > 20) *elapsed = 0;
    color_wave_advance(wave);
    return true;
}

static inline uint8_t color_wave_intensity(const color_wave_state_t *state, uint8_t led, uint8_t *pulse) {
    uint8_t pos = (uint8_t)((state->phase + (uint16_t)led * COLOR_WAVE_LED_SPACING) % COLOR_WAVE_PERIOD);
    *pulse = pos >= COLOR_WAVE_PULSE_SPACING;
    if (*pulse) pos -= COLOR_WAVE_PULSE_SPACING;
    if (pos >= COLOR_WAVE_WIDTH || (*pulse && !state->second_enabled)) return 0;
    return (uint8_t)((pos < COLOR_WAVE_WIDTH / 2 ? pos : COLOR_WAVE_WIDTH - 1 - pos) * 255u / (COLOR_WAVE_WIDTH / 2 - 1));
}

static inline uint8_t rainbow_wave_offset(uint8_t index, uint8_t step) {
    return (uint8_t)(((uint16_t)index + step) % 224u);
}
