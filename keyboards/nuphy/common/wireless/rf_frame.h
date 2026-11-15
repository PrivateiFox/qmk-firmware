// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <stdint.h>

typedef enum { NUPHY_RF_FRAME_INVALID, NUPHY_RF_FRAME_BAD_CHECKSUM, NUPHY_RF_FRAME_ACK, NUPHY_RF_FRAME_DATA } nuphy_rf_frame_result_t;

static inline nuphy_rf_frame_result_t nuphy_rf_validate_frame(const uint8_t *frame, uint16_t received, uint16_t capacity) {
    if (received == 3) return frame[2] == 0xA0 ? NUPHY_RF_FRAME_ACK : NUPHY_RF_FRAME_INVALID;
    if (received < 5) return NUPHY_RF_FRAME_INVALID;
    uint16_t length = frame[3];
    if (length + 5 > capacity || length + 5 > received) return NUPHY_RF_FRAME_INVALID;
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < length; i++) checksum += frame[4 + i];
    return checksum == frame[4 + length] ? NUPHY_RF_FRAME_DATA : NUPHY_RF_FRAME_BAD_CHECKSUM;
}
