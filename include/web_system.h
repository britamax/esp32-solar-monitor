#pragma once

// ============================================================
// SOLAR MONITOR — web_system.h
// Halaman info sistem ESP32
// Akses: http://[IP]/system
// ============================================================

const char SYSTEM_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Solar Monitor - System</title>
<style>
:root{
  --bg:#0f1923;--card:#1a2535;--border:#2d3f55;--text:#e0e0e0;
  --dim:#94a3b8;--accent:#f59e0b;--accent2:#38bdf8;
  --green:#22c55e;--red:#ef4444;--yellow:#facc15;
  --sans:'Segoe UI',Arial,sans-serif;--mono:'Courier New',monospace
}
*{box-sizing:border-box;margin:0;padding:0}
body{background:var(--bg);color:var(--text);font-family:var(--sans);min-height:100vh;padding-top:0}
nav{background:#111d2b;padding:12px 20px;display:flex;align-items:center;
  justify-content:space-between;border-bottom:1px solid var(--border);
  position:sticky;top:0;z-index:100}
.nav-left{display:flex;align-items:center;gap:12px}
.logo{font-size:16px;font-weight:bold;color:var(--accent)}
.badge{font-family:var(--mono);font-size:10px;padding:2px 8px;border-radius:4px;
  background:#1a2e42;color:var(--accent2);letter-spacing:1px}
.nav-tabs{display:flex;gap:4px}
.tab{padding:6px 14px;border-radius:6px;font-size:13px;cursor:pointer;border:none;
  background:transparent;color:#94a3b8;transition:all .2s}
.tab.active{background:var(--accent);color:#0f1923;font-weight:bold}
.tab:hover:not(.active){background:#1e3a5f;color:var(--text)}
.conn-dot{width:8px;height:8px;border-radius:50%;background:var(--green);
  box-shadow:0 0 6px var(--green);animation:pulse 2s infinite;display:inline-block}
.conn-dot.off{background:var(--red);box-shadow:0 0 6px var(--red);animation:none}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}
.uptime{font-family:var(--mono);font-size:11px;color:var(--dim)}
/* CONTENT */
.content{padding:16px;max-width:860px;margin:0 auto}
.page-title{font-size:18px;font-weight:bold;color:var(--accent);margin-bottom:16px;
  display:flex;align-items:center;gap:8px}
/* GRID */
.grid-2{display:grid;grid-template-columns:repeat(2,1fr);gap:10px;margin-bottom:10px}
.grid-3{display:grid;grid-template-columns:repeat(3,1fr);gap:10px;margin-bottom:10px}
@media(max-width:500px){.grid-2,.grid-3{grid-template-columns:1fr}}
/* CARD */
.card{background:var(--card);border-radius:10px;padding:14px 16px;
  border-top:2px solid var(--border);position:relative;overflow:hidden}
.card.accent{border-top-color:var(--accent)}
.card.blue{border-top-color:var(--accent2)}
.card.green{border-top-color:var(--green)}
.card.red{border-top-color:var(--red)}
.card.yellow{border-top-color:var(--yellow)}
.card.purple{border-top-color:#a855f7}
.card-label{font-size:10px;color:var(--dim);letter-spacing:1px;text-transform:uppercase;margin-bottom:6px}
.card-val{font-size:26px;font-weight:bold;font-family:var(--mono);color:var(--text)}
.card-val span{font-size:13px;color:var(--dim);margin-left:4px;font-family:var(--sans)}
.card-sub{font-size:11px;color:var(--dim);margin-top:4px}
/* PROGRESS BAR */
.bar-wrap{background:#0a1520;border-radius:4px;height:6px;margin-top:8px;overflow:hidden}
.bar-fill{height:100%;border-radius:4px;transition:width .5s}
.bar-green{background:var(--green)}
.bar-yellow{background:var(--yellow)}
.bar-red{background:var(--red)}
/* STATUS BADGE */
.status-badge{display:inline-flex;align-items:center;gap:5px;padding:3px 9px;
  border-radius:12px;font-size:11px;font-weight:bold}
.status-badge.ok{background:#052e16;color:var(--green)}
.status-badge.warn{background:#422006;color:var(--yellow)}
.status-badge.err{background:#450a0a;color:var(--red)}
/* DIVIDER */
.divider{border:none;border-top:1px solid var(--border);margin:14px 0}
/* OTA */
.ota-card{background:var(--card);border-radius:10px;padding:16px;margin-bottom:10px}
.ota-title{font-size:13px;font-weight:bold;color:var(--text);margin-bottom:4px}
.ota-sub{font-size:11px;color:var(--dim);margin-bottom:12px}
.drop-zone{border:2px dashed var(--border);border-radius:8px;padding:20px;text-align:center;
  cursor:pointer;transition:all .2s;position:relative}
.drop-zone:hover,.drop-zone.drag{border-color:var(--accent);background:#1a2e1a}
.drop-zone input{position:absolute;inset:0;opacity:0;cursor:pointer;width:100%;height:100%}
.drop-icon{font-size:28px;margin-bottom:6px}
.drop-text{font-size:13px;color:var(--dim)}
.drop-text span{color:var(--accent);font-weight:bold}
.file-info{display:none;margin-top:8px;font-size:12px;color:var(--accent2);
  font-family:var(--mono);padding:6px 10px;background:#0a1520;border-radius:6px}
.progress-wrap{display:none;margin-top:12px}
.progress-bar{height:8px;background:#0a1520;border-radius:4px;overflow:hidden}
.progress-fill{height:100%;background:var(--accent);border-radius:4px;
  width:0%;transition:width .3s}
.progress-text{font-size:11px;color:var(--dim);margin-top:4px;text-align:center}
.btn-ota{width:100%;margin-top:12px;padding:10px;background:var(--accent);border:none;
  border-radius:7px;color:#0f1923;font-size:13px;font-weight:bold;cursor:pointer;
  transition:all .2s;display:none}
.btn-ota:hover{background:#fbbf24}
.btn-ota:disabled{background:#374151;color:#6b7280;cursor:not-allowed}
.ota-status{display:none;margin-top:10px;padding:10px 12px;border-radius:7px;
  font-size:13px;font-weight:bold;text-align:center}
  display:flex;align-items:center;justify-content:space-between;margin-bottom:10px}
.btn-restart{padding:8px 20px;background:#450a0a;border:1px solid var(--red);
  border-radius:7px;color:var(--red);font-size:13px;font-weight:bold;cursor:pointer;transition:all .2s}
.btn-restart:hover{background:var(--red);color:#fff}
.btn-logout{padding:8px 20px;background:#1a2535;border:1px solid var(--border);
  border-radius:7px;color:var(--dim);font-size:13px;cursor:pointer;transition:all .2s}
.btn-logout:hover{border-color:var(--accent);color:var(--accent)}
/* REFRESH badge */
.refresh-info{font-size:11px;color:var(--dim);text-align:right;margin-bottom:10px}
</style>
</head>
<body>
<nav>
  <div class="nav-left">
    <div style="display:flex;align-items:center;gap:8px">
      <div class="logo">&#9728; SolarMonitor</div>
      <div class="badge">SYSTEM</div>
    </div>
    <div class="nav-tabs">
      <button class="tab" onclick="location.href='/'">Dashboard</button>
      <button class="tab" onclick="location.href='/setting'">Setting</button>
      <button class="tab active">System</button>
      <button class="tab" onclick="location.href='/log'">Log</button>
    </div>
  </div>
  <div style="display:flex;align-items:center;gap:6px">
    <span class="uptime" id="uptime">--:--:--</span>
    <span class="conn-dot" id="conn-dot"></span>
  </div>
</nav>

<div class="content">
  <div class="refresh-info">Auto refresh tiap 5 detik &nbsp;&#128260;</div>

  <!-- BARIS 1: RAM, Suhu, RSSI -->
  <div class="grid-3">
    <div class="card blue" id="card-ram">
      <div class="card-label">RAM Bebas</div>
      <div class="card-val" id="ram-free">--<span>KB</span></div>
      <div class="card-sub" id="ram-pct">dari -- KB total</div>
      <div class="bar-wrap"><div class="bar-fill bar-green" id="ram-bar" style="width:0%"></div></div>
    </div>
    <div class="card accent" id="card-temp">
      <div class="card-label">Suhu Chip ESP32</div>
      <div class="card-val" id="chip-temp">--<span>°C</span></div>
      <div class="card-sub" id="chip-temp-status">--</div>
    </div>
    <div class="card green" id="card-rssi">
      <div class="card-label">Sinyal WiFi (RSSI)</div>
      <div class="card-val" id="rssi-val">--<span>dBm</span></div>
      <div class="card-sub" id="rssi-status">--</div>
      <div class="bar-wrap"><div class="bar-fill bar-green" id="rssi-bar" style="width:0%"></div></div>
    </div>
  </div>

  <!-- BARIS 2: CPU, Flash, Uptime -->
  <div class="grid-3">
    <div class="card purple">
      <div class="card-label">CPU Frequency</div>
      <div class="card-val" id="cpu-freq">--<span>MHz</span></div>
      <div class="card-sub">Espressif ESP32</div>
    </div>
    <div class="card yellow">
      <div class="card-label">Flash Tersisa (OTA)</div>
      <div class="card-val" id="flash-free">--<span>KB</span></div>
      <div class="card-sub" id="flash-status">--</div>
    </div>
    <div class="card green">
      <div class="card-label">Uptime</div>
      <div class="card-val" style="font-size:20px" id="uptime-sys">--:--:--</div>
      <div class="card-sub" id="uptime-days">-- hari</div>
    </div>
  </div>

  <!-- BARIS 3: IP, MAC, Hostname -->
  <div class="grid-2">
    <div class="card">
      <div class="card-label">Info Jaringan</div>
      <table style="width:100%;font-size:12px;border-collapse:collapse;margin-top:6px">
        <tr><td style="color:var(--dim);padding:3px 0;width:90px">IP Address</td>
            <td style="font-family:var(--mono);color:var(--accent2)" id="net-ip">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">Gateway</td>
            <td style="font-family:var(--mono)" id="net-gw">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">MAC Address</td>
            <td style="font-family:var(--mono);font-size:11px" id="net-mac">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">Hostname</td>
            <td style="font-family:var(--mono)" id="net-host">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">SSID</td>
            <td style="font-family:var(--mono)" id="net-ssid">--</td></tr>
      </table>
    </div>
    <div class="card">
      <div class="card-label">Info Firmware</div>
      <table style="width:100%;font-size:12px;border-collapse:collapse;margin-top:6px">
        <tr><td style="color:var(--dim);padding:3px 0;width:90px">Versi</td>
            <td style="font-family:var(--mono);color:var(--accent)" id="fw-ver">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">Build</td>
            <td style="font-family:var(--mono);font-size:11px;color:var(--accent2)" id="fw-build">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">Chip Model</td>
            <td style="font-family:var(--mono)" id="fw-chip">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">Flash Size</td>
            <td style="font-family:var(--mono)" id="fw-flash">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">MQTT A</td>
            <td id="fw-mqtt-a">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">MQTT B</td>
            <td id="fw-mqtt-b">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">Waktu</td>
            <td style="font-family:var(--mono);font-size:11px" id="fw-ntp-time">--</td></tr>
        <tr><td style="color:var(--dim);padding:3px 0">NTP</td>
            <td id="fw-ntp-status">--</td></tr>
      </table>
    </div>
  </div>

  <hr class="divider">

  <!-- OTA UPDATE -->
  <div class="ota-card">
    <div class="ota-title">&#128640; OTA Firmware Update</div>
    <div class="ota-sub">Upload file <code>.bin</code> dari PlatformIO → <code>.pio/build/lolin32_lite/firmware.bin</code></div>

    <div class="drop-zone" id="drop-zone" ondragover="onDrag(event,true)" ondragleave="onDrag(event,false)" ondrop="onDrop(event)">
      <input type="file" id="ota-file" accept=".bin" onchange="onFileSelect(this)">
      <div class="drop-icon">&#128190;</div>
      <div class="drop-text">Drag & drop firmware.bin atau <span>klik untuk pilih</span></div>
    </div>
    <div class="file-info" id="file-info"></div>

    <div class="progress-wrap" id="progress-wrap">
      <div class="progress-bar"><div class="progress-fill" id="progress-fill"></div></div>
      <div class="progress-text" id="progress-text">0%</div>
    </div>

    <div class="ota-status" id="ota-status"></div>
    <button class="btn-ota" id="btn-ota" onclick="doOTA()">&#9989; Flash Firmware</button>
  </div>

  <!-- ROLLBACK -->
  <div class="restart-card">
    <div>
      <div style="font-size:13px;font-weight:bold;color:var(--text)">&#8617; Rollback Firmware</div>
      <div style="font-size:11px;color:var(--dim);margin-top:2px">Kembali ke firmware sebelumnya (slot OTA lama)</div>
    </div>
    <button class="btn-restart" onclick="doRollback()" style="background:#1a1a2e;border-color:#a855f7;color:#a855f7">&#8617; Rollback</button>
  </div>

  <hr class="divider">

  <!-- AKSI -->
  <div class="restart-card">
    <div>
      <div style="font-size:13px;font-weight:bold;color:var(--text)">Restart Perangkat</div>
      <div style="font-size:11px;color:var(--dim);margin-top:2px">Restart ESP32 tanpa menghapus data & setting</div>
    </div>
    <button class="btn-restart" onclick="doRestart()">&#9889; Restart</button>
  </div>
  <div class="restart-card">
    <div>
      <div style="font-size:13px;font-weight:bold;color:var(--text)">Logout</div>
      <div style="font-size:11px;color:var(--dim);margin-top:2px">Keluar dari sesi ini</div>
    </div>
    <button class="btn-logout" onclick="doLogout()">&#128274; Logout</button>
  </div>
</div>

<script>
function fmtUptime(s) {
  const h = Math.floor(s/3600), m = Math.floor((s%3600)/60), ss = s%60;
  return String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(ss).padStart(2,'0');
}
function rssiLabel(r) {
  if (r >= -50) return ['Sangat Kuat','bar-green'];
  if (r >= -65) return ['Kuat','bar-green'];
  if (r >= -75) return ['Sedang','bar-yellow'];
  if (r >= -85) return ['Lemah','bar-yellow'];
  return ['Sangat Lemah','bar-red'];
}
function rssiPct(r) { return Math.max(0, Math.min(100, 2*(r+100))); }
function tempLabel(t) {
  if (t < 60) return ['Normal','ok'];
  if (t < 75) return ['Hangat','warn'];
  return ['Panas!','err'];
}
function ramBarClass(pct) {
  if (pct > 60) return 'bar-green';
  if (pct > 30) return 'bar-yellow';
  return 'bar-red';
}
function mqttBadge(ok) {
  return ok
    ? '<span class="status-badge ok">&#9679; Terhubung</span>'
    : '<span class="status-badge err">&#9679; Terputus</span>';
}

function update() {
  fetch('/api/system')
    .then(r => r.json())
    .then(d => {
      // RAM
      const ramFreeKB  = Math.round(d.ram_free / 1024);
      const ramTotalKB = Math.round(d.ram_total / 1024);
      const ramPct     = Math.round(d.ram_free / d.ram_total * 100);
      document.getElementById('ram-free').innerHTML = ramFreeKB + '<span>KB</span>';
      document.getElementById('ram-pct').textContent = 'dari ' + ramTotalKB + ' KB total';
      const rb = document.getElementById('ram-bar');
      rb.style.width = ramPct + '%';
      rb.className = 'bar-fill ' + ramBarClass(ramPct);

      // Suhu chip
      const ct    = d.chip_temp.toFixed(1);
      const [tl, tc] = tempLabel(d.chip_temp);
      document.getElementById('chip-temp').innerHTML = ct + '<span>°C</span>';
      document.getElementById('chip-temp-status').innerHTML =
        '<span class="status-badge ' + tc + '">' + tl + '</span>';

      // RSSI
      const [rl, rc] = rssiLabel(d.rssi);
      document.getElementById('rssi-val').innerHTML = d.rssi + '<span>dBm</span>';
      document.getElementById('rssi-status').innerHTML =
        '<span class="status-badge ' + (d.rssi >= -75 ? 'ok' : 'warn') + '">' + rl + '</span>';
      const rssiB = document.getElementById('rssi-bar');
      rssiB.style.width = rssiPct(d.rssi) + '%';
      rssiB.className = 'bar-fill ' + rc;

      // CPU
      document.getElementById('cpu-freq').innerHTML = d.cpu_mhz + '<span>MHz</span>';

      // Flash
      const flashKB = Math.round(d.flash_free / 1024);
      document.getElementById('flash-free').innerHTML = flashKB + '<span>KB</span>';
      document.getElementById('flash-status').textContent =
        flashKB > 512 ? 'Cukup untuk OTA' : 'Mepet untuk OTA';

      // Uptime
      const s   = parseInt(d.uptime) || 0;
      const day = Math.floor(s / 86400);
      document.getElementById('uptime-sys').textContent = fmtUptime(s % 86400);
      document.getElementById('uptime-days').textContent = day + ' hari berjalan';
      document.getElementById('uptime').textContent = fmtUptime(s);

      // Jaringan
      document.getElementById('net-ip').textContent   = d.ip   || '--';
      document.getElementById('net-gw').textContent   = d.gw   || '--';
      document.getElementById('net-mac').textContent  = d.mac  || '--';
      document.getElementById('net-host').textContent = d.host || '--';
      document.getElementById('net-ssid').textContent = d.ssid || '--';

      // Firmware
      document.getElementById('fw-ver').textContent   = d.version  || '--';
      document.getElementById('fw-build').textContent = d.build    || '--';
      document.getElementById('fw-chip').textContent  = d.chip     || 'ESP32';
      document.getElementById('fw-flash').textContent = Math.round(d.flash_total/1024) + ' KB';
      document.getElementById('fw-mqtt-a').innerHTML  = mqttBadge(d.mqtt_a);
      document.getElementById('fw-mqtt-b').innerHTML  = mqttBadge(d.mqtt_b);
      document.getElementById('fw-ntp-time').textContent = d.ntp_time || '--';
      document.getElementById('fw-ntp-status').innerHTML = d.ntp_synced
        ? '<span class="status-badge ok">&#9679; Tersync</span>'
        : '<span class="status-badge err">&#9679; Belum sync</span>';

      document.getElementById('conn-dot').className = 'conn-dot';
    })
    .catch(() => document.getElementById('conn-dot').className = 'conn-dot off');
}

function doRestart() {
  if (!confirm('Restart perangkat sekarang?')) return;
  fetch('/api/setting', { method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({type:'restart'})
  }).then(() => alert('Restart dalam 3 detik...'));
}

function doLogout() {
  fetch('/api/logout', {method:'POST'})
    .finally(() => { localStorage.removeItem('sm_token'); location.href = '/login'; });
}

// ============================================================
// OTA Functions
// ============================================================
let _otaFile = null;

function onDrag(e, active) {
  e.preventDefault();
  document.getElementById('drop-zone').classList.toggle('drag', active);
}

function onDrop(e) {
  e.preventDefault();
  document.getElementById('drop-zone').classList.remove('drag');
  const file = e.dataTransfer.files[0];
  if (file) setFile(file);
}

function onFileSelect(input) {
  if (input.files[0]) setFile(input.files[0]);
}

function setFile(file) {
  if (!file.name.endsWith('.bin')) {
    showOtaStatus('✗ File harus berekstensi .bin', false);
    return;
  }
  _otaFile = file;
  const info = document.getElementById('file-info');
  info.style.display = 'block';
  info.textContent = file.name + ' — ' + (file.size/1024).toFixed(1) + ' KB';
  document.getElementById('btn-ota').style.display = 'block';
  document.getElementById('ota-status').style.display = 'none';
}

function doOTA() {
  if (!_otaFile) return;
  if (!confirm('Upload dan flash firmware baru?\n\nPerangkat akan restart otomatis setelah selesai.\nPastikan koneksi WiFi stabil.')) return;

  const btn  = document.getElementById('btn-ota');
  const wrap = document.getElementById('progress-wrap');
  const fill = document.getElementById('progress-fill');
  const txt  = document.getElementById('progress-text');

  btn.disabled = true;
  btn.textContent = 'Uploading...';
  wrap.style.display = 'block';
  showOtaStatus('', null);

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/api/ota', true);

  xhr.upload.onprogress = (e) => {
    if (e.lengthComputable) {
      const pct = Math.round(e.loaded / e.total * 100);
      fill.style.width = pct + '%';
      txt.textContent  = pct + '% — ' + (e.loaded/1024).toFixed(0) + ' / ' + (e.total/1024).toFixed(0) + ' KB';
    }
  };

  xhr.onload = () => {
    btn.disabled = false;
    btn.textContent = '✅ Flash Firmware';
    if (xhr.status === 200) {
      fill.style.width = '100%';
      fill.style.background = 'var(--green)';
      txt.textContent = '100% — Selesai!';
      showOtaStatus('✓ Upload berhasil! Perangkat restart dalam 3 detik...', true);
      btn.style.display = 'none';
      setTimeout(() => { location.href = '/'; }, 8000);
    } else {
      showOtaStatus('✗ Upload gagal: ' + xhr.responseText, false);
    }
  };

  xhr.onerror = () => {
    btn.disabled = false;
    btn.textContent = '✅ Flash Firmware';
    showOtaStatus('✗ Koneksi terputus saat upload', false);
  };

  const formData = new FormData();
  formData.append('firmware', _otaFile, _otaFile.name);
  xhr.send(formData);
}

function doRollback() {
  if (!confirm('Rollback ke firmware sebelumnya?\n\nPerangkat akan restart dengan firmware lama.')) return;
  fetch('/api/ota/rollback', {method:'POST'})
    .then(r => r.json())
    .then(d => {
      if (d.success) alert('Rollback berhasil! Restart dalam 3 detik...');
      else alert('Rollback gagal: ' + (d.error || 'tidak ada firmware lama'));
    })
    .catch(() => alert('Koneksi terputus'));
}

function showOtaStatus(msg, ok) {
  const el = document.getElementById('ota-status');
  if (!msg) { el.style.display = 'none'; return; }
  el.style.display = 'block';
  el.style.background = ok ? '#052e16' : '#450a0a';
  el.style.color = ok ? '#4ade80' : '#fca5a5';
  el.textContent = msg;
}

update();
setInterval(update, 5000);
</script>
</body>
</html>
)rawliteral";
