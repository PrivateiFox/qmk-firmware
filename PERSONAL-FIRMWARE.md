# Personal NuPhy OpenRGB firmware

This fork is based on ryodeushii/qmk-firmware commit `5ffb425f73d20caf1cad5093c5e7f23c5c08fb0c` (branch `nuphy-keyboards`). Upstream: https://github.com/ryodeushii/qmk-firmware

The `modules/openrgb` files are based on OpenRGB's QMK community module revision `2e27589c74a14e3ff79ffa7adeedab0a591813e3`: https://gitlab.com/OpenRGBDevelopers/QMK-OpenRGB. Its endpoint patch is applied explicitly to `tmk_core/protocol/usb_descriptor.h`. The module is modified to allow limiting the LEDs exposed to OpenRGB.

Build targets: `nuphy/air75v2/ansi:openrgb` and `nuphy/halo96v2/ansi:openrgb`. Both use QMK OpenRGB protocol revision D and disable VIA. The Halo exposes only 99 key LEDs; its ring and indicators remain under the keyboard's control. Mac layers use F1–F12 by default. The Air has independently controlled side bars and a custom rainbow wave. Saved keyboard settings can override lighting defaults until configuration is reset.

Initialize the pinned QMK submodules (`git submodule update --init --recursive`), install QMK's build requirements and toolchain, then build with `qmk compile -kb nuphy/air75v2/ansi -km openrgb` (substitute halo96v2 for the other target). Flash only the matching keyboard via USB and keep recovery firmware available. These customizations have limited hardware testing; verify lighting and wireless behavior after changes.

Firmware and dependencies retain their original licenses and copyright notices.

RF frame validation, bounded receive processing, and queue recovery were compared with `ablasik/nuphy-qmk` commit `67d0c8aa2e89019b49a4acdf7638d7845cef9058`. Halo ring rendering order was compared with `ArxEllie/qmk_firmware` commit `297e25345407ccc96e448387f2f91068e0d85292` (Halo75 V2). Changes were adapted to this fork; hardware behavior still needs verification.

The Air75 OpenRGB keymap enables link-time optimization to reduce static RAM usage; this is an experimental build until tested on hardware. On the local ARM GCC 15.3 build, the linker-reported heap region grew from 152 to 472 bytes before runtime allocations. The LED DMA buffer, EEPROM layout, wireless queue size, and Halo build are unchanged. The working pre-optimization source is commit `610f3b815709c954cf43004a271465b166262043`.
