#pragma once

// ============================================================
// SOLAR MONITOR — ina3221.h
// Baca data INA3221 via register I2C langsung
// Shunt 5mΩ, 3 channel
// ============================================================

#include <Wire.h>
#include "config.h"

// Register INA3221
#define INA_REG_CONFIG      0x00
#define INA_REG_CH1_SHUNT   0x01
#define INA_REG_CH1_BUS     0x02
#define INA_REG_MANUF_ID    0xFE

struct ChannelData {
  float voltage;        // Volt
  float current;        // Ampere (+ atau -)
  float power;          // Watt
};

class INA3221 {
public:
  ChannelData ch[3];    // ch[0]=CH1, ch[1]=CH2, ch[2]=CH3
  float efficiency;     // Persen
  bool  isOnline;

  // kWh akumulasi (dimuat dari NVS saat begin)
  float kwh[3];         // ch1, ch3 satu arah
  float kwhCh2In;       // kWh masuk baterai
  float kwhCh2Out;      // kWh keluar baterai

  void begin() {
    Wire.beginTransmission(INA_ADDR);
    isOnline = (Wire.endTransmission() == 0);
    if (isOnline) {
      _writeReg(INA_REG_CONFIG, INA_CONFIG_VAL);
      delay(200);
    }
  }

  void read() {
    if (!isOnline) return;
    for (int c = 0; c < 3; c++) {
      int16_t rawV = (int16_t)_readReg(INA_REG_CH1_BUS   + c * 2);
      int16_t rawI = (int16_t)_readReg(INA_REG_CH1_SHUNT + c * 2);
      ch[c].voltage = (rawV >> 3) * INA_BUS_LSB;
      ch[c].current = ((rawI >> 3) * INA_SHUNT_LSB) / INA_SHUNT_OHMS;
      ch[c].power   = ch[c].voltage * ch[c].current;
    }
    // Efisiensi
    float useful = (ch[1].current > 0 ? ch[1].power : 0) + ch[2].power;
    efficiency   = (ch[0].power > 0.5f)
                   ? min(useful / ch[0].power * 100.0f, 100.0f)
                   : 0.0f;
  }

  // Akumulasi kWh — panggil tiap interval
  // intervalSec = selang waktu sejak pemanggilan terakhir (detik)
  void accumulateKwh(float intervalSec) {
    // CH1 Panel
    if (ch[0].power > 0) kwh[0] += (ch[0].power * intervalSec) / 3600.0f;
    // CH2 Baterai — pisah masuk & keluar
    if (ch[1].current > 0.05f)  kwhCh2In  += (ch[1].power * intervalSec) / 3600.0f;
    if (ch[1].current < -0.05f) kwhCh2Out += (abs(ch[1].power) * intervalSec) / 3600.0f;
    // CH3 Beban
    if (ch[2].power > 0) kwh[2] += (ch[2].power * intervalSec) / 3600.0f;
  }

  void loadKwh(float ch1, float ch2in, float ch2out, float ch3) {
    kwh[0]    = ch1;
    kwhCh2In  = ch2in;
    kwhCh2Out = ch2out;
    kwh[2]    = ch3;
  }

  void resetKwh() {
    kwh[0] = kwh[2] = kwhCh2In = kwhCh2Out = 0.0f;
  }

  // Status baterai CH2
  const char* battStatus() {
    if      (ch[1].current >  0.05f) return "CHARGING";
    else if (ch[1].current < -0.05f) return "DISCHARG";
    else                              return "IDLE";
  }

  // State of Charge estimasi dari tegangan
  int getSOC() {
    float v = ch[1].voltage;
    if      (v >= 4.20f) return 100;
    else if (v >= 4.10f) return 90;
    else if (v >= 4.00f) return 75;
    else if (v >= 3.90f) return 60;
    else if (v >= 3.80f) return 45;
    else if (v >= 3.75f) return 30;
    else if (v >= 3.70f) return 20;
    else if (v >= 3.65f) return 10;
    else                  return 5;
  }

  String socBar() {
    int filled = map(getSOC(), 0, 100, 0, 12);
    String bar = "[";
    for (int x = 0; x < 12; x++) bar += (x < filled) ? "#" : "-";
    bar += "]";
    return bar;
  }

private:
  uint16_t _readReg(uint8_t reg) {
    Wire.beginTransmission(INA_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)INA_ADDR, (uint8_t)2);
    return ((uint16_t)Wire.read() << 8) | Wire.read();
  }

  void _writeReg(uint8_t reg, uint16_t val) {
    Wire.beginTransmission(INA_ADDR);
    Wire.write(reg);
    Wire.write((val >> 8) & 0xFF);
    Wire.write(val & 0xFF);
    Wire.endTransmission();
  }
};

INA3221 ina;