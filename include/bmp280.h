#pragma once

// ============================================================
// SOLAR MONITOR — bmp280.h
// Driver BMP280 via register I2C langsung
// Tidak perlu library Adafruit
// Suhu, Tekanan, Ketinggian, Perkiraan Cuaca
// ============================================================

#include <Wire.h>
#include "config.h"

// BMP280 Register
#define BMP_REG_ID          0xD0
#define BMP_REG_RESET       0xE0
#define BMP_REG_STATUS      0xF3
#define BMP_REG_CTRL_MEAS   0xF4
#define BMP_REG_CONFIG      0xF5
#define BMP_REG_PRESS_MSB   0xF7
#define BMP_REG_TEMP_MSB    0xFA
#define BMP_REG_CALIB_START 0x88

// BMP280 Config
// osrs_t=x2, osrs_p=x16, mode=normal
#define BMP_CTRL_VAL        0x57
// t_sb=500ms, filter=x16
#define BMP_CONFIG_VAL      0x90

class BMP280Sensor {
public:
  float temperature;  // °C
  float pressure;     // hPa
  float altitude;     // meter
  bool  isOnline;

  // Kalibasi data
  uint16_t dig_T1;
  int16_t  dig_T2, dig_T3;
  uint16_t dig_P1;
  int16_t  dig_P2, dig_P3, dig_P4, dig_P5;
  int16_t  dig_P6, dig_P7, dig_P8, dig_P9;
  int32_t  t_fine;

  void begin() {
    isOnline = false;

    // Coba alamat 0x76 dulu, lalu 0x77
    _addr = BMP280_ADDR_1;
    Wire.beginTransmission(_addr);
    if (Wire.endTransmission() != 0) {
      _addr = BMP280_ADDR_2;
      Wire.beginTransmission(_addr);
      if (Wire.endTransmission() != 0) return;
    }

    // Cek chip ID (harus 0x60 untuk BMP280)
    uint8_t id = _readByte(BMP_REG_ID);
    if (id != 0x60 && id != 0x58) return;

    // Baca kalibrasi
    _readCalibration();

    // Konfigurasi sensor
    _writeByte(BMP_REG_CTRL_MEAS, BMP_CTRL_VAL);
    _writeByte(BMP_REG_CONFIG, BMP_CONFIG_VAL);
    delay(100);

    isOnline = true;
  }

  void read() {
    if (!isOnline) return;

    // Baca raw data (6 byte: press MSB/LSB/XLSB, temp MSB/LSB/XLSB)
    Wire.beginTransmission(_addr);
    Wire.write(BMP_REG_PRESS_MSB);
    Wire.endTransmission(false);
    Wire.requestFrom(_addr, (uint8_t)6);

    uint32_t pRaw = ((uint32_t)Wire.read() << 12) |
                    ((uint32_t)Wire.read() << 4)  |
                    ((uint32_t)Wire.read() >> 4);
    uint32_t tRaw = ((uint32_t)Wire.read() << 12) |
                    ((uint32_t)Wire.read() << 4)  |
                    ((uint32_t)Wire.read() >> 4);

    temperature = _compensateTemp(tRaw);
    pressure    = _compensatePress(pRaw) / 100.0f; // Pa → hPa
    altitude    = 44330.0f * (1.0f - pow(pressure / 1013.25f, 0.1903f));
  }

  // Perkiraan cuaca berdasarkan tekanan
  const char* weatherEstimate() {
    if      (pressure >= 1022.0f) return "Cerah";
    else if (pressure >= 1013.0f) return "Berawan";
    else if (pressure >= 1000.0f) return "Mendung";
    else if (pressure >= 990.0f)  return "Hujan";
    else                           return "Badai";
  }

  // Ikon cuaca sederhana untuk OLED
  const char* weatherIcon() {
    if      (pressure >= 1022.0f) return "[SUN]";
    else if (pressure >= 1013.0f) return "[CLD]";
    else if (pressure >= 1000.0f) return "[OVC]";
    else if (pressure >= 990.0f)  return "[RAN]";
    else                           return "[STM]";
  }

private:
  uint8_t _addr = BMP280_ADDR_1;

  uint8_t _readByte(uint8_t reg) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(_addr, (uint8_t)1);
    return Wire.read();
  }

  void _writeByte(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
  }

  uint16_t _readU16LE(uint8_t reg) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(_addr, (uint8_t)2);
    return Wire.read() | ((uint16_t)Wire.read() << 8);
  }

  int16_t _readS16LE(uint8_t reg) {
    return (int16_t)_readU16LE(reg);
  }

  void _readCalibration() {
    dig_T1 = _readU16LE(0x88); dig_T2 = _readS16LE(0x8A); dig_T3 = _readS16LE(0x8C);
    dig_P1 = _readU16LE(0x8E); dig_P2 = _readS16LE(0x90); dig_P3 = _readS16LE(0x92);
    dig_P4 = _readS16LE(0x94); dig_P5 = _readS16LE(0x96); dig_P6 = _readS16LE(0x98);
    dig_P7 = _readS16LE(0x9A); dig_P8 = _readS16LE(0x9C); dig_P9 = _readS16LE(0x9E);
  }

  float _compensateTemp(uint32_t raw) {
    int32_t var1 = ((((raw >> 3) - ((int32_t)dig_T1 << 1)))
                   * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((raw >> 4) - ((int32_t)dig_T1))
                   * ((raw >> 4) - ((int32_t)dig_T1))) >> 12)
                   * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    return (float)((t_fine * 5 + 128) >> 8) / 100.0f;
  }

  float _compensatePress(uint32_t raw) {
    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)dig_P6;
    var2 += ((var1 * (int64_t)dig_P5) << 17);
    var2 += (((int64_t)dig_P4) << 35);
    var1  = ((var1 * var1 * (int64_t)dig_P3) >> 8)
           + ((var1 * (int64_t)dig_P2) << 12);
    var1  = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0) return 0;
    int64_t p = 1048576 - raw;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    return (float)p / 256.0f;
  }
};

BMP280Sensor bmp;
