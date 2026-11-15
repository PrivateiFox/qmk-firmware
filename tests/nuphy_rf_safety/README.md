# NuPhy RF safety checks

Run these host-side checks from the repository root:

```sh
cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined tests/nuphy_rf_safety/test_frame.c -o /tmp/nuphy-rf-frame-test && /tmp/nuphy-rf-frame-test
cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Itests/nuphy_rf_safety/stubs tests/nuphy_rf_safety/test_queue.c keyboards/nuphy/common/wireless/rf_queue.c -o /tmp/nuphy-rf-queue-test && /tmp/nuphy-rf-queue-test
```

These exercise frame bounds/checksums and queue overflow, not the RF hardware or UART timings.
