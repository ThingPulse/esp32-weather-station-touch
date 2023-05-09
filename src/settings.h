// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#pragma once

// ****************************************************************************
// User settings
// ****************************************************************************
// WiFi
const char *SSID = "yourssid";
const char *WIFI_PWD = "yourpassw0rd";

// timezone Europe/Zurich as per https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// format specifiers: https://cplusplus.com/reference/ctime/strftime/
#define USER_DATE_FORMAT "%d.%m.%Y"
#define USER_TIMESTAMP_FORMAT "%d.%m.%Y %H:%M:%S"



// ****************************************************************************
// System settings - do not modify unless you understand what you are doing!
// ****************************************************************************
// 2: portrait, on/off switch right side -> 0/0 top left
// 3: landscape, on/off switch at the top -> 0/0 top left
#define TFT_ROTATION 2
// all other TFT_xyz flags are defined in platformio.ini as PIO build flags

// 0: portrait, on/off switch right side -> 0/0 top left
// 1: landscape, on/off switch at the top -> 0/0 top left
#define TOUCH_ROTATION 0
#define TOUCH_SENSITIVITY 40
#define TOUCH_SDA 23
#define TOUCH_SCL 22
// Initial LCD Backlight brightness
#define TFT_LED_BRIGHTNESS 200

#define TIME_SYNC_INTERVAL_HOURS 1

// the medium blue in the TP logo is 0x0067B0 which converts to 0x0336 in 16bit RGB565
#define TFT_TP_BLUE 0x0336

#define SYSTEM_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"

#define APP_NAME "ESP32 Weather Station Touch"
#define VERSION "1.0.0"
