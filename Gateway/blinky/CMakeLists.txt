#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lte-ble-gateway)

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
# NORDIC SDK APP END
zephyr_include_directories(src)

include_directories(src/orientation_detector)
target_sources(app PRIVATE src/aggregator.c)
target_sources(app PRIVATE src/alarm.c)
target_sources(app PRIVATE src/ble.c)
target_sources(app PRIVATE src/low_power.c)
target_sources(app PRIVATE src/button.c)
target_sources(app PRIVATE src/services.c)
target_sources(app PRIVATE src/fs.c)
target_sources(app PRIVATE src/listing.c)
target_sources(app PRIVATE src/clock.c)
target_sources(app PRIVATE src/led.c)
target_sources(app PRIVATE src/lte.c)
target_sources(app PRIVATE src/battery.c)
target_sources(app PRIVATE src/buzzer.c)
# target_sources(app PRIVATE src/modemSetup.h)
# set(spm_CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/spm.conf)
