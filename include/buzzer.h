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
    digitalWrite(PIN_BUZZER, !BUZZER_ON);
    isEnabled = true;
    isActive  = false;
    _alarmEnd = 0;
    _onMs     = 100;
    _offMs    = 100;
  }

  // Mainkan event alarm dengan setting per-event
  // onMs = durasi beep, offMs = jeda, durSec = total detik
  void playAlarm(int onMs, int offMs, int durSec) {
    if (!isEnabled) return;
    _onMs     = onMs;
    _offMs    = offMs;
    _alarmEnd = millis() + (durSec * 1000UL);
    isActive  = true;
  }

  // Pola pendek blocking (startup, ok, warning)
  void play(BuzzPattern pattern) {
    if (!isEnabled) return;
    switch (pattern) {
      case BUZZ_STARTUP: _beepSequence(2, 100, 100); break;
      case BUZZ_OK:      _beep(80);                  break;
      case BUZZ_WARNING: _beepSequence(3, 200, 150); break;
      default: break;
    }
  }

  // Handle non-blocking di loop()
  void handle() {
    if (!isActive || !isEnabled) return;
    if (millis() >= _alarmEnd) {
      digitalWrite(PIN_BUZZER, !BUZZER_ON);
      isActive = false;
      return;
    }
    digitalWrite(PIN_BUZZER, BUZZER_ON);
    delay(_onMs);
    digitalWrite(PIN_BUZZER, !BUZZER_ON);
    delay(_offMs);
  }

  void off() {
    digitalWrite(PIN_BUZZER, !BUZZER_ON);
    isActive  = false;
    _alarmEnd = 0;
  }

  void stop() { off(); }  // alias untuk button handler

  void setEnabled(bool val) {
    isEnabled = val;
    if (!val) off();
  }

private:
  unsigned long _alarmEnd = 0;
  int _onMs  = 100;
  int _offMs = 100;

  void _beep(int durationMs) {
    digitalWrite(PIN_BUZZER, BUZZER_ON);
    delay(durationMs);
    digitalWrite(PIN_BUZZER, !BUZZER_ON);
    isActive = false;
  }

  void _beepSequence(int count, int onMs, int offMs) {
    for (int i = 0; i < count; i++) {
      digitalWrite(PIN_BUZZER, BUZZER_ON);
      delay(onMs);
      digitalWrite(PIN_BUZZER, !BUZZER_ON);
      if (i < count - 1) delay(offMs);
    }
    isActive = false;
  }
};

Buzzer buzzer;
