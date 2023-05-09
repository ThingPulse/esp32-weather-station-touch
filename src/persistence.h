// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

#pragma once

#include <LittleFS.h>

void listFiles();

void initFileSystem() {
  if (LittleFS.begin()) {
    log_i("Flash FS available!");
  } else {
    log_e("Flash FS initialisation failed!");
  }

  listFiles();
}

void listFiles() {
  log_i("Flash FS files found:");

  File root = LittleFS.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break;
    }
    log_i("- %s, %d bytes", entry.name(), entry.size());
    entry.close();
  }
}
