#pragma once

// ============================================================
// SOLAR MONITOR — config.h
// Semua konstanta, pin, dan default value
// ============================================================

// --- Versi Firmware ---
#define FIRMWARE_VERSION    "2.0.0"
#define FIRMWARE_BUILD      __DATE__ " " __TIME__   // otomatis dari compiler
#define DEVICE_NAME_DEFAULT "Monitoring-Daya"

// --- Pin I2C ---
#define PIN_SDA             13
#define PIN_SCL             12

// --- Pin Buzzer ---
#define PIN_BUZZER          27
#define BUZZER_ON           LOW    // Active LOW buzzer

// --- Pin Reset Button ---
#define PIN_RESET_BTN       14    // Hubungkan ke GND saat ditekan

// --- OLED ---
#define OLED_ADDR           0x3C
#define OLED_WIDTH          128
#define OLED_HEIGHT         32

// --- INA3221 ---
#define INA_ADDR            0x40
#define INA_SHUNT_OHMS_DEFAULT  0.005f      // 5mΩ default
#define INA_SHUNT_LSB       0.00004f    // 40µV/bit
#define INA_BUS_LSB         0.008f      // 8mV/bit
#define INA_CONFIG_VAL      0x7127      // CH1+2+3 ON, avg64, 1.1ms

// --- WiFi AP Setup ---
#define AP_SSID             "WifiMonitor-Setup"
#define AP_PASSWORD         ""          // tanpa password
#define AP_IP               "192.168.4.1"
#define WIFI_TIMEOUT_MS     60000       // 60 detik coba konek
#define WIFI_RETRY_DELAY    500         // ms antar coba

// --- NVS Storage Keys ---
#define NVS_NAMESPACE       "solar"
#define NVS_WIFI_SSID       "wifi_ssid"
#define NVS_WIFI_PASS       "wifi_pass"
#define NVS_DEVICE_NAME     "dev_name"
#define NVS_MODE            "mode"      // "wifi" atau "ap"
#define NVS_CH1_NAME        "ch1_name"
#define NVS_CH2_NAME        "ch2_name"
#define NVS_CH3_NAME        "ch3_name"
#define NVS_KWH_CH1         "kwh_ch1"
#define NVS_KWH_CH2_IN      "kwh_ch2in"
#define NVS_KWH_CH2_OUT     "kwh_ch2out"
#define NVS_KWH_CH3         "kwh_ch3"
#define NVS_UPTIME          "uptime"
#define NVS_SETUP_DONE      "setup_done"

// --- BMP280 ---
#define BMP280_ADDR_1       0x76
#define BMP280_ADDR_2       0x77

// --- MPU6050 ---
#define MPU6050_ADDR        0x68
#define MPU_QUAKE_THRESHOLD 0.5f    // default threshold gempa (g)

// --- Default Nama Channel ---
#define CH1_NAME_DEFAULT    "Panel Surya"
#define CH2_NAME_DEFAULT    "Baterai"
#define CH3_NAME_DEFAULT    "Beban"

// --- Default On/Off Sensor ---
#define SENSOR_BMP_DEFAULT  true
#define SENSOR_MPU_DEFAULT  true
#define SENSOR_BUZZ_DEFAULT true

// --- OLED Halaman ---
#define OLED_PAGE_COUNT     4
#define OLED_PAGE_DURATION  3000        // ms per halaman

// --- kWh Simpan Interval ---
#define KWH_SAVE_MS         300000UL    // 5 menit (lebih sering untuk cegah data hilang saat restart)

// --- Default Threshold Alarm ---
#define ALARM_V_MIN_DEFAULT   3.0f    // tegangan minimum baterai (V)
#define ALARM_V_MAX_DEFAULT   4.2f    // tegangan maksimum baterai (V)
#define ALARM_I_MAX_DEFAULT   10.0f   // arus maksimum (A)
#define ALARM_QUAKE_DEFAULT   0.5f    // threshold gempa (g)

// --- Mode Operasi ---
#define MODE_WIFI           "wifi"
#define MODE_AP             "ap"