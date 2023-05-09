// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#pragma once

#include "time.h"
#include "settings.h"

char timestampBuffer[26];

int getCurrentMinute() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    log_e("Failed to obtain time.");
    return -1;
  }
  return timeinfo.tm_min;
}

String getCurrentTimestamp(const char* format) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    log_e("Failed to obtain time.");
    return "";
  }
  strftime(timestampBuffer, sizeof(timestampBuffer), format, &timeinfo);
  return String(timestampBuffer);
}

boolean initTime() {
  struct tm timeinfo;

  log_i("Synchronizing time.");
  // Connect to NTP server with 0 TZ offset, call setTimezone() later
  configTime(0, 0, "pool.ntp.org");
  if (!getLocalTime(&timeinfo)) {
    log_e("Failed to obtain time.");
    return false;
  }
  log_i("UTC time: %s", getCurrentTimestamp(SYSTEM_TIMESTAMP_FORMAT).c_str());
  return true;
}

void logBanner() {
  log_i("**********************************************");
  log_i("* ThingPulse Weather Station Touch v%s *", VERSION);
  log_i("**********************************************");
}

void logMemoryStats() {
  log_i("Total heap: %d", ESP.getHeapSize());
  log_i("Free heap: %d", ESP.getFreeHeap());
  log_i("Total PSRAM: %d", ESP.getPsramSize());
  log_i("Free PSRAM: %d", ESP.getFreePsram());
}

void setTimezone(const char* timezone) {
  log_i("Setting timezone to '%s'.", timezone);
  // Clock settings are adjusted to show the new local time
  setenv("TZ", timezone, 1);
  tzset();
}
