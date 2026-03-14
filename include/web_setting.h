#pragma once

// ============================================================
// SOLAR MONITOR — web_setting.h
// Halaman pengaturan via browser
// Akses: http://[IP]/setting
// ============================================================

const char SETTING_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Solar Monitor - Setting</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',Arial,sans-serif;background:#0f1923;color:#e0e0e0;min-height:100vh}
/* NAV */
nav{background:#111d2b;padding:12px 20px;display:flex;align-items:center;justify-content:space-between;border-bottom:1px solid #1e3a5f;position:sticky;top:0;z-index:100}
.nav-left{display:flex;align-items:center;gap:12px}
.logo{font-size:16px;font-weight:bold;color:#f59e0b}
.logo span{font-size:11px;color:#64748b;margin-left:6px}
.nav-tabs{display:flex;gap:4px}
.tab{padding:6px 14px;border-radius:6px;font-size:13px;cursor:pointer;border:none;background:transparent;color:#94a3b8;transition:all .2s}
.tab.active{background:#f59e0b;color:#0f1923;font-weight:bold}
.tab:hover:not(.active){background:#1e3a5f;color:#e0e0e0}
.conn-dot{width:8px;height:8px;border-radius:50%;background:#22c55e;display:inline-block;margin-left:4px}
.conn-dot.off{background:#ef4444}
/* MAIN */
main{max-width:860px;margin:0 auto;padding:20px 16px}
/* SECTION */
.section{background:#1a2535;border-radius:10px;margin-bottom:16px;overflow:hidden}
.section-header{padding:12px 16px;display:flex;align-items:center;justify-content:space-between;border-bottom:1px solid #2d3f55;cursor:pointer;user-select:none}
.section-title{font-size:13px;font-weight:bold;color:#f59e0b;display:flex;align-items:center;gap:8px}
.section-title .icon{font-size:14px}
.section-body{padding:16px}
.chevron{color:#64748b;font-size:12px;transition:transform .2s}
.chevron.open{transform:rotate(180deg)}
/* FORM */
.form-row{display:flex;gap:12px;margin-bottom:12px;flex-wrap:wrap}
.form-group{flex:1;min-width:180px}
.form-group.full{flex:0 0 100%}
label{display:block;font-size:11px;color:#94a3b8;margin-bottom:4px}
input[type=text],input[type=password],input[type=number],select{
  width:100%;padding:8px 10px;background:#0f1923;border:1px solid #2d3f55;
  border-radius:6px;color:#e0e0e0;font-size:13px;outline:none;transition:border .2s}
input:focus,select:focus{border-color:#f59e0b}
input[type=number]{-moz-appearance:textfield}
/* TOGGLE */
.toggle-row{display:flex;align-items:center;justify-content:space-between;padding:10px 0;border-bottom:1px solid #1e3a5f}
.toggle-row:last-child{border-bottom:none}
.toggle-label{font-size:13px;color:#e0e0e0}
.toggle-sub{font-size:11px;color:#64748b;margin-top:2px}
.toggle{position:relative;width:42px;height:24px;flex-shrink:0}
.toggle input{opacity:0;width:0;height:0}
.slider{position:absolute;inset:0;background:#2d3f55;border-radius:24px;cursor:pointer;transition:.3s}
.slider:before{content:"";position:absolute;width:18px;height:18px;left:3px;top:3px;background:#fff;border-radius:50%;transition:.3s}
input:checked+.slider{background:#f59e0b}
input:checked+.slider:before{transform:translateX(18px)}
/* BUTTONS */
.btn-row{display:flex;gap:8px;margin-top:16px;flex-wrap:wrap}
.btn{padding:9px 18px;border:none;border-radius:7px;font-size:13px;font-weight:bold;cursor:pointer;transition:opacity .2s}
.btn:hover{opacity:.85}
.btn-save{background:#f59e0b;color:#0f1923}
.btn-secondary{background:#2d3f55;color:#e0e0e0}
.btn-danger{background:#7f1d1d;color:#fca5a5}
.btn-sm{font-size:12px;padding:7px 12px}
/* STATUS */
.toast{position:fixed;bottom:24px;right:24px;padding:12px 18px;border-radius:8px;font-size:13px;font-weight:bold;display:none;z-index:999;animation:slideIn .3s}
.toast.ok{background:#064e3b;color:#6ee7b7;border:1px solid #065f46}
.toast.err{background:#450a0a;color:#fca5a5;border:1px solid #7f1d1d}
@keyframes slideIn{from{transform:translateY(20px);opacity:0}to{transform:translateY(0);opacity:1}}
/* SCAN LIST */
.scan-list{margin-top:6px;max-height:150px;overflow-y:auto;border:1px solid #2d3f55;border-radius:7px;display:none}
.scan-item{padding:8px 11px;border-bottom:1px solid #2d3f55;cursor:pointer;display:flex;justify-content:space-between;font-size:12px;transition:background .15s}
.scan-item:last-child{border-bottom:none}
.scan-item:hover,.scan-item.sel{background:#1f2d3d;color:#f59e0b}
.signal{font-size:11px;color:#64748b}
.ip-fields{display:none}
.spinner{display:inline-block;width:11px;height:11px;border:2px solid #f59e0b;border-top-color:transparent;border-radius:50%;animation:spin .8s linear infinite;margin-right:4px;vertical-align:middle}
@keyframes spin{to{transform:rotate(360deg)}}
.divider{height:1px;background:#2d3f55;margin:12px 0}
.unit{font-size:11px;color:#64748b;margin-left:4px}
</style>
</head>
<body>

<nav>
  <div class="nav-left">
    <div class="logo">&#9728; SolarMonitor <span>SETTING</span></div>
    <div class="nav-tabs">
      <button class="tab" onclick="location.href='/'">Dashboard</button>
      <button class="tab active">Setting</button>
      <button class="tab" onclick="location.href='/system'">System</button>
      <button class="tab" onclick="location.href='/log'">Log</button>
    </div>
  </div>
  <div style="display:flex;align-items:center;gap:6px;font-size:12px;color:#64748b">
    <span id="uptime-val">--:--:--</span>
    <span class="conn-dot" id="conn-dot"></span>
  </div>
</nav>

<main>

  <!-- SECTION: OLED -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-oled')">
      <div class="section-title"><span class="icon">&#128250;</span> Tampilan OLED</div>
      <span class="chevron" id="chev-sec-oled">&#9660;</span>
    </div>
    <div class="section-body" id="sec-oled" style="display:none">
      <div class="toggle-row">
        <div>
          <div class="toggle-label">Layar OLED</div>
          <div class="toggle-sub">Nyalakan/matikan layar OLED</div>
        </div>
        <label class="toggle"><input type="checkbox" id="oled-on"><span class="slider"></span></label>
      </div>
      <div class="toggle-row">
        <div>
          <div class="toggle-label">Auto-Scroll</div>
          <div class="toggle-sub">Ganti halaman otomatis</div>
        </div>
        <label class="toggle"><input type="checkbox" id="oled-scroll" onchange="toggleAutoScroll()"><span class="slider"></span></label>
      </div>
      <div class="form-row" style="margin-top:12px" id="oled-page-row">
        <div class="form-group">
          <label>Halaman Aktif</label>
          <select id="oled-page">
            <option value="0">Status WiFi</option>
            <option value="1">Ringkasan Semua Channel</option>
            <option value="2">CH1 - Panel Surya</option>
            <option value="3">CH2 - Baterai</option>
            <option value="4">CH3 - Beban</option>
            <option value="5">BMP280 - Lingkungan</option>
            <option value="6">MPU6050 - Getaran</option>
          </select>
        </div>
        <div class="form-group" id="oled-dur-group" style="display:none">
          <label>Durasi per Halaman <span class="unit">(detik)</span></label>
          <input type="number" id="oled-dur" step="1" min="1" max="30" placeholder="3">
        </div>
      </div>
      <div class="form-row">
        <div class="form-group full">
          <label>Kecerahan: <span id="bright-val">128</span></label>
          <input type="range" id="oled-bright" min="0" max="255" value="128"
            style="width:100%;accent-color:#f59e0b;margin-top:4px"
            oninput="document.getElementById('bright-val').textContent=this.value">
        </div>
      </div>
      <div class="btn-row">
        <button class="btn btn-save btn-sm" onclick="saveOled()">Simpan Setting OLED</button>
      </div>
    </div>
  </div>

  <!-- SECTION: NAMA CHANNEL -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-ch')">
      <div class="section-title"><span class="icon">&#128268;</span> Nama Channel</div>
      <span class="chevron" id="chev-sec-ch">&#9660;</span>
    </div>
    <div class="section-body" id="sec-ch" style="display:none">
      <div class="form-row">
        <div class="form-group">
          <label>Channel 1 (Panel → SCC)</label>
          <input type="text" id="ch1" maxlength="20" placeholder="Panel Surya">
        </div>
        <div class="form-group">
          <label>Channel 2 (SCC → Baterai)</label>
          <input type="text" id="ch2" maxlength="20" placeholder="Baterai">
        </div>
        <div class="form-group">
          <label>Channel 3 (Output → Beban)</label>
          <input type="text" id="ch3" maxlength="20" placeholder="Beban">
        </div>
      </div>
      <div class="btn-row">
        <button class="btn btn-save btn-sm" onclick="saveChannel()">Simpan Nama Channel</button>
      </div>
    </div>
  </div>

  <!-- SECTION: SENSOR -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-sensor')">
      <div class="section-title"><span class="icon">&#128268;</span> Sensor & Perangkat</div>
      <span class="chevron" id="chev-sec-sensor">&#9660;</span>
    </div>
    <div class="section-body" id="sec-sensor" style="display:none">
      <div class="toggle-row">
        <div>
          <div class="toggle-label">BMP280</div>
          <div class="toggle-sub">Sensor suhu, tekanan udara & ketinggian</div>
        </div>
        <label class="toggle"><input type="checkbox" id="sen-bmp"><span class="slider"></span></label>
      </div>
      <div class="toggle-row">
        <div>
          <div class="toggle-label">MPU6050</div>
          <div class="toggle-sub">Sensor akselerometer & deteksi gempa</div>
        </div>
        <label class="toggle"><input type="checkbox" id="sen-mpu"><span class="slider"></span></label>
      </div>
      <div class="toggle-row">
        <div>
          <div class="toggle-label">Buzzer</div>
          <div class="toggle-sub">Alarm suara untuk peringatan & gempa</div>
        </div>
        <label class="toggle"><input type="checkbox" id="sen-buzz"><span class="slider"></span></label>
      </div>
      <div class="btn-row">
        <button class="btn btn-save btn-sm" onclick="saveSensor()">Simpan Setting Sensor</button>
      </div>

      <div class="divider"></div>

      <!-- R Shunt INA3221 -->
      <div style="margin-top:4px">
        <div class="toggle-label">R Shunt INA3221</div>
        <div class="toggle-sub">Nilai resistor shunt yang terpasang. Ubah sesuai hardware.</div>
        <div class="form-row" style="margin-top:8px;align-items:flex-end">
          <div class="form-group" style="flex:1.5">
            <label>Nilai Shunt <span class="unit">(mΩ)</span></label>
            <input type="number" id="shunt_mohms" step="0.1" min="0.1" max="100" placeholder="5.0">
          </div>
          <div class="form-group" style="flex:1;font-size:11px;color:#64748b;padding-bottom:4px" id="shunt-info">
          </div>
          <div style="padding-bottom:2px">
            <button class="btn btn-save btn-sm" onclick="saveShunt()">Terapkan</button>
          </div>
        </div>
        <div id="shunt-status" style="display:none;font-size:12px;padding:6px 10px;border-radius:6px;margin-top:6px"></div>
      </div>

      <div class="divider"></div>

      <!-- Offset Suhu BMP280 -->
      <div style="margin-top:4px">
        <div class="toggle-label">Kalibrasi Offset Suhu BMP280</div>
        <div class="toggle-sub">Koreksi pembacaan suhu jika tidak akurat. Misal sensor baca 79°C, suhu nyata 29°C → isi <b style="color:#e0e0e0">-50</b></div>
        <div class="form-row" style="margin-top:8px;align-items:flex-end">
          <div class="form-group" style="flex:1">
            <label>Offset Suhu <span class="unit">(°C)</span></label>
            <input type="number" id="bmp_t_off" step="0.5" min="-80" max="80" placeholder="0">
          </div>
          <div class="form-group" style="flex:1.5;font-size:11px;color:#64748b;padding-bottom:4px" id="bmp-off-preview"></div>
          <div style="padding-bottom:2px">
            <button class="btn btn-save btn-sm" onclick="saveBmpOffset()">Terapkan</button>
          </div>
        </div>
        <div id="bmp-off-status" style="display:none;font-size:12px;padding:6px 10px;border-radius:6px;margin-top:6px"></div>
      </div>
    </div>
  </div>

  <!-- SECTION: BUZZER SETTING -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-buzz')">
      <div class="section-title"><span class="icon">&#128276;</span> Setting Buzzer</div>
      <span class="chevron" id="chev-sec-buzz">&#9660;</span>
    </div>
    <div class="section-body" id="sec-buzz" style="display:none">
      <div style="font-size:11px;color:#94a3b8;margin-bottom:12px">
        Tiap event punya pola bunyi sendiri. <b style="color:#e0e0e0">Durasi beep</b> = lama buzzer ON per beep, <b style="color:#e0e0e0">Delay</b> = jeda antar beep, <b style="color:#e0e0e0">Total</b> = berapa detik alarm berbunyi.
      </div>
      <div style="font-size:12px;font-weight:bold;color:#ef4444;margin-bottom:8px">&#127923; Gempa</div>
      <div class="form-row">
        <div class="form-group"><label>Durasi Beep <span class="unit">(ms)</span></label><input type="number" id="bq-on" step="10" min="50" max="1000" placeholder="100"></div>
        <div class="form-group"><label>Delay Antar Beep <span class="unit">(ms)</span></label><input type="number" id="bq-off" step="50" min="50" max="2000" placeholder="200"></div>
        <div class="form-group"><label>Durasi Total <span class="unit">(detik)</span></label><input type="number" id="bq-dur" step="1" min="1" max="60" placeholder="10"></div>
      </div>
      <div class="divider"></div>
      <div style="font-size:12px;font-weight:bold;color:#38bdf8;margin-bottom:8px">&#9889; Tegangan Rendah</div>
      <div class="form-row">
        <div class="form-group"><label>Durasi Beep <span class="unit">(ms)</span></label><input type="number" id="bv-on" step="10" min="50" max="1000" placeholder="200"></div>
        <div class="form-group"><label>Delay Antar Beep <span class="unit">(ms)</span></label><input type="number" id="bv-off" step="50" min="50" max="2000" placeholder="500"></div>
        <div class="form-group"><label>Durasi Total <span class="unit">(detik)</span></label><input type="number" id="bv-dur" step="1" min="1" max="60" placeholder="10"></div>
      </div>
      <div class="divider"></div>
      <div style="font-size:12px;font-weight:bold;color:#f59e0b;margin-bottom:8px">&#9889; Arus Berlebih</div>
      <div class="form-row">
        <div class="form-group"><label>Durasi Beep <span class="unit">(ms)</span></label><input type="number" id="bc-on" step="10" min="50" max="1000" placeholder="100"></div>
        <div class="form-group"><label>Delay Antar Beep <span class="unit">(ms)</span></label><input type="number" id="bc-off" step="50" min="50" max="2000" placeholder="100"></div>
        <div class="form-group"><label>Durasi Total <span class="unit">(detik)</span></label><input type="number" id="bc-dur" step="1" min="1" max="60" placeholder="10"></div>
      </div>
      <div class="btn-row">
        <button class="btn btn-save btn-sm" onclick="saveBuzzer()">Simpan Setting Buzzer</button>
        <button class="btn btn-secondary btn-sm" onclick="testBuzzer('quake')">Test Gempa</button>
        <button class="btn btn-secondary btn-sm" onclick="testBuzzer('volt')">Test Tegangan</button>
        <button class="btn btn-secondary btn-sm" onclick="testBuzzer('curr')">Test Arus</button>
      </div>
    </div>
  </div>

  <!-- SECTION: KALIBRASI MPU -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-mpu')">
      <div class="section-title"><span class="icon">&#127919;</span> Kalibrasi MPU6050</div>
      <span class="chevron" id="chev-sec-mpu">&#9660;</span>
    </div>
    <div class="section-body" id="sec-mpu" style="display:none">
      <div style="padding:10px;background:#0f1923;border-radius:6px;font-size:12px;color:#94a3b8;margin-bottom:14px;line-height:1.6">
        <b style="color:#f59e0b">Cara kalibrasi:</b><br>
        1. Pastikan perangkat <b style="color:#e0e0e0">datar dan tidak bergerak</b><br>
        2. Klik tombol "Mulai Kalibrasi"<br>
        3. ESP32 sampling ~3 detik lalu simpan otomatis<br>
        4. <b style="color:#ef4444">Jangan gerakkan perangkat</b> sampai muncul notifikasi selesai
      </div>
      <div id="calib-status" style="display:none;padding:10px;border-radius:6px;margin-bottom:12px;font-size:13px;font-weight:bold;text-align:center"></div>
      <div class="btn-row">
        <button class="btn btn-save btn-sm" id="btn-calib" onclick="startCalib()">&#127919; Mulai Kalibrasi</button>
        <button class="btn btn-danger btn-sm" onclick="resetCalib()">Reset Kalibrasi</button>
      </div>
      <div style="margin-top:12px;font-size:11px;color:#64748b" id="calib-info"></div>
    </div>
  </div>

  <!-- SECTION: THRESHOLD ALARM -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-alarm')">
      <div class="section-title"><span class="icon">&#9888;</span> Threshold Alarm</div>
      <span class="chevron" id="chev-sec-alarm">&#9660;</span>
    </div>
    <div class="section-body" id="sec-alarm" style="display:none">
      <div class="form-row">
        <div class="form-group">
          <label>Tegangan Minimum Baterai <span class="unit">(V)</span></label>
          <input type="number" id="alm-vmin" step="0.1" min="2.5" max="4.2" placeholder="3.0">
        </div>
        <div class="form-group">
          <label>Tegangan Maksimum Baterai <span class="unit">(V)</span></label>
          <input type="number" id="alm-vmax" step="0.1" min="3.0" max="4.5" placeholder="4.2">
        </div>
      </div>
      <div class="form-row">
        <div class="form-group">
          <label>Arus Maksimum <span class="unit">(A)</span></label>
          <input type="number" id="alm-imax" step="0.5" min="0" max="30" placeholder="10.0">
        </div>
        <div class="form-group">
          <label>Threshold Gempa <span class="unit">(g)</span></label>
          <input type="number" id="alm-quake" step="0.1" min="0.1" max="5.0" placeholder="0.5">
        </div>
      </div>
      <div class="btn-row">
        <button class="btn btn-save btn-sm" onclick="saveAlarm()">Simpan Threshold</button>
      </div>
    </div>
  </div>

  <!-- SECTION: WIFI -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-wifi')">
      <div class="section-title"><span class="icon">&#128246;</span> Koneksi WiFi</div>
      <span class="chevron" id="chev-sec-wifi">&#9660;</span>
    </div>
    <div class="section-body" id="sec-wifi" style="display:none">
      <div class="form-row">
        <div class="form-group full">
          <label>Nama Perangkat (hostname)</label>
          <input type="text" id="devname" maxlength="24" placeholder="Monitoring-Daya">
        </div>
      </div>
      <div class="divider"></div>
      <label>Jaringan WiFi</label>
      <button class="btn btn-secondary btn-sm" style="margin-top:6px;width:100%" onclick="scanWifi()" id="btn-scan">&#128268; Scan Jaringan</button>
      <div class="scan-list" id="scan-list"></div>
      <div class="form-row" style="margin-top:10px">
        <div class="form-group">
          <label>SSID</label>
          <input type="text" id="ssid" placeholder="Nama jaringan WiFi">
        </div>
        <div class="form-group">
          <label>Password</label>
          <input type="password" id="wpass" placeholder="Kata sandi WiFi">
        </div>
      </div>
      <div class="divider"></div>
      <label style="margin-bottom:8px;display:block">Mode IP</label>
      <div style="display:flex;gap:8px;margin-bottom:10px">
        <button class="btn btn-save btn-sm" id="btn-dhcp" onclick="setIPMode('dhcp')">DHCP (Otomatis)</button>
        <button class="btn btn-secondary btn-sm" id="btn-static" onclick="setIPMode('static')">IP Statis</button>
      </div>
      <div class="ip-fields" id="ip-fields">
        <div class="form-row">
          <div class="form-group"><label>IP Address</label><input type="text" id="ip" placeholder="192.168.1.100"></div>
          <div class="form-group"><label>Gateway</label><input type="text" id="gw" placeholder="192.168.1.1"></div>
        </div>
        <div class="form-row">
          <div class="form-group"><label>Subnet Mask</label><input type="text" id="sn" placeholder="255.255.255.0"></div>
          <div class="form-group"><label>DNS</label><input type="text" id="dns" placeholder="8.8.8.8"></div>
        </div>
      </div>
      <div class="btn-row">
        <button class="btn btn-save btn-sm" onclick="saveWifi()">Simpan & Restart</button>
        <button class="btn btn-danger btn-sm" onclick="resetWifi()">Reset WiFi</button>
      </div>
    </div>
  </div>

  <!-- SECTION: LOKASI PERANGKAT -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-loc')">
      <div class="section-title"><span class="icon">&#127757;</span> Lokasi Perangkat</div>
      <span class="chevron" id="chev-sec-loc">&#9660;</span>
    </div>
    <div class="section-body" id="sec-loc" style="display:none">
      <div id="loc-status-bar" style="display:none;padding:10px 12px;border-radius:8px;margin-bottom:12px;font-size:13px;"></div>

      <div class="form-row">
        <div class="form-group">
          <label>Nama Lokasi</label>
          <input type="text" id="loc_name" placeholder="Cth: Rumah - Kediri">
        </div>
      </div>
      <div class="form-row">
        <div class="form-group">
          <label>Latitude</label>
          <input type="text" id="loc_lat" placeholder="-7.8234567">
        </div>
        <div class="form-group">
          <label>Longitude</label>
          <input type="text" id="loc_lng" placeholder="112.0123456">
        </div>
      </div>
      <div class="form-row">
        <div class="form-group">
          <label>Altitude (meter)</label>
          <input type="text" id="loc_alt" placeholder="0">
        </div>
      </div>

      <div style="display:flex;gap:8px;margin-top:4px;flex-wrap:wrap">
        <button class="btn btn-secondary btn-sm" onclick="getGPS()" id="btn-gps">&#128205; Ambil dari GPS Browser</button>
        <button class="btn btn-save btn-sm" onclick="saveLoc()">Simpan Lokasi</button>
      </div>
      <div id="loc-map-link" style="display:none;margin-top:10px;font-size:12px;color:#94a3b8">
        &#128205; <a id="loc-gmap" href="#" target="_blank" style="color:#f59e0b">Lihat di Google Maps</a>
      </div>
    </div>
  </div>

  <!-- SECTION: MQTT -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-mqtt')">
      <div class="section-title"><span class="icon">&#128225;</span> MQTT</div>
      <span class="chevron" id="chev-sec-mqtt">&#9660;</span>
    </div>
    <div class="section-body" id="sec-mqtt" style="display:none">

      <!-- Data selector helper -->
      <div style="background:#0a1520;border-radius:8px;padding:10px 12px;margin-bottom:14px;font-size:12px;color:#64748b">
        <b style="color:#94a3b8">Pilih Data:</b> &nbsp;
        Bit: 1=INA3221 &nbsp; 2=BMP280 &nbsp; 4=MPU/Gempa &nbsp; 8=Alarm &nbsp; 16=Lokasi &nbsp;
        <br>Jumlahkan: semua=31, hanya MPU+Lokasi=20, hanya INA=1
      </div>

      <!-- BROKER A -->
      <div style="border:1px solid #2d3f55;border-radius:8px;padding:12px;margin-bottom:14px">
        <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:10px">
          <div style="display:flex;align-items:center;gap:8px">
            <b style="color:#f59e0b;font-size:13px">&#128308; Broker A</b>
            <span id="mqtt-a-badge" style="font-size:11px;padding:2px 8px;border-radius:10px;background:#1a2535;color:#64748b">⚫ Nonaktif</span>
          </div>
          <label class="toggle-switch"><input type="checkbox" id="ma_en"><span class="toggle-thumb"></span></label>
        </div>
        <div class="form-row">
          <div class="form-group" style="flex:2">
            <label>Host / IP</label>
            <input type="text" id="ma_host" placeholder="192.168.1.10 atau broker.hivemq.com">
          </div>
          <div class="form-group" style="flex:0.8">
            <label>Port</label>
            <input type="text" id="ma_port" placeholder="1883">
          </div>
        </div>
        <div class="form-row">
          <div class="form-group">
            <label>Username (opsional)</label>
            <input type="text" id="ma_user" placeholder="kosong jika tidak ada">
          </div>
          <div class="form-group">
            <label>Password</label>
            <input type="password" id="ma_pass" placeholder="••••••">
          </div>
        </div>
        <div class="form-row">
          <div class="form-group">
            <label>Base Topic</label>
            <input type="text" id="ma_topic" placeholder="solar">
          </div>
          <div class="form-group">
            <label>Interval Kirim (detik)</label>
            <input type="number" id="ma_int" min="5" max="3600" placeholder="10">
          </div>
        </div>
        <div class="form-row">
          <div class="form-group">
            <label>Data (bit field, 1-31)</label>
            <input type="number" id="ma_data" min="1" max="31" placeholder="31">
          </div>
          <div class="form-group" style="display:flex;align-items:flex-end">
            <div id="ma_data_desc" style="font-size:11px;color:#64748b;padding-bottom:2px"></div>
          </div>
        </div>
        <div id="ma-status" style="display:none;font-size:12px;margin-top:6px;padding:6px 10px;border-radius:6px"></div>
      </div>

      <!-- BROKER B -->
      <div style="border:1px solid #2d3f55;border-radius:8px;padding:12px;margin-bottom:14px">
        <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:10px">
          <div style="display:flex;align-items:center;gap:8px">
            <b style="color:#3b82f6;font-size:13px">&#128309; Broker B</b>
            <span id="mqtt-b-badge" style="font-size:11px;padding:2px 8px;border-radius:10px;background:#1a2535;color:#64748b">⚫ Nonaktif</span>
          </div>
          <label class="toggle-switch"><input type="checkbox" id="mb_en"><span class="toggle-thumb"></span></label>
        </div>
        <div class="form-row">
          <div class="form-group" style="flex:2">
            <label>Host / IP</label>
            <input type="text" id="mb_host" placeholder="192.168.1.10 atau broker.hivemq.com">
          </div>
          <div class="form-group" style="flex:0.8">
            <label>Port</label>
            <input type="text" id="mb_port" placeholder="1883">
          </div>
        </div>
        <div class="form-row">
          <div class="form-group">
            <label>Username (opsional)</label>
            <input type="text" id="mb_user" placeholder="kosong jika tidak ada">
          </div>
          <div class="form-group">
            <label>Password</label>
            <input type="password" id="mb_pass" placeholder="••••••">
          </div>
        </div>
        <div class="form-row">
          <div class="form-group">
            <label>Base Topic</label>
            <input type="text" id="mb_topic" placeholder="solar">
          </div>
          <div class="form-group">
            <label>Interval Kirim (detik)</label>
            <input type="number" id="mb_int" min="5" max="3600" placeholder="10">
          </div>
        </div>
        <div class="form-row">
          <div class="form-group">
            <label>Data (bit field, 1-31)</label>
            <input type="number" id="mb_data" min="1" max="31" placeholder="31">
          </div>
          <div class="form-group" style="display:flex;align-items:flex-end">
            <div id="mb_data_desc" style="font-size:11px;color:#64748b;padding-bottom:2px"></div>
          </div>
        </div>
        <div id="mb-status" style="display:none;font-size:12px;margin-top:6px;padding:6px 10px;border-radius:6px"></div>
      </div>

      <button class="btn btn-save" onclick="saveMqtt()">Simpan & Terapkan MQTT</button>
    </div>
  </div>
  <div class="section">
    <!-- SECTION: WAKTU & NTP -->
  <div class="section">
    <div class="section-header" onclick="toggle('sec-ntp')">
      <div class="section-title"><span class="icon">&#128336;</span> Waktu &amp; NTP</div>
      <span class="chevron" id="chev-sec-ntp">&#9660;</span>
    </div>
    <div class="section-body" id="sec-ntp" style="display:none">
      <div class="toggle-row">
        <div>
          <div class="toggle-label">Sinkronisasi Waktu (NTP)</div>
          <div class="toggle-sub">Ambil waktu dari internet tiap boot dan tiap 6 jam</div>
        </div>
        <label class="toggle-switch"><input type="checkbox" id="ntp_en"><span class="toggle-thumb"></span></label>
      </div>
      <div class="form-row" style="margin-top:12px">
        <div class="form-group" style="flex:2">
          <label>NTP Server</label>
          <input type="text" id="ntp_server" placeholder="pool.ntp.org">
        </div>
        <div class="form-group" style="flex:1;min-width:120px">
          <label>Offset Timezone (UTC)</label>
          <input type="number" id="ntp_offset" placeholder="7" min="-12" max="14">
        </div>
      </div>
      <div style="font-size:11px;color:#64748b;margin-bottom:12px">
        Contoh: WIB = +7, WITA = +8, WIT = +9, Singapore = +8, London = 0, New York = -5
      </div>
      <div id="ntp-status" style="display:none;font-size:12px;padding:8px 12px;border-radius:7px;margin-bottom:10px"></div>
      <div style="display:flex;gap:8px;flex-wrap:wrap">
        <button class="btn btn-save btn-sm" onclick="saveNtp()">Simpan & Terapkan</button>
        <button class="btn btn-secondary btn-sm" onclick="syncNtpNow()">&#128260; Sync Sekarang</button>
      </div>
    </div>
  </div>

    <!-- SECTION: SISTEM -->
    <div class="section-header" onclick="toggle('sec-sys')">
      <div class="section-title"><span class="icon">&#9881;</span> Sistem</div>
      <span class="chevron" id="chev-sec-sys">&#9660;</span>
    </div>
    <div class="section-body" id="sec-sys" style="display:none">

      <!-- Ubah Username & Password -->
      <div style="margin-bottom:16px">
        <div class="toggle-label" style="margin-bottom:8px">&#128274; Ubah Username & Password</div>
        <div class="form-row">
          <div class="form-group">
            <label>Username Baru</label>
            <input type="text" id="new-usr" placeholder="admin" autocomplete="off">
          </div>
          <div class="form-group">
            <label>Password Baru</label>
            <div style="position:relative">
              <input type="password" id="new-pwd" placeholder="••••••" autocomplete="new-password">
              <button type="button" onclick="toggleNewPwd()"
                style="position:absolute;right:8px;top:50%;transform:translateY(-50%);
                  background:none;border:none;color:#64748b;cursor:pointer;font-size:14px">&#128065;</button>
            </div>
          </div>
          <div class="form-group">
            <label>Konfirmasi Password</label>
            <input type="password" id="new-pwd2" placeholder="••••••" autocomplete="new-password">
          </div>
        </div>
        <button class="btn btn-primary btn-sm" onclick="saveAuth()">Simpan Kredensial</button>
        <div id="auth-status" style="display:none;font-size:12px;padding:6px 10px;border-radius:6px;margin-top:6px"></div>
      </div>

      <div class="toggle-row">
        <div>
          <div class="toggle-label">Reset Akumulasi kWh</div>
          <div class="toggle-sub">Menghapus data kWh semua channel</div>
        </div>
        <button class="btn btn-danger btn-sm" onclick="resetKwh()">Reset kWh</button>
      </div>
      <div class="toggle-row">
        <div>
          <div class="toggle-label">Restart Perangkat</div>
          <div class="toggle-sub">Restart ESP32 tanpa menghapus data</div>
        </div>
        <button class="btn btn-secondary btn-sm" onclick="restartDevice()">Restart</button>
      </div>
      <div class="toggle-row">
        <div>
          <div class="toggle-label">Logout</div>
          <div class="toggle-sub">Keluar dari sesi ini</div>
        </div>
        <button class="btn btn-secondary btn-sm" onclick="doLogout()">&#128274; Logout</button>
      </div>
    </div>
  </div>

</main>

<div class="toast" id="toast"></div>

<script>
// ============================================================
// STATE
// ============================================================
let ipMode = 'dhcp';

// ============================================================
// INIT — load semua setting saat halaman dibuka
// ============================================================
window.onload = () => {
  loadSetting();
  startUptime();
};

function loadSetting() {
  fetch('/api/setting')
    .then(r => r.json())
    .then(d => {
      // OLED
      document.getElementById('oled-on').checked     = d.oled_on !== false;
      document.getElementById('oled-scroll').checked = d.oled_scroll;
      document.getElementById('oled-page').value     = d.oled_page || 0;
      document.getElementById('oled-dur').value      = d.oled_dur  || 3;
      document.getElementById('oled-bright').value   = d.oled_bright !== undefined ? d.oled_bright : 128;
      document.getElementById('bright-val').textContent = d.oled_bright !== undefined ? d.oled_bright : 128;
      toggleAutoScroll();
      // Buzzer per event
      document.getElementById('bq-on').value  = d.bq_on  || 100;
      document.getElementById('bq-off').value = d.bq_off || 200;
      document.getElementById('bq-dur').value = d.bq_dur || 10;
      document.getElementById('bv-on').value  = d.bv_on  || 200;
      document.getElementById('bv-off').value = d.bv_off || 500;
      document.getElementById('bv-dur').value = d.bv_dur || 10;
      document.getElementById('bc-on').value  = d.bc_on  || 100;
      document.getElementById('bc-off').value = d.bc_off || 100;
      document.getElementById('bc-dur').value = d.bc_dur || 10;
      // Kalibrasi MPU
      const ci = document.getElementById('calib-info');
      if (d.mpu_cal) {
        ci.innerHTML = '✓ Sudah dikalibrasi — offset tersimpan di memori';
        ci.style.color = '#22c55e';
      } else {
        ci.innerHTML = '⚠ Belum dikalibrasi — deteksi gempa mungkin tidak akurat';
        ci.style.color = '#f59e0b';
      }
      // Channel
      document.getElementById('ch1').value  = d.ch1  || '';
      document.getElementById('ch2').value  = d.ch2  || '';
      document.getElementById('ch3').value  = d.ch3  || '';
      // Sensor
      document.getElementById('sen-bmp').checked  = d.bmp;
      document.getElementById('sen-mpu').checked  = d.mpu;
      document.getElementById('sen-buzz').checked = d.buzz;
      // R Shunt — tampil dalam mΩ
      if (d.shunt_ohms !== undefined) {
        const mohms = (d.shunt_ohms * 1000).toFixed(3);
        document.getElementById('shunt_mohms').value = parseFloat(mohms);
        document.getElementById('shunt-info').textContent = '= ' + d.shunt_ohms.toFixed(6) + ' Ω';
      }
      // BMP Offset suhu
      if (d.bmp_t_off !== undefined) {
        document.getElementById('bmp_t_off').value = d.bmp_t_off;
        updateBmpOffPreview();
      }
      // Alarm
      document.getElementById('alm-vmin').value  = d.vmin;
      document.getElementById('alm-vmax').value  = d.vmax;
      document.getElementById('alm-imax').value  = d.imax;
      document.getElementById('alm-quake').value = d.quake;
      // WiFi
      document.getElementById('devname').value = d.devname || '';
      document.getElementById('ssid').value    = d.ssid    || '';
      // IP mode
      setIPMode(d.ip_mode || 'dhcp');
      if (d.ip_mode === 'static') {
        document.getElementById('ip').value  = d.ip  || '';
        document.getElementById('gw').value  = d.gw  || '';
        document.getElementById('sn').value  = d.sn  || '';
        document.getElementById('dns').value = d.dns || '';
      }
      // Lokasi
      document.getElementById('loc_name').value = d.loc_name || '';
      document.getElementById('loc_lat').value  = d.loc_lat  || '';
      document.getElementById('loc_lng').value  = d.loc_lng  || '';
      document.getElementById('loc_alt').value  = d.loc_alt  || '';
      if (d.loc_set && d.loc_lat && d.loc_lng) {
        const bar = document.getElementById('loc-status-bar');
        bar.style.display = 'block';
        bar.style.background = '#064e3b'; bar.style.color = '#6ee7b7';
        bar.textContent = '✓ Lokasi tersimpan: ' + (d.loc_name||'?') + ' ('+d.loc_lat.toFixed(5)+', '+d.loc_lng.toFixed(5)+')';
        const ml = document.getElementById('loc-map-link');
        ml.style.display = 'block';
        document.getElementById('loc-gmap').href = 'https://maps.google.com/?q='+d.loc_lat+','+d.loc_lng;
      }
      // MQTT Broker A
      document.getElementById('ma_en').checked    = d.ma_en || false;
      document.getElementById('ma_host').value    = d.ma_host  || '';
      document.getElementById('ma_port').value    = d.ma_port  || 1883;
      document.getElementById('ma_user').value    = d.ma_user  || '';
      document.getElementById('ma_topic').value   = d.ma_topic || 'solar';
      document.getElementById('ma_int').value     = d.ma_int   || 10;
      document.getElementById('ma_data').value    = d.ma_data  !== undefined ? d.ma_data : 31;
      updateDataDesc('ma');
      // MQTT Broker B
      document.getElementById('mb_en').checked    = d.mb_en || false;
      document.getElementById('mb_host').value    = d.mb_host  || '';
      document.getElementById('mb_port').value    = d.mb_port  || 1883;
      document.getElementById('mb_user').value    = d.mb_user  || '';
      document.getElementById('mb_topic').value   = d.mb_topic || 'solar';
      document.getElementById('mb_int').value     = d.mb_int   || 10;
      document.getElementById('mb_data').value    = d.mb_data  !== undefined ? d.mb_data : 31;
      updateDataDesc('mb');
      // NTP
      document.getElementById('ntp_en').checked     = d.ntp_en !== false;
      document.getElementById('ntp_server').value   = d.ntp_server || 'pool.ntp.org';
      document.getElementById('ntp_offset').value   = d.ntp_offset !== undefined ? d.ntp_offset : 7;
      // Koneksi dot
      document.getElementById('conn-dot').className = 'conn-dot';
    })
    .catch(() => {
      document.getElementById('conn-dot').className = 'conn-dot off';
    });
}

// ============================================================
// NTP
// ============================================================
function saveNtp() {
  const st = document.getElementById('ntp-status');
  const show = (msg, ok) => {
    st.style.display='block';
    st.style.background = ok ? '#052e16' : '#450a0a';
    st.style.color = ok ? '#4ade80' : '#fca5a5';
    st.textContent = msg;
  };
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({
      type: 'ntp',
      ntp_en:     document.getElementById('ntp_en').checked,
      ntp_server: document.getElementById('ntp_server').value.trim(),
      ntp_offset: parseInt(document.getElementById('ntp_offset').value) || 7
    })
  }).then(r=>r.json()).then(d=>{
    if (d.success) show('✓ Setting NTP tersimpan & diterapkan', true);
    else show('✗ Gagal menyimpan', false);
  }).catch(()=>show('✗ Koneksi terputus', false));
}

function syncNtpNow() {
  const st = document.getElementById('ntp-status');
  st.style.display='block';
  st.style.background='#0a1520';
  st.style.color='#94a3b8';
  st.textContent = '⏳ Sync NTP...';
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({type:'ntp_sync'})
  }).then(r=>r.json()).then(d=>{
    st.style.background = d.success ? '#052e16' : '#450a0a';
    st.style.color = d.success ? '#4ade80' : '#fca5a5';
    st.textContent = d.success ? '✓ Sync OK: ' + (d.time||'') : '✗ Sync gagal';
  }).catch(()=>{
    st.style.background='#450a0a'; st.style.color='#fca5a5';
    st.textContent='✗ Koneksi terputus';
  });
}

function mqttBadgeHtml(ok, enabled) {
  if (!enabled) return '⚫ Nonaktif';
  return ok ? '🟢 Terhubung' : '🔴 Terputus';
}
function mqttBadgeStyle(ok, enabled) {
  if (!enabled) return 'background:#1a2535;color:#64748b';
  return ok ? 'background:#052e16;color:#4ade80' : 'background:#450a0a;color:#fca5a5';
}

// ============================================================
// UPTIME + MQTT STATUS
// ============================================================
function startUptime() {
  const pollStatus = () => {
    fetch('/api/status')
      .then(r => r.json())
      .then(d => {
        const enA = document.getElementById('ma_en')?.checked;
        const enB = document.getElementById('mb_en')?.checked;
        const ba = document.getElementById('mqtt-a-badge');
        const bb = document.getElementById('mqtt-b-badge');
        if (ba) { ba.textContent = mqttBadgeHtml(d.mqtt_a, enA); ba.style.cssText = mqttBadgeStyle(d.mqtt_a, enA) + ';font-size:11px;padding:2px 8px;border-radius:10px'; }
        if (bb) { bb.textContent = mqttBadgeHtml(d.mqtt_b, enB); bb.style.cssText = mqttBadgeStyle(d.mqtt_b, enB) + ';font-size:11px;padding:2px 8px;border-radius:10px'; }
        document.getElementById('conn-dot').className = 'conn-dot';
      })
      .catch(() => document.getElementById('conn-dot').className = 'conn-dot off');
  };

  setInterval(() => {
    fetch('/api/data')
      .then(r => r.json())
      .then(d => {
        const s = parseInt(d.uptime) || 0;
        const h = Math.floor(s/3600), m = Math.floor((s%3600)/60), ss = s%60;
        document.getElementById('uptime-val').textContent =
          String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(ss).padStart(2,'0');
        document.getElementById('conn-dot').className = 'conn-dot';
      })
      .catch(() => document.getElementById('conn-dot').className = 'conn-dot off');
    pollStatus();
  }, 5000);
  pollStatus();
}

// ============================================================
// BUZZER
// ============================================================
function saveBuzzer() {
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({
      type:   'buzzer',
      bq_on:  parseInt(document.getElementById('bq-on').value),
      bq_off: parseInt(document.getElementById('bq-off').value),
      bq_dur: parseInt(document.getElementById('bq-dur').value),
      bv_on:  parseInt(document.getElementById('bv-on').value),
      bv_off: parseInt(document.getElementById('bv-off').value),
      bv_dur: parseInt(document.getElementById('bv-dur').value),
      bc_on:  parseInt(document.getElementById('bc-on').value),
      bc_off: parseInt(document.getElementById('bc-off').value),
      bc_dur: parseInt(document.getElementById('bc-dur').value)
    })
  }).then(r=>r.json()).then(d => toast(d.success ? '✓ Setting buzzer tersimpan' : '✗ Gagal', d.success))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

function testBuzzer(event) {
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'buzz-test', event: event })
  }).then(()=> toast('🔔 Test buzzer dijalankan'))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

// ============================================================
// KALIBRASI MPU
// ============================================================
function startCalib() {
  const btn  = document.getElementById('btn-calib');
  const stat = document.getElementById('calib-status');
  btn.disabled = true;
  btn.innerHTML = '<span class="spinner"></span>Kalibrasi...';
  stat.style.display = 'none';

  // Kirim perintah mulai
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'calib-start' })
  }).then(() => {
    // Poll setiap 500ms sampai selesai (max 10 detik)
    let tries = 0;
    const poll = setInterval(() => {
      tries++;
      fetch('/api/setting', { method:'POST',
        headers:{'Content-Type':'application/json'},
        body: JSON.stringify({ type: 'calib-poll' })
      }).then(r=>r.json()).then(d => {
        if (d.done || tries > 20) {
          clearInterval(poll);
          btn.disabled = false;
          btn.innerHTML = '&#127919; Mulai Kalibrasi';
          stat.style.display = 'block';
          const ci = document.getElementById('calib-info');
          if (d.success) {
            stat.style.background = '#064e3b';
            stat.style.color = '#6ee7b7';
            stat.textContent = '✓ Kalibrasi berhasil! Offset disimpan.';
            ci.innerHTML = '✓ Sudah dikalibrasi — offset tersimpan di memori';
            ci.style.color = '#22c55e';
          } else {
            stat.style.background = '#450a0a';
            stat.style.color = '#fca5a5';
            stat.textContent = '✗ Kalibrasi gagal — cek koneksi MPU6050';
          }
          setTimeout(()=>stat.style.display='none', 4000);
        }
      }).catch(()=>{ clearInterval(poll); btn.disabled=false; btn.innerHTML='&#127919; Mulai Kalibrasi'; });
    }, 500);
  }).catch(()=>{
    btn.disabled = false;
    btn.innerHTML = '&#127919; Mulai Kalibrasi';
    toast('✗ Gagal memulai kalibrasi', false);
  });
}

function resetCalib() {
  if (!confirm('Reset data kalibrasi MPU6050?')) return;
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'calib-reset' })
  }).then(r=>r.json()).then(d => {
    if (d.success) {
      toast('✓ Kalibrasi direset');
      const ci = document.getElementById('calib-info');
      ci.innerHTML = '⚠ Belum dikalibrasi — deteksi gempa mungkin tidak akurat';
      ci.style.color = '#f59e0b';
    } else toast('✗ Gagal', false);
  }).catch(()=>toast('✗ Koneksi terputus', false));
}

// ============================================================
// OLED
// ============================================================
function toggleAutoScroll() {
  const scroll = document.getElementById('oled-scroll').checked;
  document.getElementById('oled-page-row').style.display = scroll ? 'none' : 'flex';
  document.getElementById('oled-dur-group').style.display = scroll ? 'block' : 'none';
  if (scroll) document.getElementById('oled-page-row').style.display = 'none';
}

function saveOled() {
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({
      type:        'oled',
      oled_on:     document.getElementById('oled-on').checked,
      oled_scroll: document.getElementById('oled-scroll').checked,
      oled_page:   parseInt(document.getElementById('oled-page').value),
      oled_dur:    parseInt(document.getElementById('oled-dur').value) * 1000,
      oled_bright: parseInt(document.getElementById('oled-bright').value)
    })
  }).then(r=>r.json()).then(d => toast(d.success ? '✓ Setting OLED tersimpan' : '✗ Gagal', d.success))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

// ============================================================
// SECTION TOGGLE
// ============================================================
function toggle(id) {
  const el = document.getElementById(id);
  const chev = document.getElementById('chev-' + id);
  const open = el.style.display !== 'none';
  el.style.display = open ? 'none' : 'block';
  chev.className = 'chevron' + (open ? '' : ' open');
}

// ============================================================
// IP MODE
// ============================================================
function setIPMode(m) {
  ipMode = m;
  document.getElementById('btn-dhcp').className   = 'btn btn-sm ' + (m==='dhcp'   ? 'btn-save' : 'btn-secondary');
  document.getElementById('btn-static').className = 'btn btn-sm ' + (m==='static' ? 'btn-save' : 'btn-secondary');
  document.getElementById('ip-fields').style.display = m==='static' ? 'block' : 'none';
}

// ============================================================
// WIFI SCAN
// ============================================================
function scanWifi() {
  const btn = document.getElementById('btn-scan');
  btn.innerHTML = '<span class="spinner"></span>Memindai...';
  btn.disabled = true;
  fetch('/api/scan').then(r=>r.json()).then(d=>{
    const list = document.getElementById('scan-list');
    list.style.display = 'block'; list.innerHTML = '';
    if (!d.networks.length) { list.innerHTML = '<div class="scan-item">Tidak ada jaringan</div>'; }
    else d.networks.forEach(n => {
      const div = document.createElement('div');
      div.className = 'scan-item';
      div.innerHTML = `<span>${n.ssid}</span><span class="signal">${n.rssi} dBm</span>`;
      div.onclick = () => {
        document.getElementById('ssid').value = n.ssid;
        list.querySelectorAll('.scan-item').forEach(i=>i.classList.remove('sel'));
        div.classList.add('sel');
      };
      list.appendChild(div);
    });
    btn.innerHTML = '&#128268; Scan Ulang'; btn.disabled = false;
  }).catch(()=>{ btn.innerHTML = '&#128268; Scan Jaringan'; btn.disabled = false; });
}

// ============================================================
// SAVE FUNCTIONS
// ============================================================
function toast(msg, ok=true) {
  const t = document.getElementById('toast');
  t.className = 'toast ' + (ok ? 'ok' : 'err');
  t.textContent = msg; t.style.display = 'block';
  setTimeout(() => t.style.display = 'none', 3000);
}

function saveChannel() {
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({
      type: 'channel',
      ch1: document.getElementById('ch1').value.trim(),
      ch2: document.getElementById('ch2').value.trim(),
      ch3: document.getElementById('ch3').value.trim()
    })
  }).then(r=>r.json()).then(d => toast(d.success ? '✓ Nama channel tersimpan' : '✗ Gagal', d.success))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

function saveSensor() {
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({
      type: 'sensor',
      bmp:  document.getElementById('sen-bmp').checked,
      mpu:  document.getElementById('sen-mpu').checked,
      buzz: document.getElementById('sen-buzz').checked
    })
  }).then(r=>r.json()).then(d => toast(d.success ? '✓ Setting sensor tersimpan' : '✗ Gagal', d.success))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

function saveAlarm() {
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({
      type:  'alarm',
      vmin:  parseFloat(document.getElementById('alm-vmin').value),
      vmax:  parseFloat(document.getElementById('alm-vmax').value),
      imax:  parseFloat(document.getElementById('alm-imax').value),
      quake: parseFloat(document.getElementById('alm-quake').value)
    })
  }).then(r=>r.json()).then(d => toast(d.success ? '✓ Threshold tersimpan' : '✗ Gagal', d.success))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

function saveWifi() {
  if (!confirm('Perangkat akan restart. Lanjutkan?')) return;
  const payload = {
    type: 'wifi',
    devname:  document.getElementById('devname').value.trim(),
    ssid:     document.getElementById('ssid').value.trim(),
    password: document.getElementById('wpass').value,
    ip_mode:  ipMode
  };
  if (ipMode === 'static') {
    payload.ip  = document.getElementById('ip').value.trim();
    payload.gw  = document.getElementById('gw').value.trim();
    payload.sn  = document.getElementById('sn').value.trim();
    payload.dns = document.getElementById('dns').value.trim();
  }
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify(payload)
  }).then(r=>r.json()).then(d => {
    if (d.success) toast('✓ Tersimpan! Restart dalam 3 detik...');
    else toast('✗ Gagal menyimpan', false);
  }).catch(()=>toast('✗ Koneksi terputus', false));
}

function resetWifi() {
  if (!confirm('Reset semua setting WiFi? Perangkat akan masuk Setup Mode.')) return;
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'reset-wifi' })
  }).then(()=> toast('✓ Reset WiFi! Konek ke WifiMonitor-Setup'))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

function resetKwh() {
  if (!confirm('Hapus semua data akumulasi kWh?')) return;
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'reset-kwh' })
  }).then(r=>r.json()).then(d => toast(d.success ? '✓ Data kWh direset' : '✗ Gagal', d.success))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

// ============================================================
// R SHUNT
// ============================================================
function updateBmpOffPreview() {
  const off = parseFloat(document.getElementById('bmp_t_off').value) || 0;
  const el  = document.getElementById('bmp-off-preview');
  if (!el) return;
  if (off === 0) { el.textContent = ''; return; }
  el.textContent = 'Contoh: sensor 79°C → tampil ' + (79 + off).toFixed(1) + '°C';
}
document.addEventListener('DOMContentLoaded', () => {
  const bo = document.getElementById('bmp_t_off');
  if (bo) bo.addEventListener('input', updateBmpOffPreview);
});

function saveBmpOffset() {
  const off  = parseFloat(document.getElementById('bmp_t_off').value) || 0;
  const stat = document.getElementById('bmp-off-status');
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'bmp-offset', bmp_t_off: off })
  }).then(r=>r.json()).then(d => {
    stat.style.display = 'block';
    if (d.success) {
      stat.style.background = '#064e3b'; stat.style.color = '#6ee7b7';
      stat.textContent = '✓ Offset ' + (off >= 0 ? '+' : '') + off + '°C diterapkan!';
      toast('✓ Offset suhu disimpan!');
    } else {
      stat.style.background = '#450a0a'; stat.style.color = '#fca5a5';
      stat.textContent = '✗ Gagal menyimpan';
    }
    setTimeout(()=>stat.style.display='none', 4000);
  }).catch(()=>toast('✗ Koneksi terputus', false));
}

function saveShunt() {
  const mohms = parseFloat(document.getElementById('shunt_mohms').value);
  const stat  = document.getElementById('shunt-status');
  if (isNaN(mohms) || mohms <= 0) {
    toast('✗ Nilai shunt tidak valid', false);
    return;
  }
  const ohms = mohms / 1000.0;
  document.getElementById('shunt-info').textContent = '= ' + ohms.toFixed(6) + ' Ω';

  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'shunt', shunt_ohms: ohms })
  }).then(r=>r.json()).then(d => {
    stat.style.display = 'block';
    if (d.success) {
      stat.style.background = '#064e3b'; stat.style.color = '#6ee7b7';
      stat.textContent = '✓ Shunt ' + mohms.toFixed(3) + ' mΩ diterapkan langsung!';
      toast('✓ R Shunt diperbarui!');
    } else {
      stat.style.background = '#450a0a'; stat.style.color = '#fca5a5';
      stat.textContent = '✗ ' + (d.msg || 'Gagal');
    }
    setTimeout(()=>stat.style.display='none', 4000);
  }).catch(()=>toast('✗ Koneksi terputus', false));
}

