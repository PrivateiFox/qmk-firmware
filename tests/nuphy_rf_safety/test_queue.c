// SPDX-License-Identifier: GPL-2.0-or-later
#include <assert.h>
#include "../../keyboards/nuphy/common/wireless/rf_queue.h"
extern const rf_queue_t rf_queue;
int main(void) {
    report_buffer_t report = {.cmd = 1};
    for (unsigned i = 0; i < 63; ++i) {
        report.buffer[0] = i;
        assert(rf_queue.enqueue(&report));
    }
    report.buffer[0] = 0xff; // latest state, e.g. a key release
    assert(rf_queue.enqueue(&report));
    assert(rf_queue.dequeue(&report) && report.buffer[0] == 1);
    for (unsigned i = 0; i < 61; ++i) assert(rf_queue.dequeue(&report));
    assert(rf_queue.dequeue(&report) && report.buffer[0] == 0xff);
    assert(rf_queue.is_empty());
    return 0;
}
