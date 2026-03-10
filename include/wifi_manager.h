#pragma once

// ============================================================
// SOLAR MONITOR — wifi_manager.h
// - Wizard setup: WiFi, IP static/DHCP, nama device
// - Web server aktif di WiFi mode DAN AP mode
// - OLED: WiFi status dulu, sensor belakangan
// ============================================================

#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "storage.h"
#include "oled.h"

DNSServer dnsServer;
AsyncWebServer server(80);

// ============================================================
// HTML WIZARD SETUP
// ============================================================
const char WIZARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Solar Monitor - Setup</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:Arial,sans-serif;background:#0f1923;color:#e0e0e0;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:16px}
.card{background:#1a2535;border-radius:12px;padding:24px;width:100%;max-width:420px;box-shadow:0 4px 24px rgba(0,0,0,.4)}
.logo{text-align:center;margin-bottom:20px}
.logo h1{font-size:20px;color:#f59e0b}
.logo p{font-size:12px;color:#94a3b8;margin-top:4px}
h2{font-size:14px;color:#f59e0b;margin:16px 0 10px;padding-bottom:6px;border-bottom:1px solid #2d3f55}
label{display:block;font-size:12px;color:#94a3b8;margin-bottom:3px;margin-top:10px}
input[type=text],input[type=password],select{width:100%;padding:9px 11px;background:#0f1923;border:1px solid #2d3f55;border-radius:7px;color:#e0e0e0;font-size:13px;outline:none}
input:focus,select:focus{border-color:#f59e0b}
.row{display:flex;gap:8px}
.row>div{flex:1}
.mode-group{display:flex;gap:8px;margin-top:4px}
.mode-btn{flex:1;padding:9px;border:2px solid #2d3f55;border-radius:7px;background:#0f1923;color:#94a3b8;font-size:12px;cursor:pointer;text-align:center;transition:all .2s}
.mode-btn.active{border-color:#f59e0b;color:#f59e0b;background:#1f2d3d}
.scan-list{margin-top:6px;max-height:150px;overflow-y:auto;border:1px solid #2d3f55;border-radius:7px;display:none}
.scan-item{padding:9px 11px;border-bottom:1px solid #2d3f55;cursor:pointer;display:flex;justify-content:space-between;font-size:12px;transition:background .15s}
.scan-item:last-child{border-bottom:none}
.scan-item:hover,.scan-item.selected{background:#1f2d3d;color:#f59e0b}
.signal{font-size:11px;color:#64748b}
.btn{width:100%;padding:11px;border:none;border-radius:7px;font-size:14px;font-weight:bold;cursor:pointer;margin-top:14px;transition:opacity .2s}
.btn:hover{opacity:.85}
.btn-primary{background:#f59e0b;color:#0f1923}
.btn-sm{background:#2d3f55;color:#e0e0e0;font-size:12px;padding:7px;margin-top:6px}
.status-box{margin-top:12px;padding:10px;border-radius:7px;font-size:12px;display:none}
.ok{background:#064e3b;color:#6ee7b7}
.err{background:#450a0a;color:#fca5a5}
.info{background:#1e3a5f;color:#93c5fd}
.spinner{display:inline-block;width:12px;height:12px;border:2px solid #f59e0b;border-top-color:transparent;border-radius:50%;animation:spin .8s linear infinite;margin-right:5px;vertical-align:middle}
@keyframes spin{to{transform:rotate(360deg)}}
.toggle-pass{display:flex;align-items:center;gap:5px;margin-top:5px;font-size:11px;color:#64748b;cursor:pointer}
.ip-section{display:none}
.hint{font-size:11px;color:#64748b;margin-top:3px}
</style>
</head>
<body>
<div class="card">
  <div class="logo">
    <h1>&#9728; Solar Monitor</h1>
    <p>Pengaturan Awal Perangkat</p>
  </div>

  <!-- NAMA DEVICE -->
  <h2>Nama Perangkat</h2>
  <label>Nama yang muncul di router dan web</label>
  <input type="text" id="devname" placeholder="Monitoring-Daya" maxlength="24" value="Monitoring-Daya">

  <!-- MODE KONEKSI -->
  <h2>Mode Koneksi</h2>
  <div class="mode-group">
    <div class="mode-btn active" id="btn-wifi" onclick="setMode('wifi')">&#128246; WiFi Router</div>
    <div class="mode-btn" id="btn-ap" onclick="setMode('ap')">&#128225; Local AP</div>
  </div>

  <!-- WIFI SECTION -->
  <div id="section-wifi">
    <label>Jaringan WiFi</label>
    <button class="btn btn-sm" onclick="scanWifi()" id="btn-scan">&#128268; Scan Jaringan</button>
    <div class="scan-list" id="scan-list"></div>
    <label>Nama WiFi (SSID)</label>
    <input type="text" id="ssid" placeholder="Nama jaringan">
    <label>Kata Sandi</label>
    <input type="password" id="pass" placeholder="Kata sandi WiFi">
    <label class="toggle-pass"><input type="checkbox" onchange="this.closest('label').previousElementSibling.type=this.checked?'text':'password'"> Tampilkan kata sandi</label>

    <!-- IP MODE -->
    <h2>Pengaturan IP</h2>
    <div class="mode-group">
      <div class="mode-btn active" id="btn-dhcp" onclick="setIP('dhcp')">DHCP (Otomatis)</div>
      <div class="mode-btn" id="btn-static" onclick="setIP('static')">IP Statis</div>
    </div>
    <div class="ip-section" id="ip-section">
      <div class="row">
        <div><label>IP Address</label><input type="text" id="ip" placeholder="192.168.1.100"></div>
        <div><label>Gateway</label><input type="text" id="gw" placeholder="192.168.1.1"></div>
      </div>
      <div class="row">
        <div><label>Subnet Mask</label><input type="text" id="sn" placeholder="255.255.255.0"></div>
        <div><label>DNS</label><input type="text" id="dns" placeholder="8.8.8.8"></div>
      </div>
    </div>
  </div>

  <!-- AP MODE INFO -->
  <div id="section-ap" style="display:none">
    <div class="status-box info" style="display:block;margin-top:10px">
      <b>Mode Local AP</b><br>
      ESP32 menjadi hotspot sendiri.<br>
      Konek ke <b>WifiMonitor-Setup</b><br>
      Buka browser: <b>192.168.4.1</b>
    </div>
  </div>

  <div id="status-box" class="status-box"></div>
  <button class="btn btn-primary" onclick="simpan()" id="btn-simpan">Simpan &amp; Mulai</button>
</div>

<script>
let selMode='wifi', selIP='dhcp';

function setMode(m){
  selMode=m;
  ['wifi','ap'].forEach(x=>{
    document.getElementById('btn-'+x).className='mode-btn'+(m===x?' active':'');
  });
  document.getElementById('section-wifi').style.display=m==='wifi'?'block':'none';
  document.getElementById('section-ap').style.display=m==='ap'?'block':'none';
}

function setIP(m){
  selIP=m;
  ['dhcp','static'].forEach(x=>{
    document.getElementById('btn-'+x).className='mode-btn'+(m===x?' active':'');
  });
  document.getElementById('ip-section').style.display=m==='static'?'block':'none';
}

function scanWifi(){
  const btn=document.getElementById('btn-scan');
  btn.innerHTML='<span class="spinner"></span>Memindai...';
  btn.disabled=true;
  fetch('/api/scan').then(r=>r.json()).then(d=>{
    const list=document.getElementById('scan-list');
    list.style.display='block';
    list.innerHTML='';
    if(!d.networks.length){list.innerHTML='<div class="scan-item">Tidak ada jaringan</div>';return;}
    d.networks.forEach(n=>{
      const div=document.createElement('div');
      div.className='scan-item';
      div.innerHTML=`<span>${n.ssid}</span><span class="signal">${n.rssi}dBm</span>`;
      div.onclick=()=>{
        document.getElementById('ssid').value=n.ssid;
        list.querySelectorAll('.scan-item').forEach(i=>i.classList.remove('selected'));
        div.classList.add('selected');
      };
      list.appendChild(div);
    });
    btn.innerHTML='&#128268; Scan Ulang';
    btn.disabled=false;
  }).catch(()=>{btn.innerHTML='&#128268; Scan Jaringan';btn.disabled=false;});
}

function showSt(msg,cls){
  const b=document.getElementById('status-box');
  b.className='status-box '+cls;b.innerHTML=msg;b.style.display='block';
}

function simpan(){
  const btn=document.getElementById('btn-simpan');
  const devname=document.getElementById('devname').value.trim()||'Monitoring-Daya';

  if(selMode==='ap'){
    btn.disabled=true;btn.innerHTML='<span class="spinner"></span>Menyimpan...';
    fetch('/api/save',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({mode:'ap',devname,ssid:'',password:'',ip_mode:'dhcp'})})
    .then(r=>r.json()).then(()=>{
      showSt('&#10003; Tersimpan! Restart dalam 3 detik.','ok');
      btn.innerHTML='Simpan & Mulai';btn.disabled=false;
    });
    return;
  }

  const ssid=document.getElementById('ssid').value.trim();
  const pass=document.getElementById('pass').value;
  if(!ssid){showSt('&#9888; Nama WiFi tidak boleh kosong.','err');return;}

  const payload={mode:'wifi',devname,ssid,password:pass,ip_mode:selIP};
  if(selIP==='static'){
    payload.ip  =document.getElementById('ip').value.trim();
    payload.gw  =document.getElementById('gw').value.trim();
    payload.sn  =document.getElementById('sn').value.trim();
    payload.dns =document.getElementById('dns').value.trim();
    if(!payload.ip||!payload.gw){showSt('&#9888; IP Address dan Gateway wajib diisi.','err');return;}
  }

  btn.disabled=true;btn.innerHTML='<span class="spinner"></span>Menyimpan...';
  fetch('/api/save',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payload)})
  .then(r=>r.json()).then(d=>{
    if(d.success)showSt('&#10003; Tersimpan! Perangkat akan konek ke <b>'+ssid+'</b> dan restart.','ok');
    else showSt('&#10007; Gagal menyimpan.','err');
    btn.innerHTML='Simpan & Mulai';btn.disabled=false;
  }).catch(()=>{showSt('&#10007; Koneksi terputus.','err');btn.innerHTML='Simpan & Mulai';btn.disabled=false;});
}
</script>
</body>
</html>
)rawliteral";

// ============================================================
// STORAGE TAMBAHAN — IP static (tambah di storage.h nanti)
// ============================================================
// Key: "ip_mode" (dhcp/static), "ip_addr", "ip_gw", "ip_sn", "ip_dns"

class WifiManager {
public:
  bool   isConnected;
  bool   isAPMode;
  bool   isSetupMode;
  String ipAddress;

  void begin() {
    isConnected = false;
    isAPMode    = false;
    isSetupMode = false;

    String mode = storage.getMode();

    if (mode == MODE_AP) {
      _startLocalAP();
      _startWebServer();
      return;
    }

    if (!storage.hasWifiCredentials()) {
      _startSetupAP();
      return;
    }

    // OLED: tampil "Konek WiFi..."
    oledDisp.showWifiConnecting(storage.getWifiSSID(), 0);
    _connectWifi(storage.getWifiSSID(), storage.getWifiPass());

    if (isConnected) {
      // Update info OLED
      oledDisp.setWifiInfo("WiFi", storage.getWifiSSID(), ipAddress, true);
      // Mulai web server
      _startWebServer();
    } else {
      _startSetupAP();
    }
  }

  void handle() {
    if (isAPMode) dnsServer.processNextRequest();
  }

private:

  void _connectWifi(String ssid, String pass) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(storage.getDeviceName().c_str());

    // IP Static jika dikonfigurasi
    String ipMode = storage.getIPMode();
    if (ipMode == "static") {
      IPAddress ip, gw, sn, dns;
      ip.fromString(storage.getStaticIP());
      gw.fromString(storage.getGateway());
      sn.fromString(storage.getSubnet());
      dns.fromString(storage.getDNS());
      WiFi.config(ip, gw, sn, dns);
      Serial.println("[WiFi] IP Static: " + storage.getStaticIP());
    }

    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("[WiFi] Konek ke: " + ssid);

    unsigned long start = millis();
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - start >= WIFI_TIMEOUT_MS) {
        Serial.println("\n[WiFi] Timeout!");
        return;
      }
      oledDisp.showWifiConnecting(ssid, attempt++);
      delay(WIFI_RETRY_DELAY);
      Serial.print(".");
    }

    isConnected = true;
    ipAddress   = WiFi.localIP().toString();
    Serial.println("\n[WiFi] Terhubung! IP: " + ipAddress);

    // Tampil IP di OLED sebentar
    oledDisp.showWifiConnected(ipAddress);
  }

  void _startSetupAP() {
    isAPMode    = true;
    isSetupMode = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPsetHostname(storage.getDeviceName().c_str());
    dnsServer.start(53, "*", WiFi.softAPIP());
    ipAddress = WiFi.softAPIP().toString();

    oledDisp.setWifiInfo("Setup", AP_SSID, ipAddress, true);
    oledDisp.showAPMode();

    Serial.println("[AP] Setup Mode: " + String(AP_SSID));
    _setupWizardRoutes();
    server.begin();
  }

  void _startLocalAP() {
    isAPMode    = true;
    isSetupMode = false;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPsetHostname(storage.getDeviceName().c_str());
    dnsServer.start(53, "*", WiFi.softAPIP());
    ipAddress = WiFi.softAPIP().toString();

    oledDisp.setWifiInfo("AP", AP_SSID, ipAddress, true);
    Serial.println("[AP] Local AP Mode: " + ipAddress);
  }

  void _startWebServer() {
    _setupWizardRoutes();
    // Route root → halaman wizard/setup jika belum setup
    // (Tahap 4: tambah route dashboard di sini)
    server.begin();
    Serial.println("[Web] Server aktif di " + ipAddress);
  }

  void _setupWizardRoutes() {
    // Halaman wizard
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
      req->send(200, "text/html", WIZARD_HTML);
    });
    server.on("/setup", HTTP_GET, [](AsyncWebServerRequest* req) {
      req->send(200, "text/html", WIZARD_HTML);
    });

    // Captive portal redirect
    server.onNotFound([](AsyncWebServerRequest* req) {
      req->redirect("http://192.168.4.1/");
    });

    // API Scan WiFi
    server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest* req) {
      int n = WiFi.scanNetworks();
      String json = "{\"networks\":[";
      for (int i = 0; i < n; i++) {
        if (i > 0) json += ",";
        // Escape SSID sederhana
        String ssid = WiFi.SSID(i);
        ssid.replace("\"", "\\\"");
        json += "{\"ssid\":\"" + ssid + "\",\"rssi\":" + WiFi.RSSI(i) + "}";
      }
      json += "]}";
      req->send(200, "application/json", json);
    });

    // API Simpan setting
    AsyncCallbackJsonWebHandler* saveHandler =
      new AsyncCallbackJsonWebHandler("/api/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
          String mode    = json["mode"].as<String>();
          String devname = json["devname"].as<String>();
          String ssid    = json["ssid"].as<String>();
          String pass    = json["password"].as<String>();
          String ipMode  = json["ip_mode"].as<String>();

          if (devname.length() > 0) storage.setDeviceName(devname);
          storage.setMode(mode);

          if (mode == MODE_WIFI) {
            storage.setWifiCredentials(ssid, pass);
            storage.setIPMode(ipMode);
            if (ipMode == "static") {
              storage.setStaticIP(
                json["ip"].as<String>(),
                json["gw"].as<String>(),
                json["sn"].as<String>(),
                json["dns"].as<String>()
              );
            }
          }
          storage.setSetupDone(true);

          req->send(200, "application/json", "{\"success\":true}");
          delay(2000);
          ESP.restart();
        });
    server.addHandler(saveHandler);

    // API Status (untuk Tahap 4 dashboard)
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
      // Placeholder — akan dikembangkan di Tahap 4
      req->send(200, "application/json", "{\"ok\":true}");
    });
  }

  // Helper: tampil IP setelah konek
  void showWifiConnected(String ip) {
    oledDisp.showStatus(
      "WiFi Terhubung!",
      "IP: " + ip,
      "Buka browser:"
    );
    delay(2500);
  }
};

WifiManager wifiManager;