function restartDevice() {
  if (!confirm('Restart perangkat?')) return;
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type: 'restart' })
  }).then(()=> toast('✓ Restart dalam 3 detik...'))
    .catch(()=>toast('✗ Koneksi terputus', false));
}

function toggleNewPwd() {
  const i = document.getElementById('new-pwd');
  i.type = i.type === 'password' ? 'text' : 'password';
}

function saveAuth() {
  const usr  = document.getElementById('new-usr').value.trim();
  const pwd  = document.getElementById('new-pwd').value;
  const pwd2 = document.getElementById('new-pwd2').value;
  const st   = document.getElementById('auth-status');
  const show = (msg, ok) => {
    st.style.display = 'block';
    st.style.background = ok ? '#052e16' : '#450a0a';
    st.style.color = ok ? '#4ade80' : '#fca5a5';
    st.textContent = msg;
  };
  if (!usr) return show('✗ Username tidak boleh kosong', false);
  if (pwd.length < 4) return show('✗ Password minimal 4 karakter', false);
  if (pwd !== pwd2) return show('✗ Konfirmasi password tidak cocok', false);
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type:'auth', usr, pwd })
  }).then(r=>r.json()).then(d=>{
    if (d.success) {
      show('✓ Kredensial berhasil diubah. Silakan login ulang.', true);
      document.getElementById('new-usr').value = '';
      document.getElementById('new-pwd').value = '';
      document.getElementById('new-pwd2').value = '';
      setTimeout(() => doLogout(), 2000);
    } else {
      show('✗ Gagal menyimpan', false);
    }
  }).catch(()=>show('✗ Koneksi terputus', false));
}

