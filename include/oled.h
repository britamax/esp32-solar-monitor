#pragma once

// ============================================================
// SOLAR MONITOR — oled.h
// OLED 128x32 — Tampilan statis, halaman dikontrol via web
// Tidak ada auto-scroll
// Default: halaman WiFi status
// ============================================================

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "ina3221.h"

// ID Halaman
#define PAGE_WIFI     0   // Status WiFi (default)
#define PAGE_INA_ALL  1   // Ringkasan semua channel
#define PAGE_CH1      2   // CH1 Panel
#define PAGE_CH2      3   // CH2 Baterai
#define PAGE_CH3      4   // CH3 Beban
#define PAGE_BMP      5   // BMP280
#define PAGE_MPU      6   // MPU6050
#define PAGE_MAX      7

class OledDisplay {
public:
  bool  _isOnline;
  int   _currentPage;   // halaman yang sedang tampil

  // Data dari sensor lain
  float _bmpTemp=0, _bmpPress=0, _bmpAlt=0;
  const char* _bmpWeather = "---";
  float _mpuAX=0, _mpuAY=0, _mpuGX=0, _mpuGZ=0, _mpuVib=0;
  const char* _mpuStatus = "---";
  bool _bmpActive = false;
  bool _mpuActive = false;

  // Info WiFi untuk halaman status
  String _wifiSSID   = "";
  String _wifiIP     = "";
  String _wifiMode   = "---";   // "WiFi", "AP", "Setup"
  bool   _wifiOk     = false;

private:
  Adafruit_SSD1306 _oled;

public:
  OledDisplay() : _oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1) {}

  bool begin() {
    _isOnline    = _oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    _currentPage = PAGE_WIFI;  // default halaman WiFi
    return _isOnline;
  }

  // Set halaman aktif (dari web setting)
  void setPage(int page) {
    if (page >= 0 && page < PAGE_MAX) _currentPage = page;
  }

  int getPage() { return _currentPage; }

  // Update info WiFi
  void setWifiInfo(String mode, String ssid, String ip, bool ok) {
    _wifiMode = mode;
    _wifiSSID = ssid;
    _wifiIP   = ip;
    _wifiOk   = ok;
  }

  void setBmpData(float t, float p, float a, const char* w) {
    _bmpTemp=t; _bmpPress=p; _bmpAlt=a; _bmpWeather=w;
    _bmpActive = true;
  }

  void setMpuData(float ax, float ay, float gx, float gz,
                  float vib, const char* status) {
    _mpuAX=ax; _mpuAY=ay; _mpuGX=gx; _mpuGZ=gz;
    _mpuVib=vib; _mpuStatus=status;
    _mpuActive = true;
  }

  // ============================================================
  // HALAMAN SISTEM — boot sequence
  // ============================================================

  void showSplash(String deviceName) {
    if (!_isOnline) return;
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);  _oled.println("** Solar Monitor **");
    _oled.setCursor(0, 11); _oled.println(deviceName);
    _oled.setCursor(0, 23); _oled.print("fw v"); _oled.println(FIRMWARE_VERSION);
    _oled.display();
    delay(1500);
  }

  // Tampil saat WiFi mencoba konek
  void showWifiConnecting(String ssid, int attempt) {
    if (!_isOnline) return;
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);  _oled.println("Konek WiFi...");
    _oled.setCursor(0, 11); _oled.println(ssid);
    _oled.setCursor(0, 23);
    int dots = (attempt % 12) + 1;
    for (int i = 0; i < dots; i++) _oled.print(".");
    _oled.display();
  }

  // Tampil saat AP Setup Mode
  void showAPMode() {
    if (!_isOnline) return;
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);  _oled.println("-- SETUP MODE --");
    _oled.setCursor(0, 11); _oled.println("WiFi: WifiMonitor-Setup");
    _oled.setCursor(0, 23); _oled.println("Buka: 192.168.4.1");
    _oled.display();
  }

  void showError(String msg) {
    if (!_isOnline) return;
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);  _oled.println("!!! ERROR !!!");
    _oled.setCursor(0, 11); _oled.println(msg);
    _oled.setCursor(0, 23); _oled.println("Cek SDA=13 SCL=12");
    _oled.display();
  }

  void showStatus(String line1, String line2, String line3) {
    if (!_isOnline) return;
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);  _oled.println(line1);
    _oled.setCursor(0, 11); _oled.println(line2);
    _oled.setCursor(0, 23); _oled.println(line3);
    _oled.display();
  }

  void showWifiConnected(String ip) {
    if (!_isOnline) return;
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);  _oled.println("WiFi Terhubung!");
    _oled.setCursor(0, 11); _oled.println("IP:");
    _oled.setCursor(0, 23); _oled.println(ip);
    _oled.display();
    delay(2000);
  }

  // ============================================================
  // HALAMAN DATA — dipanggil dari update()
  // ============================================================

  // PAGE_WIFI: Status koneksi
  // Mode: WiFi  SSID: BMW-C5
  // IP: 192.168.1.19
  // INA:OK BMP:OK MPU:OK
  void _pageWifi() {
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);

    // Baris 1: mode + status
    _oled.setCursor(0, 0);
    if (_wifiOk) {
      _oled.printf("%-6s OK  %s", _wifiMode.c_str(), _wifiSSID.substring(0,8).c_str());
    } else {
      _oled.printf("%-6s --  ---", _wifiMode.c_str());
    }

    // Baris 2: IP
    _oled.setCursor(0, 11);
    if (_wifiOk && _wifiIP.length() > 0) {
      _oled.printf("IP: %s", _wifiIP.c_str());
    } else {
      _oled.println("IP: -");
    }

    // Baris 3: status sensor
    _oled.setCursor(0, 23);
    _oled.printf("INA:%s BMP:%s MPU:%s",
      ina.isOnline ? "OK" : "--",
      _bmpActive   ? "OK" : "--",
      _mpuActive   ? "OK" : "--"
    );
    _oled.display();
  }

  // PAGE_INA_ALL: Ringkasan 3 channel
  // P 18.5V 1.10A 20.4W
  // B  4.1V+0.85A CHG 90%
  // L  4.0V 1.20A  4.8W
  void _pageInaAll(String n1, String n2, String n3) {
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);

    char l1 = n1.length() > 0 ? n1[0] : 'P';
    char l2 = n2.length() > 0 ? n2[0] : 'B';
    char l3 = n3.length() > 0 ? n3[0] : 'L';

    _oled.setCursor(0, 0);
    _oled.printf("%c%5.1fV%5.2fA%5.1fW", l1,
      ina.ch[0].voltage, ina.ch[0].current, ina.ch[0].power);

    _oled.setCursor(0, 11);
    _oled.printf("%c%5.2fV%+5.2fA %s %d%%", l2,
      ina.ch[1].voltage, ina.ch[1].current,
      ina.battStatus(), ina.getSOC());

    _oled.setCursor(0, 22);
    _oled.printf("%c%5.2fV%5.2fA%5.1fW", l3,
      ina.ch[2].voltage, ina.ch[2].current, ina.ch[2].power);

    _oled.display();
  }

  // PAGE_CH1: Panel detail
  // -Panel Surya- ON
  // V:18.50V  I: 1.10A
  // P:20.35W
  void _pageCh1(String name) {
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);
    _oled.printf("-%s- %s",
      name.substring(0,9).c_str(),
      ina.ch[0].power > 1.0f ? "ON" : "OFF");
    _oled.setCursor(0, 11);
    _oled.printf("V:%5.2fV  I:%5.2fA",
      ina.ch[0].voltage, ina.ch[0].current);
    _oled.setCursor(0, 23);
    _oled.printf("P:%6.2fW", ina.ch[0].power);
    _oled.display();
  }

  // PAGE_CH2: Baterai detail
  // -Baterai- CHARGING
  // V: 4.10V  SoC: 90%
  // I:+0.85A [########--]
  void _pageCh2(String name) {
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);
    _oled.printf("-%s- %s",
      name.substring(0,6).c_str(), ina.battStatus());
    _oled.setCursor(0, 11);
    _oled.printf("V:%5.2fV  SoC:%3d%%",
      ina.ch[1].voltage, ina.getSOC());
    _oled.setCursor(0, 23);
    // Bar SoC ringkas (8 karakter)
    int filled = map(ina.getSOC(), 0, 100, 0, 8);
    _oled.printf("I:%+5.2fA [", ina.ch[1].current);
    for (int i = 0; i < 8; i++) _oled.print(i < filled ? "#" : "-");
    _oled.print("]");
    _oled.display();
  }

  // PAGE_CH3: Beban + efisiensi
  // -Beban-
  // V: 4.05V  I: 1.20A
  // P: 4.86W Eff:83.5%
  void _pageCh3(String name) {
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);
    _oled.printf("-%s-", name.substring(0,10).c_str());
    _oled.setCursor(0, 11);
    _oled.printf("V:%5.2fV  I:%5.2fA",
      ina.ch[2].voltage, ina.ch[2].current);
    _oled.setCursor(0, 23);
    _oled.printf("P:%5.2fW Eff:%4.1f%%",
      ina.ch[2].power, ina.efficiency);
    _oled.display();
  }

  // PAGE_BMP: Lingkungan
  // -BMP280- [CLD]
  // T:28.5C  P:1013hPa
  // Alt: 45m
  void _pageBmp() {
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);
    _oled.printf("-BMP280- %s", _bmpWeather);
    _oled.setCursor(0, 11);
    _oled.printf("T:%4.1fC  P:%6.1fhPa",
      _bmpTemp, _bmpPress);
    _oled.setCursor(0, 23);
    _oled.printf("Alt:%5.1f m", _bmpAlt);
    _oled.display();
  }

  // PAGE_MPU: Getaran
  // -MPU6050- AMAN
  // AX:+0.12  AY:-0.05
  // Vib:0.02g
  void _pageMpu() {
    _oled.clearDisplay();
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(0, 0);
    _oled.printf("-MPU6050- %s", _mpuStatus);
    _oled.setCursor(0, 11);
    _oled.printf("AX:%+5.2f  AY:%+5.2f", _mpuAX, _mpuAY);
    _oled.setCursor(0, 23);
    _oled.printf("Vib: %5.3f g", _mpuVib);
    _oled.display();
  }

  // ============================================================
  // UPDATE — panggil di loop(), tampil halaman aktif saja
  // ============================================================
  void update(String ch1Name, String ch2Name, String ch3Name) {
    if (!_isOnline) return;

    switch (_currentPage) {
      case PAGE_WIFI:    _pageWifi(); break;
      case PAGE_INA_ALL: _pageInaAll(ch1Name, ch2Name, ch3Name); break;
      case PAGE_CH1:     _pageCh1(ch1Name); break;
      case PAGE_CH2:     _pageCh2(ch2Name); break;
      case PAGE_CH3:     _pageCh3(ch3Name); break;
      case PAGE_BMP:     if (_bmpActive) _pageBmp(); else _pageWifi(); break;
      case PAGE_MPU:     if (_mpuActive) _pageMpu(); else _pageWifi(); break;
      default:           _pageWifi(); break;
    }
  }
};

OledDisplay oledDisp;