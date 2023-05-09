// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#pragma once

#include <WiFi.h>

#include "settings.h"

void startWiFi() {
  WiFi.begin(SSID, WIFI_PWD);
  log_i("Connecting to WiFi '%s'...", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    log_i(".");
    delay(200);
  }
  log_i("...done. IP: %s, WiFi RSSI: %d.", WiFi.localIP().toString().c_str(), WiFi.RSSI());
}
