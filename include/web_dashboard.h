#pragma once

// ============================================================
// SOLAR MONITOR — web_dashboard.h
// v2.0.0: Web Dashboard — data INA3221 realtime
// Update data via polling /api/data tiap 1 detik
// Tema gelap, layout ringkas
// ============================================================

#include "config.h"
#include "ina3221.h"
#include "bmp280.h"
#include "mpu6050.h"
#include "storage.h"

// ============================================================
// HTML DASHBOARD — PROGMEM
// ============================================================
const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Solar Monitor</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Saira:wght@300;400;600;700&display=swap');

:root {
  --bg:      #0f1923;
  --card:    #1a2535;
  --border:  #2d3f55;
  --accent:  #f59e0b;
  --accent2: #38bdf8;
  --green:   #22c55e;
  --red:     #ef4444;
  --dim:     #94a3b8;
  --text:    #e0e0e0;
  --mono:    'Share Tech Mono', monospace;
  --sans:    'Saira', sans-serif;
}

* { box-sizing: border-box; margin: 0; padding: 0; }

body {
  background: var(--bg);
  color: var(--text);
  font-family: var(--sans);
  min-height: 100vh;
  padding: 16px;
  padding-top: 0;
}

/* HEADER */
/* NAV — sama persis dengan setting */
nav {
  background: #111d2b;
  padding: 12px 20px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  border-bottom: 1px solid var(--border);
  position: sticky;
  top: 0;
  z-index: 100;
}
.nav-left { display: flex; align-items: center; gap: 12px; }
.logo { font-size: 16px; font-weight: bold; color: var(--accent); }
.badge {
  font-family: var(--mono);
  font-size: 10px;
  padding: 2px 8px;
  border-radius: 4px;
  background: #1a2e42;
  color: var(--accent2);
  letter-spacing: 1px;
}
.nav-tabs { display: flex; gap: 4px; }
.tab {
  padding: 6px 14px;
  border-radius: 6px;
  font-size: 13px;
  cursor: pointer;
  border: none;
  background: transparent;
  color: #94a3b8;
  transition: all .2s;
}
.tab.active { background: var(--accent); color: #0f1923; font-weight: bold; }
.tab:hover:not(.active) { background: #1e3a5f; color: var(--text); }
.conn-dot {
  width: 8px; height: 8px;
  border-radius: 50%;
  background: var(--green);
  box-shadow: 0 0 6px var(--green);
  animation: pulse 2s infinite;
  display: inline-block;
  margin-left: 4px;
}
.conn-dot.off { background: var(--red); box-shadow: 0 0 6px var(--red); animation: none; }
@keyframes pulse { 0%,100%{opacity:1} 50%{opacity:.4} }
.uptime { font-family: var(--mono); font-size: 11px; color: var(--dim); }

/* GRID */
.grid-3 { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-bottom: 10px; }
.grid-2 { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; margin-bottom: 10px; }
.grid-1 { margin-bottom: 10px; }

@media(max-width: 700px) {
  .grid-3 { grid-template-columns: 1fr; }
  .grid-2 { grid-template-columns: 1fr; }
}
@media(min-width: 701px) and (max-width: 1000px) {
  .grid-3 { grid-template-columns: repeat(2, 1fr); }
}

/* CARD */
.card {
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 10px;
  padding: 14px 16px;
  position: relative;
  overflow: hidden;
}
.card::before {
  content: '';
  position: absolute;
  top: 0; left: 0; right: 0;
  height: 2px;
  background: var(--border);
}
.card.ch1::before { background: var(--accent); }
.card.ch2::before { background: var(--green); }
.card.ch3::before { background: var(--accent2); }
.card.summary::before { background: linear-gradient(90deg, var(--accent), var(--accent2)); }
.card.env::before { background: #a78bfa; }

.card-title {
  font-size: 10px;
  font-weight: 600;
  letter-spacing: 2px;
  text-transform: uppercase;
  color: var(--dim);
  margin-bottom: 10px;
  display: flex;
  align-items: center;
  justify-content: space-between;
}
.card-title .tag {
  font-family: var(--mono);
  font-size: 9px;
  padding: 1px 6px;
  border-radius: 3px;
  letter-spacing: 1px;
}
.tag-on  { background: #14532d; color: var(--green); }
.tag-off { background: #450a0a; color: var(--red); }
.tag-chg { background: #14532d; color: var(--green); }
.tag-dis { background: #450a0a; color: var(--red); }
.tag-idl { background: #1a2e42; color: var(--dim); }

/* NILAI BESAR */
.val-row { display: flex; align-items: baseline; gap: 4px; margin-bottom: 4px; }
.val-big {
  font-family: var(--mono);
  font-size: 28px;
  font-weight: 400;
  color: var(--text);
  line-height: 1;
}
.val-unit {
  font-family: var(--mono);
  font-size: 12px;
  color: var(--dim);
}
.val-label { font-size: 10px; color: var(--dim); margin-bottom: 8px; letter-spacing: 1px; }

/* GRID NILAI KECIL */
.metrics { display: grid; grid-template-columns: 1fr 1fr; gap: 6px; margin-top: 8px; }
.metric { background: #0a1520; border-radius: 6px; padding: 7px 10px; }
.metric-label { font-size: 9px; color: var(--dim); letter-spacing: 1px; text-transform: uppercase; margin-bottom: 2px; }
.metric-val { font-family: var(--mono); font-size: 16px; color: var(--text); }
.metric-val.pos { color: var(--green); }
.metric-val.neg { color: var(--red); }
.metric-val.accent { color: var(--accent); }
.metric-val.blue { color: var(--accent2); }

/* SOC BAR */
.soc-wrap { margin-top: 10px; }
.soc-label { display: flex; justify-content: space-between; font-size: 10px; color: var(--dim); margin-bottom-4px; margin-bottom: 4px; }
.soc-bar { height: 6px; background: #0a1520; border-radius: 3px; overflow: hidden; }
.soc-fill {
  height: 100%;
  border-radius: 3px;
  background: var(--green);
  transition: width .5s ease;
}
.soc-fill.warn  { background: var(--accent); }
.soc-fill.crit  { background: var(--red); }

/* SUMMARY ROW */
.sum-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 8px 0;
  border-bottom: 1px solid var(--border);
}
.sum-row:last-child { border-bottom: none; }
.sum-name { font-size: 11px; color: var(--dim); letter-spacing: 1px; }
.sum-vals { display: flex; gap: 16px; }
.sum-val { font-family: var(--mono); font-size: 13px; color: var(--text); text-align: right; min-width: 70px; }
.sum-val span { font-size: 9px; color: var(--dim); }

/* EFF BAR */
.eff-wrap { display: flex; align-items: center; gap: 10px; margin-top: 10px; }
.eff-bar { flex: 1; height: 4px; background: #0a1520; border-radius: 2px; overflow: hidden; }
.eff-fill { height: 100%; background: linear-gradient(90deg, var(--accent), var(--green)); border-radius: 2px; transition: width .5s; }
.eff-pct { font-family: var(--mono); font-size: 14px; color: var(--accent); min-width: 48px; text-align: right; }

/* KWH GRID */
.kwh-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 6px; margin-top: 8px; }
.kwh-item { background: #0a1520; border-radius: 6px; padding: 8px 10px; text-align: center; }
.kwh-label { font-size: 9px; color: var(--dim); letter-spacing: 1px; text-transform: uppercase; margin-bottom: 3px; }
.kwh-val { font-family: var(--mono); font-size: 14px; color: var(--accent); }

/* ENV */
.env-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; margin-top: 4px; }
.env-item { background: #0a1520; border-radius: 6px; padding: 8px 10px; text-align: center; }
.env-label { font-size: 9px; color: var(--dim); letter-spacing: 1px; text-transform: uppercase; margin-bottom: 3px; }
.env-val { font-family: var(--mono); font-size: 16px; color: #a78bfa; }

/* FOOTER */
.footer {
  text-align: center;
  font-size: 10px;
  color: var(--dim);
  margin-top: 12px;
  font-family: var(--mono);
}
</style>
</head>
<body>

<nav>
  <div class="nav-left">
    <div style="display:flex;align-items:center;gap:8px">
      <div class="logo">&#9728; SolarMonitor</div>
      <div class="badge">LIVE</div>
    </div>
    <div class="nav-tabs">
      <button class="tab active">Dashboard</button>
      <button class="tab" onclick="location.href='/setting'">Setting</button>
      <button class="tab" onclick="location.href='/system'">System</button>
      <button class="tab" onclick="location.href='/log'">Log</button>
    </div>
  </div>
  <div style="display:flex;align-items:center;gap:6px;font-size:12px;color:#64748b">
    <span class="uptime" id="uptime">--:--:--</span>
    <span class="conn-dot" id="conn-dot"></span>
  </div>
</nav>

<div style="padding-top:16px">

<!-- BARIS 1: 3 CHANNEL -->
<div class="grid-3">

  <!-- CH1 Panel -->
  <div class="card ch1">
    <div class="card-title">
      <span id="ch1-name">PANEL SURYA</span>
      <span class="tag tag-off" id="ch1-tag">OFF</span>
    </div>
    <div class="val-row">
      <span class="val-big" id="ch1-v">--.-</span>
      <span class="val-unit">V</span>
    </div>
    <div class="val-label">TEGANGAN</div>
    <div class="metrics">
      <div class="metric">
        <div class="metric-label">Arus</div>
        <div class="metric-val accent" id="ch1-a">-.--</div>
        <div class="metric-label">A</div>
      </div>
      <div class="metric">
        <div class="metric-label">Daya</div>
        <div class="metric-val accent" id="ch1-w">--.--</div>
        <div class="metric-label">W</div>
      </div>
    </div>
  </div>

  <!-- CH2 Baterai -->
  <div class="card ch2">
    <div class="card-title">
      <span id="ch2-name">BATERAI</span>
      <span class="tag tag-idl" id="ch2-tag">IDLE</span>
    </div>
    <div class="val-row">
      <span class="val-big" id="ch2-v">-.--</span>
      <span class="val-unit">V</span>
    </div>
    <div class="val-label">TEGANGAN</div>
    <div class="metrics">
      <div class="metric">
        <div class="metric-label">Arus</div>
        <div class="metric-val" id="ch2-a">-.--</div>
        <div class="metric-label">A</div>
      </div>
      <div class="metric">
        <div class="metric-label">SoC</div>
        <div class="metric-val green" id="ch2-soc">--%</div>
        <div class="metric-label">&nbsp;</div>
      </div>
    </div>
    <div class="soc-wrap">
      <div class="soc-label">
        <span>0%</span><span id="soc-pct">--%</span><span>100%</span>
      </div>
      <div class="soc-bar"><div class="soc-fill" id="soc-bar" style="width:0%"></div></div>
    </div>
  </div>

  <!-- CH3 Beban -->
  <div class="card ch3">
    <div class="card-title">
      <span id="ch3-name">BEBAN</span>
      <span class="tag tag-off" id="ch3-tag">OFF</span>
    </div>
    <div class="val-row">
      <span class="val-big" id="ch3-v">-.--</span>
      <span class="val-unit">V</span>
    </div>
    <div class="val-label">TEGANGAN</div>
    <div class="metrics">
      <div class="metric">
        <div class="metric-label">Arus</div>
        <div class="metric-val blue" id="ch3-a">-.--</div>
        <div class="metric-label">A</div>
      </div>
      <div class="metric">
        <div class="metric-label">Daya</div>
        <div class="metric-val blue" id="ch3-w">-.--</div>
        <div class="metric-label">W</div>
      </div>
    </div>
  </div>

</div>

<!-- BARIS 2: RINGKASAN + kWh -->
<div class="grid-2">

  <!-- Ringkasan + Efisiensi -->
  <div class="card summary">
    <div class="card-title">RINGKASAN SISTEM</div>
    <div class="sum-row">
      <span class="sum-name">PANEL</span>
      <div class="sum-vals">
        <span class="sum-val" id="s-ch1-v">--.-<span>V</span></span>
        <span class="sum-val" id="s-ch1-a">-.--<span>A</span></span>
        <span class="sum-val accent" id="s-ch1-w">--.--<span>W</span></span>
      </div>
    </div>
    <div class="sum-row">
      <span class="sum-name">BATERAI</span>
      <div class="sum-vals">
        <span class="sum-val" id="s-ch2-v">-.--<span>V</span></span>
        <span class="sum-val" id="s-ch2-a">-.--<span>A</span></span>
        <span class="sum-val" id="s-ch2-w">-.--<span>W</span></span>
      </div>
    </div>
    <div class="sum-row">
      <span class="sum-name">BEBAN</span>
      <div class="sum-vals">
        <span class="sum-val" id="s-ch3-v">-.--<span>V</span></span>
        <span class="sum-val" id="s-ch3-a">-.--<span>A</span></span>
        <span class="sum-val blue" id="s-ch3-w">-.--<span>W</span></span>
      </div>
    </div>
    <div class="eff-wrap">
      <span style="font-size:10px;color:var(--dim);letter-spacing:1px">EFISIENSI</span>
      <div class="eff-bar"><div class="eff-fill" id="eff-bar" style="width:0%"></div></div>
      <span class="eff-pct" id="eff-pct">--.-%</span>
    </div>
  </div>

  <!-- Akumulasi Energi -->
  <div class="card">
    <div class="card-title">AKUMULASI ENERGI</div>
    <div class="kwh-grid">
      <div class="kwh-item">
        <div class="kwh-label">Panel</div>
        <div class="kwh-val" id="kwh-ch1">-.-- Wh</div>
      </div>
      <div class="kwh-item">
        <div class="kwh-label">Bat IN</div>
        <div class="kwh-val" id="kwh-ch2in">-.-- Wh</div>
      </div>
      <div class="kwh-item">
        <div class="kwh-label">Bat OUT</div>
        <div class="kwh-val" id="kwh-ch2out">-.-- Wh</div>
      </div>
      <div class="kwh-item">
        <div class="kwh-label">Beban</div>
        <div class="kwh-val" id="kwh-ch3">-.-- Wh</div>
      </div>
    </div>
    <!-- Loss -->
    <div style="margin-top:10px;display:flex;justify-content:space-between;align-items:center">
      <span style="font-size:10px;color:var(--dim);letter-spacing:1px">LOSSES SCC</span>
      <span class="metric-val" id="loss-w" style="font-family:var(--mono);font-size:16px;color:#f87171">-.-- W</span>
    </div>
  </div>

</div>

<!-- BARIS 3: ENV (BMP + MPU) jika aktif -->
<div class="grid-1" id="env-section" style="display:none">
  <div class="card env">
    <div class="card-title">SENSOR LINGKUNGAN</div>
    <div class="env-grid">
      <div class="env-item">
        <div class="env-label">Suhu</div>
        <div class="env-val" id="env-temp">--.--</div>
        <div style="font-size:9px;color:var(--dim)">°C</div>
      </div>
      <div class="env-item">
        <div class="env-label">Tekanan</div>
        <div class="env-val" id="env-press">----.-</div>
        <div style="font-size:9px;color:var(--dim)">hPa</div>
      </div>
      <div class="env-item">
        <div class="env-label">Getaran</div>
        <div class="env-val" id="env-vib">-.---</div>
        <div style="font-size:9px;color:var(--dim)" id="env-vib-status">AMAN</div>
      </div>
    </div>
  </div>
</div>

<div class="footer">Solar Monitor v2.0.0 &nbsp;|&nbsp; <span id="footer-ip">192.168.x.x</span></div>

<script>
let failCount = 0;

function fmt(v, d) { return isNaN(v) ? '--' : parseFloat(v).toFixed(d); }
function fmtSigned(v, d) {
  if (isNaN(v)) return '--';
  const n = parseFloat(v);
  return (n >= 0 ? '+' : '') + n.toFixed(d);
}
// Unit otomatis: <1Wh → mWh, 1–999Wh → Wh, ≥1000Wh → kWh
// input dalam kWh (nilai dari ESP32)
function fmtEnergy(kwh) {
  if (isNaN(kwh)) return '--';
  const wh = kwh * 1000;
  if (wh < 1)      return (wh * 1000).toFixed(1) + ' mWh';
  if (wh < 1000)   return wh.toFixed(2) + ' Wh';
  return kwh.toFixed(3) + ' kWh';
}

function fmtUptime(s) {
  const h = Math.floor(s / 3600);
  const m = Math.floor((s % 3600) / 60);
  const sec = s % 60;
  return String(h).padStart(2,'0') + ':' + String(m).padStart(2,'0') + ':' + String(sec).padStart(2,'0');
}

function setConn(ok) {
  const d = document.getElementById('conn-dot');
  d.className = 'conn-dot' + (ok ? '' : ' off');
}

function setBattTag(status) {
  const el = document.getElementById('ch2-tag');
  if (status === 'CHARGING') { el.className='tag tag-chg'; el.textContent='CHG'; }
  else if (status === 'DISCHARG') { el.className='tag tag-dis'; el.textContent='DIS'; }
  else { el.className='tag tag-idl'; el.textContent='IDLE'; }
}

function setSocBar(soc) {
  const fill = document.getElementById('soc-bar');
  fill.style.width = soc + '%';
  if      (soc > 50) fill.className = 'soc-fill';
  else if (soc > 20) fill.className = 'soc-fill warn';
  else               fill.className = 'soc-fill crit';
}

function setTag(id, on) {
  const el = document.getElementById(id);
  if (on) { el.className='tag tag-on'; el.textContent='ON'; }
  else    { el.className='tag tag-off'; el.textContent='OFF'; }
}

function setCh2Current(a) {
  const el = document.getElementById('ch2-a');
  el.textContent = (a >= 0 ? '+' : '') + fmt(a, 2);
  el.className = 'metric-val ' + (a > 0.05 ? 'pos' : a < -0.05 ? 'neg' : '');
}

function update() {
  fetch('/api/data')
    .then(r => r.json())
    .then(d => {
      failCount = 0;
      setConn(true);

      // CH1
      document.getElementById('ch1-v').textContent = fmt(d.ch1.v, 2);
      document.getElementById('ch1-a').textContent = fmt(d.ch1.a, 2);
      document.getElementById('ch1-w').textContent = fmt(d.ch1.w, 2);
      setTag('ch1-tag', d.ch1.w > 1.0);
      document.getElementById('ch1-name').textContent = (d.names.ch1 || 'PANEL').toUpperCase();

      // CH2
      document.getElementById('ch2-v').textContent = fmt(d.ch2.v, 2);
      setCh2Current(d.ch2.a);
      document.getElementById('ch2-soc').textContent = d.ch2.soc + '%';
      document.getElementById('soc-pct').textContent = d.ch2.soc + '%';
      setSocBar(d.ch2.soc);
      setBattTag(d.ch2.status);
      document.getElementById('ch2-name').textContent = (d.names.ch2 || 'BATERAI').toUpperCase();

      // CH3
      document.getElementById('ch3-v').textContent = fmt(d.ch3.v, 2);
      document.getElementById('ch3-a').textContent = fmt(d.ch3.a, 2);
      document.getElementById('ch3-w').textContent = fmt(d.ch3.w, 2);
      setTag('ch3-tag', d.ch3.w > 0.5);
      document.getElementById('ch3-name').textContent = (d.names.ch3 || 'BEBAN').toUpperCase();

      // Ringkasan
      document.getElementById('s-ch1-v').innerHTML = fmt(d.ch1.v,2)+'<span>V</span>';
      document.getElementById('s-ch1-a').innerHTML = fmt(d.ch1.a,2)+'<span>A</span>';
      document.getElementById('s-ch1-w').innerHTML = fmt(d.ch1.w,2)+'<span>W</span>';
      document.getElementById('s-ch2-v').innerHTML = fmt(d.ch2.v,2)+'<span>V</span>';
      document.getElementById('s-ch2-a').innerHTML = fmtSigned(d.ch2.a,2)+'<span>A</span>';
      document.getElementById('s-ch2-w').innerHTML = fmt(Math.abs(d.ch2.w),2)+'<span>W</span>';
      document.getElementById('s-ch3-v').innerHTML = fmt(d.ch3.v,2)+'<span>V</span>';
      document.getElementById('s-ch3-a').innerHTML = fmt(d.ch3.a,2)+'<span>A</span>';
      document.getElementById('s-ch3-w').innerHTML = fmt(d.ch3.w,2)+'<span>W</span>';

      // Efisiensi
      const eff = Math.min(d.eff, 100);
      document.getElementById('eff-pct').textContent = fmt(eff,1) + '%';
      document.getElementById('eff-bar').style.width = eff + '%';

      // Loss
      document.getElementById('loss-w').textContent = fmt(d.loss, 2) + ' W';

      // kWh
      document.getElementById('kwh-ch1').textContent    = fmtEnergy(d.kwh.ch1);
      document.getElementById('kwh-ch2in').textContent  = fmtEnergy(d.kwh.ch2in);
      document.getElementById('kwh-ch2out').textContent = fmtEnergy(d.kwh.ch2out);
      document.getElementById('kwh-ch3').textContent    = fmtEnergy(d.kwh.ch3);

      // Uptime
      document.getElementById('uptime').textContent = fmtUptime(d.uptime);

      // ENV
      if (d.bmp.active || d.mpu.active) {
        document.getElementById('env-section').style.display = 'block';
        if (d.bmp.active) {
          document.getElementById('env-temp').textContent  = fmt(d.bmp.temp, 1);
          document.getElementById('env-press').textContent = fmt(d.bmp.press, 1);
        }
        if (d.mpu.active) {
          document.getElementById('env-vib').textContent        = fmt(d.mpu.vib, 3);
          document.getElementById('env-vib-status').textContent = d.mpu.status;
        }
      }

      // IP
      document.getElementById('footer-ip').textContent = d.ip || '';
    })
    .catch(() => {
      failCount++;
      if (failCount > 3) setConn(false);
    });
}

// Update tiap 1 detik
update();
setInterval(update, 1000);
</script>
</div>
</body>
</html>
)rawliteral";

// ============================================================
// FUNGSI BUILD JSON DATA — dipanggil dari wifi_manager.h
// ============================================================
String buildDataJson(String ipAddr, unsigned long uptimeSec,
                     bool bmpActive, bool mpuActive) {
  String j = "{";

  // CH1
  j += "\"ch1\":{\"v\":" + String(ina.ch[0].voltage, 3)
     + ",\"a\":"         + String(ina.ch[0].current, 3)
     + ",\"w\":"         + String(ina.ch[0].power, 3) + "},";

  // CH2
  j += "\"ch2\":{\"v\":" + String(ina.ch[1].voltage, 3)
     + ",\"a\":"         + String(ina.ch[1].current, 3)
     + ",\"w\":"         + String(ina.ch[1].power, 3)
     + ",\"soc\":"       + String(ina.getSOC())
     + ",\"status\":\""  + String(ina.battStatus()) + "\"},";

  // CH3
  j += "\"ch3\":{\"v\":" + String(ina.ch[2].voltage, 3)
     + ",\"a\":"         + String(ina.ch[2].current, 3)
     + ",\"w\":"         + String(ina.ch[2].power, 3) + "},";

  // Efisiensi & Loss
  float loss = ina.ch[0].power
               - (ina.ch[1].current > 0 ? ina.ch[1].power : 0)
               - ina.ch[2].power;
  j += "\"eff\":"  + String(ina.efficiency, 1) + ",";
  j += "\"loss\":" + String(loss > 0 ? loss : 0, 3) + ",";

  // kWh
  j += "\"kwh\":{\"ch1\":"    + String(ina.kwh[0], 3)
     + ",\"ch2in\":"           + String(ina.kwhCh2In, 3)
     + ",\"ch2out\":"          + String(ina.kwhCh2Out, 3)
     + ",\"ch3\":"             + String(ina.kwh[2], 3) + "},";

  // Nama channel
  j += "\"names\":{\"ch1\":\"" + storage.getCh1Name()
     + "\",\"ch2\":\""         + storage.getCh2Name()
     + "\",\"ch3\":\""         + storage.getCh3Name() + "\"},";

  // BMP280
  j += "\"bmp\":{\"active\":" + String(bmpActive ? "true" : "false");
  if (bmpActive) {
    j += ",\"temp\":"  + String(bmp.temperature, 2)
       + ",\"press\":" + String(bmp.pressure, 1)
       + ",\"alt\":"   + String(bmp.altitude, 1);
  }
  j += "},";

  // MPU6050
  j += "\"mpu\":{\"active\":" + String(mpuActive ? "true" : "false");
  if (mpuActive) {
    j += ",\"vib\":"    + String(mpu.vibration, 3)
       + ",\"status\":\"" + String(mpu.vibrationStatus()) + "\"";
  }
  j += "},";

  // Uptime + IP
  j += "\"uptime\":" + String(uptimeSec) + ",";
  j += "\"ip\":\""   + ipAddr + "\"";
  j += "}";

  return j;
}
