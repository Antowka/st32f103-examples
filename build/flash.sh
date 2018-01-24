#!/usr/bin/env bash
#st-flash --reset write blink.bin 0x08000000

openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
-f /usr/share/openocd/scripts/target/stm32f1x.cfg \
-c "init" -c targets -c "reset init" \
-c "flash write_image erase blink.elf" \
-c "verify_image blink.elf" \
-c "reset" -c "shutdown"