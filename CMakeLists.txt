# SPDX-License-Identifier: Apache-2.0

set(BOARD olimex_lora_stm32wl_devkit)

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(bat-monitor)

target_sources(app PRIVATE src/read_adc.c)
target_sources(app PRIVATE src/main.c)
