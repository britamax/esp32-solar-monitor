#pragma once

// ============================================================
// SOLAR MONITOR — session_manager.h
// Manajemen session token (RAM only, 7 hari expired)
// ============================================================

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#define SESSION_DURATION_MS  (7UL * 24 * 60 * 60 * 1000)  // 7 hari
#define SESSION_TOKEN_LEN    32
#define MAX_SESSIONS         4   // Maks sesi aktif bersamaan

struct Session {
  char token[SESSION_TOKEN_LEN + 1];
  unsigned long createdAt;   // millis()
  bool valid;
};

class SessionManager {
public:
  Session sessions[MAX_SESSIONS] = {};

  // Generate token hex random 32 karakter
  String createSession() {
    String token = "";
    for (int i = 0; i < SESSION_TOKEN_LEN; i++) {
      token += String(random(0, 16), HEX);
    }
    // Simpan di slot kosong atau slot terlama
    int slot = findOldest();
    strncpy(sessions[slot].token, token.c_str(), SESSION_TOKEN_LEN);
    sessions[slot].token[SESSION_TOKEN_LEN] = '\0';
    sessions[slot].createdAt = millis();
    sessions[slot].valid = true;
    return token;
  }

  // Validasi token dari request (cookie)
  bool isValid(AsyncWebServerRequest* request) {
    String token = extractToken(request);
    if (token.isEmpty()) return false;
    return validateToken(token);
  }

  // Hapus semua session (logout semua)
  void invalidateToken(const String& token) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
      if (sessions[i].valid && token == sessions[i].token) {
        sessions[i].valid = false;
        memset(sessions[i].token, 0, sizeof(sessions[i].token));
      }
    }
  }

  void invalidateAll() {
    for (int i = 0; i < MAX_SESSIONS; i++) {
      sessions[i].valid = false;
      memset(sessions[i].token, 0, sizeof(sessions[i].token));
    }
  }

  // Buat Set-Cookie header string
  static String makeCookieHeader(const String& token) {
    return "sm_session=" + token + "; Path=/; HttpOnly; Max-Age=604800; SameSite=Lax";
  }

  // Cookie clear
  static String clearCookieHeader() {
    return "sm_session=; Path=/; HttpOnly; Max-Age=0";
  }

  // Ekstrak token dari cookie header
  String extractToken(AsyncWebServerRequest* request) {
    if (!request->hasHeader("Cookie")) return "";
    String cookie = request->header("Cookie");
    int idx = cookie.indexOf("sm_session=");
    if (idx < 0) return "";
    int start = idx + 11;
    int end   = cookie.indexOf(';', start);
    if (end < 0) end = cookie.length();
    return cookie.substring(start, end);
  }

  bool validateToken(const String& token) {
    unsigned long now = millis();
    for (int i = 0; i < MAX_SESSIONS; i++) {
      if (!sessions[i].valid) continue;
      // Cek rollover millis
      unsigned long age = now - sessions[i].createdAt;
      if (age > SESSION_DURATION_MS) {
        sessions[i].valid = false;  // Expired
        continue;
      }
      if (token == sessions[i].token) return true;
    }
    return false;
  }

private:
  int findOldest() {
    // Cari slot kosong dulu
    for (int i = 0; i < MAX_SESSIONS; i++) {
      if (!sessions[i].valid) return i;
    }
    // Tidak ada slot kosong — hapus terlama
    int oldest = 0;
    unsigned long minTime = sessions[0].createdAt;
    for (int i = 1; i < MAX_SESSIONS; i++) {
      if (sessions[i].createdAt < minTime) {
        minTime = sessions[i].createdAt;
        oldest = i;
      }
    }
    return oldest;
  }
};

// Instance global
SessionManager sessionMgr;
