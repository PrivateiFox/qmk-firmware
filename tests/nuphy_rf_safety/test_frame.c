// SPDX-License-Identifier: GPL-2.0-or-later
#include <assert.h>
#include "../../keyboards/nuphy/common/wireless/rf_frame.h"
int main(void) {
    uint8_t frame[16] = {0x5a, 0x01, 0xa0, 3, 1, 2, 3, 6};
    assert(nuphy_rf_validate_frame(frame, 3, sizeof(frame)) == NUPHY_RF_FRAME_ACK);
    frame[2] = 0;
    assert(nuphy_rf_validate_frame(frame, 3, sizeof(frame)) == NUPHY_RF_FRAME_INVALID);
    assert(nuphy_rf_validate_frame(frame, 4, sizeof(frame)) == NUPHY_RF_FRAME_INVALID);
    assert(nuphy_rf_validate_frame(frame, 7, sizeof(frame)) == NUPHY_RF_FRAME_INVALID);
    assert(nuphy_rf_validate_frame(frame, 8, sizeof(frame)) == NUPHY_RF_FRAME_DATA);
    assert(nuphy_rf_validate_frame(frame, 9, sizeof(frame)) == NUPHY_RF_FRAME_DATA);
    frame[7] = 7;
    assert(nuphy_rf_validate_frame(frame, 8, sizeof(frame)) == NUPHY_RF_FRAME_BAD_CHECKSUM);
    frame[3] = 12;
    assert(nuphy_rf_validate_frame(frame, sizeof(frame), sizeof(frame)) == NUPHY_RF_FRAME_INVALID);
    return 0;
}