function doLogout() {
  fetch('/api/logout', {method:'POST'})
    .finally(() => { localStorage.removeItem('sm_token'); location.href = '/login'; });
}

// ============================================================
// LOKASI
// ============================================================
function getGPS() {
  const btn = document.getElementById('btn-gps');
  const bar = document.getElementById('loc-status-bar');

  if (!navigator.geolocation) {
    bar.style.display = 'block';
    bar.style.background = '#450a0a'; bar.style.color = '#fca5a5';
    bar.textContent = '✗ Browser tidak mendukung GPS. Isi koordinat manual.';
    return;
  }

  btn.textContent = '⏳ Mengambil lokasi...';
  btn.disabled = true;
  bar.style.display = 'none';

  navigator.geolocation.getCurrentPosition(
    (pos) => {
      btn.textContent = '📍 Ambil dari GPS Browser';
      btn.disabled = false;
      const lat = pos.coords.latitude;
      const lng = pos.coords.longitude;
      const alt = pos.coords.altitude || 0;
      document.getElementById('loc_lat').value = lat.toFixed(7);
      document.getElementById('loc_lng').value = lng.toFixed(7);
      document.getElementById('loc_alt').value = alt.toFixed(1);
      bar.style.display = 'block';
      bar.style.background = '#064e3b'; bar.style.color = '#6ee7b7';
      bar.textContent = '✓ GPS berhasil! Akurasi: ±' + Math.round(pos.coords.accuracy) + 'm. Klik Simpan Lokasi.';
    },
    (err) => {
      btn.textContent = '📍 Ambil dari GPS Browser';
      btn.disabled = false;
      bar.style.display = 'block';
      bar.style.background = '#451a03'; bar.style.color = '#fbbf24';
      const msg = {1:'Akses ditolak. Cek izin browser.',2:'Lokasi tidak tersedia.',3:'Timeout.'};
      bar.textContent = '⚠ GPS gagal: ' + (msg[err.code]||err.message) + ' — Isi koordinat manual.';
    },
    { enableHighAccuracy: true, timeout: 15000, maximumAge: 0 }
  );
}

