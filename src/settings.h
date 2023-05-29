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

// uncomment to get "08/23/2022 02:55:02 pm" instead of "23.08.2022 14:55:02"
// #define DATE_TIME_FORMAT_US

// values in metric or imperial system?
bool IS_METRIC = true;

// OpenWeatherMap Settings
// Sign up here to get an API key: https://docs.thingpulse.com/how-tos/openweathermap-key/
const String OPEN_WEATHER_MAP_API_KEY = "";

/*
Go to https://openweathermap.org/find?q= and search for a location. Go through the
result set and select the entry closest to the actual location you want to display
data for. It'll be a URL like https://openweathermap.org/city/2657896. The number
at the end is what you assign to the constant below.
 */
const String OPEN_WEATHER_MAP_LOCATION_ID = "2657896";
const String DISPLAYED_LOCATION_NAME = "Zurich";
//String OPEN_WEATHER_MAP_LOCATION_ID = "3833367";
//String DISPLAYED_LOCATION_NAME = "Ushuaia";
//String OPEN_WEATHER_MAP_LOCATION_ID = "2147714";
//String DISPLAYED_LOCATION_NAME = "Sydney";
//String OPEN_WEATHER_MAP_LOCATION_ID = "5879400";
//String DISPLAYED_LOCATION_NAME = "Anchorage";
/*
Arabic -> ar, Bulgarian -> bg, Catalan -> ca, Czech -> cz, German -> de, Greek -> el,
English -> en, Persian (Farsi) -> fa, Finnish -> fi, French -> fr, Galician -> gl,
Croatian -> hr, Hungarian -> hu, Italian -> it, Japanese -> ja, Korean -> kr,
Latvian -> la, Lithuanian -> lt, Macedonian -> mk, Dutch -> nl, Polish -> pl,
Portuguese -> pt, Romanian -> ro, Russian -> ru, Swedish -> se, Slovak -> sk,
Slovenian -> sl, Spanish -> es, Turkish -> tr, Ukrainian -> ua, Vietnamese -> vi,
Chinese Simplified -> zh_cn, Chinese Traditional -> zh_tw.
*/
const String OPEN_WEATHER_MAP_LANGUAGE = "en";



// ****************************************************************************
// System settings - do not modify unless you understand what you are doing!
// ****************************************************************************
typedef struct RectangleDef {
  int x;
  int y;
  int width;
  int height;
} RectangleDef;

RectangleDef timeSpritePos = {0, 30, 320, 55};

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

#define UPDATE_INTERVAL_MINUTES 10

// the medium blue in the TP logo is 0x0067B0 which converts to 0x0336 in 16bit RGB565
#define TFT_TP_BLUE 0x0336

// format specifiers: https://cplusplus.com/reference/ctime/strftime/
#ifdef DATE_TIME_FORMAT_US
  int timePosX = 29;
  #define UI_DATE_FORMAT "%m/%d/%Y"
  #define UI_TIME_FORMAT "%H:%M:%S %P"
  #define UI_TIME_FORMAT_NO_SECONDS "%I:%M %P"
  #define UI_TIMESTAMP_FORMAT (UI_DATE_FORMAT + " " + UI_TIME_FORMAT)
#else
  int timePosX = 68;
  #define UI_DATE_FORMAT "%d.%m.%Y"
  #define UI_TIME_FORMAT "%H:%M:%S"
  #define UI_TIME_FORMAT_NO_SECONDS "%H:%M"
  #define UI_TIMESTAMP_FORMAT (UI_DATE_FORMAT + " " + UI_TIME_FORMAT)
#endif

#define SYSTEM_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"

#define MAX_FORECASTS 12

#define APP_NAME "ESP32 Weather Station Touch"
#define VERSION "1.0.0"
