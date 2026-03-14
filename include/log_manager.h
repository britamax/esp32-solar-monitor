#pragma once

// ============================================================
// SOLAR MONITOR — log_manager.h
// Circular log buffer 50 baris — akses via /log
// Kategori: kWh, MQTT, Sensor, WiFi, SYS
// ============================================================

#include <Arduino.h>

#define LOG_MAX_ENTRIES  50
#define LOG_MSG_LEN      96

// Kategori log
#define LOG_KWH    "kWh"
#define LOG_MQTT   "MQTT"
#define LOG_SENSOR "SENSOR"
#define LOG_WIFI   "WiFi"
#define LOG_SYS    "SYS"

struct LogEntry {
  char     time[20];    // "DD Mon YYYY HH:MM:SS" atau uptime
  char     cat[8];      // kategori
  char     msg[LOG_MSG_LEN];
  bool     valid;
};

class LogManager {
public:
  LogEntry entries[LOG_MAX_ENTRIES] = {};
  int      head  = 0;   // index tulis berikutnya
  int      count = 0;   // total entry valid

  // Tambah log entry
  void add(const char* cat, const char* msg, const char* timeStr = nullptr) {
    LogEntry& e = entries[head];
    e.valid = true;

    // Timestamp
    if (timeStr && strlen(timeStr) > 0) {
      strncpy(e.time, timeStr, sizeof(e.time)-1);
      e.time[sizeof(e.time)-1] = '\0';
    } else {
      // Fallback: uptime dalam detik
      unsigned long s = millis() / 1000;
      snprintf(e.time, sizeof(e.time), "+%lus", s);
    }

    strncpy(e.cat, cat, sizeof(e.cat)-1);
    e.cat[sizeof(e.cat)-1] = '\0';

    strncpy(e.msg, msg, sizeof(e.msg)-1);
    e.msg[sizeof(e.msg)-1] = '\0';

    // Serial output juga
    Serial.printf("[LOG][%s] %s\n", cat, msg);

    // Maju circular buffer
    head = (head + 1) % LOG_MAX_ENTRIES;
    if (count < LOG_MAX_ENTRIES) count++;
  }

  // Shorthand dengan format
  void addf(const char* cat, const char* timeStr, const char* fmt, ...) {
    char buf[LOG_MSG_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    add(cat, buf, timeStr);
  }

  // Hapus semua log
  void clear() {
    for (int i = 0; i < LOG_MAX_ENTRIES; i++) entries[i].valid = false;
    head = 0; count = 0;
  }

  // Serialize ke JSON string untuk /api/log
  // Urutan: terbaru dulu
  String toJson(const char* filterCat = nullptr) {
    String out = "[";
    bool first = true;

    // Iterate dari entry terbaru ke terlama
    for (int i = 0; i < LOG_MAX_ENTRIES; i++) {
      // Index dari terbaru
      int idx = (head - 1 - i + LOG_MAX_ENTRIES) % LOG_MAX_ENTRIES;
      LogEntry& e = entries[idx];
      if (!e.valid) continue;

      // Filter kategori
      if (filterCat && strlen(filterCat) > 0 && strcmp(filterCat, "all") != 0) {
        if (strcmp(e.cat, filterCat) != 0) continue;
      }

      if (!first) out += ",";
      first = false;

      // Escape msg
      String msg = e.msg;
      msg.replace("\\", "\\\\");
      msg.replace("\"", "\\\"");

      out += "{\"t\":\"" + String(e.time) + "\","
             "\"c\":\"" + String(e.cat)  + "\","
             "\"m\":\"" + msg + "\"}";
    }
    out += "]";
    return out;
  }
};

LogManager logger;