function saveLoc() {
  const name = document.getElementById('loc_name').value.trim();
  const lat  = parseFloat(document.getElementById('loc_lat').value);
  const lng  = parseFloat(document.getElementById('loc_lng').value);
  const alt  = parseFloat(document.getElementById('loc_alt').value) || 0;

  if (!name) { toast('✗ Nama lokasi wajib diisi', false); return; }
  if (isNaN(lat)||isNaN(lng)) { toast('✗ Koordinat tidak valid', false); return; }

  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ type:'location', name, lat, lng, alt })
  }).then(r=>r.json()).then(d => {
    if (d.success) {
      toast('✓ Lokasi disimpan!');
      const bar = document.getElementById('loc-status-bar');
      bar.style.display = 'block';
      bar.style.background = '#064e3b'; bar.style.color = '#6ee7b7';
      bar.textContent = '✓ Lokasi: ' + name + ' ('+lat.toFixed(5)+', '+lng.toFixed(5)+')';
      const ml = document.getElementById('loc-map-link');
      ml.style.display = 'block';
      document.getElementById('loc-gmap').href = 'https://maps.google.com/?q='+lat+','+lng;
    } else { toast('✗ Gagal menyimpan', false); }
  }).catch(()=>toast('✗ Koneksi terputus', false));
}

