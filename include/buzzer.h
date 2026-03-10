#pragma once

// ============================================================
// SOLAR MONITOR — buzzer.h
// Driver Active Buzzer
// Berbagai pola beep untuk notifikasi berbeda
// ============================================================

#include <Arduino.h>
#include "config.h"

// Pola beep
enum BuzzPattern {
  BUZZ_OFF       = 0,
  BUZZ_STARTUP   = 1,  // 2 beep pendek — sistem siap
  BUZZ_OK        = 2,  // 1 beep pendek — konfirmasi
  BUZZ_WARNING   = 3,  // 3 beep sedang — peringatan
  BUZZ_ALARM     = 4,  // beep panjang berulang — bahaya
  BUZZ_QUAKE     = 5,  // beep cepat — gempa
  BUZZ_CUSTOM    = 6   // durasi custom dari setting
};

class Buzzer {
public:
  bool isEnabled;
  bool isActive;

  void begin() {
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    isEnabled = true;
    isActive  = false;
    _pattern  = BUZZ_OFF;
    _endTime  = 0;
  }

  // Mainkan pola beep
  void play(BuzzPattern pattern) {
    if (!isEnabled) return;
    _pattern = pattern;

    switch (pattern) {
      case BUZZ_STARTUP:
        _beepSequence(2, 100, 100);
        break;
      case BUZZ_OK:
        _beep(80);
        break;
      case BUZZ_WARNING:
        _beepSequence(3, 200, 150);
        break;
      case BUZZ_ALARM:
        _beep(1000);
        break;
      case BUZZ_QUAKE:
        _beepSequence(5, 100, 80);
        break;
      default:
        break;
    }
  }

  // Beep custom: durasi ON dan jeda (ms)
  void playCustom(int onMs, int offMs, int count) {
    if (!isEnabled) return;
    _beepSequence(count, onMs, offMs);
  }

  // Nyalakan terus (untuk alarm aktif)
  void on() {
    if (!isEnabled) return;
    digitalWrite(PIN_BUZZER, HIGH);
    isActive = true;
  }

  void off() {
    digitalWrite(PIN_BUZZER, LOW);
    isActive = false;
  }

  void setEnabled(bool val) {
    isEnabled = val;
    if (!val) off();
  }

private:
  BuzzPattern _pattern;
  unsigned long _endTime;

  void _beep(int durationMs) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(durationMs);
    digitalWrite(PIN_BUZZER, LOW);
    isActive = false;
  }

  void _beepSequence(int count, int onMs, int offMs) {
    for (int i = 0; i < count; i++) {
      digitalWrite(PIN_BUZZER, HIGH);
      delay(onMs);
      digitalWrite(PIN_BUZZER, LOW);
      if (i < count - 1) delay(offMs);
    }
    isActive = false;
  }
};

Buzzer buzzer;