// ============================================================
// MQTT
// ============================================================
function updateDataDesc(prefix) {
  const v = parseInt(document.getElementById(prefix+'_data').value) || 0;
  const labels = ['INA3221','BMP280','MPU/Gempa','Alarm','Lokasi'];
  const aktif  = labels.filter((_,i) => v & (1<<i));
  document.getElementById(prefix+'_data_desc').textContent =
    aktif.length ? '→ ' + aktif.join(', ') : '→ (tidak ada data)';
}
document.addEventListener('DOMContentLoaded', () => {
  ['ma_data','mb_data'].forEach(id => {
    const el = document.getElementById(id);
    if (el) el.addEventListener('input', () => updateDataDesc(id.slice(0,2)));
  });
});

function saveMqtt() {
  const payload = {
    type:    'mqtt',
    ma_en:   document.getElementById('ma_en').checked,
    ma_host: document.getElementById('ma_host').value.trim(),
    ma_port: parseInt(document.getElementById('ma_port').value) || 1883,
    ma_user: document.getElementById('ma_user').value.trim(),
    ma_pass: document.getElementById('ma_pass').value,
    ma_topic:document.getElementById('ma_topic').value.trim() || 'solar',
    ma_int:  parseInt(document.getElementById('ma_int').value)  || 10,
    ma_data: parseInt(document.getElementById('ma_data').value) || 31,
    mb_en:   document.getElementById('mb_en').checked,
    mb_host: document.getElementById('mb_host').value.trim(),
    mb_port: parseInt(document.getElementById('mb_port').value) || 1883,
    mb_user: document.getElementById('mb_user').value.trim(),
    mb_pass: document.getElementById('mb_pass').value,
    mb_topic:document.getElementById('mb_topic').value.trim() || 'solar',
    mb_int:  parseInt(document.getElementById('mb_int').value)  || 10,
    mb_data: parseInt(document.getElementById('mb_data').value) || 31,
  };

  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify(payload)
  }).then(r=>r.json()).then(d => {
    if (d.success) {
      toast('✓ MQTT disimpan & diterapkan!');
    } else { toast('✗ Gagal menyimpan MQTT', false); }
  }).catch(()=>toast('✗ Koneksi terputus', false));
}
</script>
</body>
</html>
)rawliteral";
