// HTML/JS for the WiFi logger web UI.
// Kept in a separate header because Arduino's .ino prototype-generator
// chokes on the word "function" inside the raw string literal.

const char* INDEX_HTML = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, viewport-fit=cover">
  <title>Cardputer Adv - Wi-Fi Signal Monitor</title>

  <!-- iOS "Add to Home Screen" app behavior (fullscreen, no Safari chrome) -->
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="default">
  <meta name="apple-mobile-web-app-title" content="Wi-Fi Signal Monitor">
  <meta name="theme-color" content="#f2f2f6">
  <link rel="icon" href="/icon.png">
  <link rel="apple-touch-icon" href="/icon.png">

  <script src="https://cdn.jsdelivr.net/npm/chart.js@4.5.1"></script>
  <!-- chartjs-plugin-zoom needs Hammer.js loaded first for pan (drag) and pinch gestures -->
  <script src="https://cdn.jsdelivr.net/npm/hammerjs@2.0.8/hammer.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-zoom@2/dist/chartjs-plugin-zoom.min.js"></script>
  <style>
    :root {
      --bg: #f2f2f6;
      --card: #ffffff;
      --ink: #1c1c1e;
      --muted: #8a8a8e;
      --accent: #2f6fed;
      --accent-bg: #e8f0fe;
      --track: #eceef1;
      --good: #34c759;
      --ok: #ffcc00;
      --bad: #ff3b30;
      --border: rgba(0,0,0,0.06);
      --shadow: 0 1px 3px rgba(0,0,0,0.06), 0 1px 2px rgba(0,0,0,0.04);
    }
    html { background: var(--bg); height: 100%; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: var(--bg);
      color: var(--ink);
      min-height: 100vh;
      min-height: 100dvh;
      margin: 0;
      padding: max(20px, env(safe-area-inset-top)) max(16px, env(safe-area-inset-right))
               max(20px, env(safe-area-inset-bottom)) max(16px, env(safe-area-inset-left));
      box-sizing: border-box;
      overscroll-behavior-y: none;
      -webkit-tap-highlight-color: transparent;
    }
    .wrap { max-width: 460px; margin: 0 auto; }

    .top-row { display: flex; align-items: flex-start; justify-content: space-between; }

    .badge {
      display: inline-block;
      background: var(--accent-bg);
      color: var(--accent);
      font-size: 0.72em;
      font-weight: 700;
      letter-spacing: 0.06em;
      padding: 4px 10px;
      border-radius: 8px;
      margin-bottom: 10px;
    }
    .gear-btn {
      background: var(--card);
      border: none;
      box-shadow: var(--shadow);
      width: 34px;
      height: 34px;
      border-radius: 50%;
      font-size: 1.05em;
      color: var(--muted);
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 0;
      flex-shrink: 0;
    }
    h1 { font-size: 1.7em; font-weight: 800; margin: 0 0 4px; letter-spacing: -0.01em; }
    .ssid-line { color: var(--ink); font-weight: 600; font-size: 0.95em; margin-bottom: 18px; }

    .settings-overlay {
      position: fixed;
      inset: 0;
      background: rgba(0,0,0,0.35);
      display: none;
      align-items: flex-end;
      justify-content: center;
      z-index: 50;
    }
    .settings-overlay.open { display: flex; }
    .settings-panel {
      background: var(--card);
      width: 100%;
      max-width: 460px;
      border-radius: 20px 20px 0 0;
      padding: 20px 20px max(20px, env(safe-area-inset-bottom));
      box-sizing: border-box;
      box-shadow: 0 -4px 24px rgba(0,0,0,0.15);
    }
    .settings-head { display: flex; align-items: center; justify-content: space-between; margin-bottom: 16px; }
    .settings-head h3 { margin: 0; font-size: 1.1em; font-weight: 800; }
    .settings-info {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 10px 2px;
      font-size: 0.9em;
      border-bottom: 1px solid var(--border);
      margin-bottom: 16px;
    }
    .settings-info-label { color: var(--ink); font-weight: 600; }
    .settings-info-value { font-weight: 800; font-size: 1.05em; color: var(--ink); }
    .interval-select {
      font-family: inherit;
      font-size: 1.05em;
      font-weight: 800;
      color: var(--ink);
      -webkit-text-fill-color: var(--ink);
      background: transparent;
      border: none;
      text-align: right;
    }
    .close-btn {
      background: none;
      border: none;
      font-size: 1.5em;
      line-height: 1;
      color: var(--muted);
      cursor: pointer;
      padding: 4px;
    }

    .card {
      background: var(--card);
      border-radius: 16px;
      box-shadow: var(--shadow);
      padding: 18px 20px;
      box-sizing: border-box;
    }

    .metrics {
      display: flex;
      margin-bottom: 14px;
    }
    .metric { flex: 1; }
    .metric + .metric { padding-left: 18px; margin-left: 18px; border-left: 1px solid var(--border); }
    .metric-label { color: var(--muted); font-size: 0.75em; font-weight: 700; letter-spacing: 0.06em; text-transform: uppercase; margin-bottom: 6px; }
    .metric-value { font-size: 1.9em; font-weight: 800; letter-spacing: -0.01em; }
    .metric-value .unit { font-size: 0.5em; font-weight: 600; color: var(--muted); margin-left: 2px; }
    .status-pill {
      display: inline-block;
      font-size: 0.4em;
      font-weight: 800;
      letter-spacing: 0.02em;
      padding: 3px 9px;
      border-radius: 999px;
      margin-left: 8px;
      vertical-align: middle;
    }
    .pill-good { background: var(--good); color: #ffffff; }
    .pill-fair { background: var(--ok); color: #7a5900; }
    .pill-poor { background: var(--bad); color: #ffffff; }
    .bar-track { background: var(--track); border-radius: 999px; height: 8px; margin-top: 10px; overflow: hidden; }
    .bar-fill { height: 100%; border-radius: 999px; background: var(--good); width: 0%; transition: width 0.3s ease, background 0.3s ease; }
    .metric-ts { color: var(--muted); font-size: 0.75em; margin-top: 8px; }

    .legend {
      display: flex;
      justify-content: center;
      flex-wrap: wrap;
      gap: 4px 14px;
      background: var(--card);
      border-radius: 999px;
      box-shadow: var(--shadow);
      padding: 10px 16px;
      margin: 14px 0;
      font-size: 0.78em;
      color: var(--muted);
    }
    .legend .item { display: inline-flex; align-items: center; white-space: nowrap; }
    .dot { display: inline-block; width: 9px; height: 9px; border-radius: 50%; margin-right: 6px; }

    .section-head { display: flex; align-items: baseline; justify-content: space-between; margin: 22px 0 10px; }
    .section-title-group { display: flex; align-items: baseline; gap: 6px; }
    .section-head h2 { font-size: 1.05em; font-weight: 800; margin: 0; }
    .section-head .interval-note { color: var(--ink); font-weight: 700; font-size: 0.85em; }
    .section-head .range-note { color: var(--muted); font-size: 0.8em; }

    .range-picker {
      display: flex;
      background: var(--track);
      border-radius: 999px;
      padding: 3px;
      margin-bottom: 12px;
    }
    .range-picker button {
      flex: 1;
      border: none;
      background: transparent;
      color: var(--muted);
      font-family: inherit;
      font-size: 0.8em;
      font-weight: 700;
      padding: 7px 0;
      border-radius: 999px;
      cursor: pointer;
    }
    .range-picker button.active { background: var(--card); color: var(--ink); box-shadow: var(--shadow); }

    .chart-box { position: relative; width: 100%; height: 240px; overflow: hidden; border-radius: 16px; }
    /* Without this, iOS Safari intercepts a one-finger drag on the canvas as a
       page-scroll gesture instead of handing it to the pan/pinch handlers. */
    #chart { touch-action: none; }
    .zoom-watermark {
      position: absolute;
      top: 4px;
      right: 10px;
      z-index: 1;
      font-size: 2em;
      font-weight: 800;
      letter-spacing: 0.02em;
      text-transform: uppercase;
      color: rgba(0,0,0,0.1);
      pointer-events: none;
      line-height: 1;
      display: none;
    }
    .zoom-watermark.active { display: block; }
    .zoom-hint { text-align: center; color: var(--muted); font-size: 0.72em; margin-top: 8px; }

    #exportCsvBtn, #clearLogBtn, #forgetBtn {
      display: block;
      width: 100%;
      border: none;
      padding: 13px 14px;
      border-radius: 14px;
      font-family: inherit;
      font-size: 0.95em;
      font-weight: 700;
      cursor: pointer;
    }
    #exportCsvBtn, #clearLogBtn { background: var(--track); color: var(--ink); margin-bottom: 10px; }
    #forgetBtn { background: rgba(255,59,48,0.08); color: var(--bad); }

    .footer { text-align: center; color: var(--muted); font-size: 0.78em; margin: 18px 0 4px; }
  </style>
</head>
<body>
  <div class="wrap">
    <div class="top-row">
      <span class="badge">CARDPUTER ADV</span>
      <button class="gear-btn" id="settingsBtn" aria-label="Settings">&#9881;</button>
    </div>
    <h1>Wi-Fi Signal Monitor</h1>
    <div class="ssid-line" id="ssid">-</div>

    <div class="card">
      <div class="metrics">
        <div class="metric">
          <div class="metric-label">RSSI</div>
          <div class="metric-value"><span id="rssi">-</span><span class="unit">dBm</span><span class="status-pill" id="rssiStatus">-</span></div>
          <div class="bar-track"><div class="bar-fill" id="rssiBar"></div></div>
          <div class="metric-ts" id="ts">-</div>
        </div>
        <div class="metric">
          <div class="metric-label">Battery</div>
          <div class="metric-value"><span id="batt">-</span><span class="unit">%</span></div>
          <div class="bar-track"><div class="bar-fill" id="battBar"></div></div>
        </div>
      </div>
    </div>

    <div class="legend">
      <span class="item"><span class="dot" style="background:#34c759"></span>Good (&ge; -60 dBm)</span>
      <span class="item"><span class="dot" style="background:#ffcc00"></span>Fair (-60 to -75 dBm)</span>
      <span class="item"><span class="dot" style="background:#ff3b30"></span>Poor (&lt; -75 dBm)</span>
    </div>

    <div class="section-head">
      <div class="section-title-group">
        <h2>Signal history</h2>
        <span class="interval-note">- Interval: <span id="intervalHeader">60</span>s<span id="avgNote"></span></span>
      </div>
      <span class="range-note" id="rangeLabel">last 1h</span>
    </div>
    <div class="range-picker">
      <button data-h="1" class="active">1H</button>
      <button data-h="3">3H</button>
      <button data-h="6">6H</button>
      <button data-h="12">12H</button>
      <button data-h="24">24H</button>
      <button data-h="all">All</button>
    </div>
    <div class="card chart-box">
      <canvas id="chart"></canvas>
      <span class="zoom-watermark" id="zoomLabel">Zoomed</span>
    </div>
    <div class="zoom-hint">Pinch to zoom &middot; drag to pan &middot; tap to zoom Y</div>

    <div class="footer">Cardputer Adv &middot; ESP32-S3</div>
  </div>

  <div class="settings-overlay" id="settingsOverlay">
    <div class="settings-panel">
      <div class="settings-head">
        <h3>Settings</h3>
        <button class="close-btn" id="closeSettings" aria-label="Close">&times;</button>
      </div>
      <div class="settings-info">
        <span class="settings-info-label">Cardputer's IP address</span>
        <span class="settings-info-value" id="ipAddr">-</span>
      </div>
      <div class="settings-info">
        <span class="settings-info-label">Logging interval</span>
        <select id="intervalSelect" class="interval-select">
          <option value="5000">5s</option>
          <option value="10000">10s</option>
          <option value="30000">30s</option>
          <option value="60000">60s</option>
        </select>
      </div>
      <div class="settings-info">
        <span class="settings-info-label">SD card</span>
        <span class="settings-info-value" id="sdStatus">-</span>
      </div>
      <button id="exportCsvBtn">Export CSV</button>
      <button id="clearLogBtn">Clear Log</button>
      <button id="forgetBtn">Forget Wi-Fi</button>
    </div>
  </div>
  <script>
    // Per-boot token from the device; required by /forget so a stray
    // request from elsewhere on the LAN can't blindly wipe WiFi creds.
    const CSRF_TOKEN = "%CSRF_TOKEN%";

    // Draws shaded good/ok/weak bands behind the line plus dashed threshold
    // lines at -60 and -75 dBm, so the zones are visible at a glance instead
    // of only being explained in the legend above the chart.
    const thresholdBands = {
      id: 'thresholdBands',
      beforeDatasetsDraw(chart) {
        const { ctx, chartArea, scales: { y } } = chart;
        if (!chartArea) return;
        const { left, right, top, bottom } = chartArea;
        // The y-axis now auto-scales to whatever's visible, so a threshold can
        // fall outside the current range - clamp the fill so it never bleeds
        // past the chart area, and skip the dashed line/label when that
        // threshold isn't actually in view.
        const goodY = Math.max(top, Math.min(bottom, y.getPixelForValue(-60)));
        const okY = Math.max(top, Math.min(bottom, y.getPixelForValue(-75)));

        ctx.save();
        ctx.fillStyle = 'rgba(52,199,89,0.08)';
        ctx.fillRect(left, top, right - left, goodY - top);
        ctx.fillStyle = 'rgba(255,204,0,0.10)';
        ctx.fillRect(left, goodY, right - left, okY - goodY);
        ctx.fillStyle = 'rgba(255,59,48,0.08)';
        ctx.fillRect(left, okY, right - left, bottom - okY);

        ctx.setLineDash([4, 4]);
        ctx.lineWidth = 1;
        ctx.font = '10px -apple-system, sans-serif';
        ctx.textAlign = 'right';
        if (y.min <= -60 && -60 <= y.max) {
          ctx.strokeStyle = 'rgba(52,199,89,0.6)';
          ctx.beginPath(); ctx.moveTo(left, goodY); ctx.lineTo(right, goodY); ctx.stroke();
          ctx.fillStyle = '#248a3d';
          ctx.fillText('-60 dBm', right - 4, goodY - 4);
        }
        if (y.min <= -75 && -75 <= y.max) {
          ctx.strokeStyle = 'rgba(255,59,48,0.6)';
          ctx.beginPath(); ctx.moveTo(left, okY); ctx.lineTo(right, okY); ctx.stroke();
          ctx.fillStyle = '#d70015';
          ctx.fillText('-75 dBm', right - 4, okY + 12);
        }
        ctx.setLineDash([]);
        ctx.restore();
      }
    };

    const zoomLabel = document.getElementById('zoomLabel');
    let currentFiltered = []; // the full loaded history currently plotted (see getVisibleSlice for what's on screen)
    let yFitted = false; // true after a tap has fitted the y-axis, until the next reset/range change

    function resetYScale() {
      chart.options.scales.y.min = -100;
      chart.options.scales.y.max = -20;
    }

    // "ZOOMED" reflects only the tap-to-fit y-axis state now. Panning/pinching
    // around in time is normal navigation across the full loaded history, not
    // a special state worth flagging - every range button other than "All"
    // narrows the initial view via zoomScale(), which would otherwise make
    // isZoomedOrPanned() true almost all the time.
    function updateZoomLabel() {
      zoomLabel.classList.toggle('active', yFitted);
    }

    // The data points currently visible within the chart's x-axis window
    // (currentFiltered always holds the full loaded history now - this is
    // just whatever slice of it is scrolled/zoomed into view right now).
    function getVisibleSlice() {
      if (!currentFiltered.length) return [];
      const xMin = chart.scales.x.min ?? 0;
      const xMax = chart.scales.x.max ?? currentFiltered.length - 1;
      const lo = Math.max(0, Math.round(xMin));
      const hi = Math.min(currentFiltered.length - 1, Math.round(xMax));
      return lo <= hi ? currentFiltered.slice(lo, hi + 1) : currentFiltered;
    }

    // Fits the y-axis to whatever's actually visible on screen right now,
    // rather than the full -100..-20 dBm range - makes small fluctuations
    // readable instead of squashed into a flat-looking line. Only runs when
    // triggered by tapping the chart (or while tracking after that, on pan/zoom).
    function autoScaleY() {
      const visible = getVisibleSlice();
      if (!visible.length) return;
      const values = visible.map(p => p.v);
      const dataMin = Math.min(...values);
      const dataMax = Math.max(...values);

      const pad = 3;
      let yMin = Math.max(-100, dataMin - pad);
      let yMax = Math.min(-20, dataMax + pad);
      if (yMax - yMin < 10) {
        const mid = (yMin + yMax) / 2;
        yMin = Math.max(-100, mid - 10);
        yMax = Math.min(-20, mid + 10);
      }
      chart.options.scales.y.min = yMin;
      chart.options.scales.y.max = yMax;
      chart.update('none');
    }

    // Keeps the header average, y-fit, and "ZOOMED" label in sync with
    // whatever's currently scrolled/zoomed into view.
    function updateVisibleStats() {
      updateAvgNote(getVisibleSlice());
      if (yFitted) autoScaleY();
      updateZoomLabel();
    }

    function onZoomOrPanComplete() {
      updateVisibleStats();
    }

    const ctx = document.getElementById('chart').getContext('2d');
    const chart = new Chart(ctx, {
      type: 'line',
      data: { labels: [], datasets: [{ label: 'RSSI (dBm)', data: [], borderColor: '#2f6fed', borderWidth: 1, tension: 0.2, pointRadius: 0 }] },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        scales: {
          x: { ticks: { color: '#8a8a8e', maxTicksLimit: 6, font: { size: 9 } }, grid: { color: 'rgba(0,0,0,0.05)' } },
          // min/max are recalculated per render by autoScaleY() to fit whatever's
          // visible - these are just the pre-data-load fallback.
          y: { min: -100, max: -20, ticks: { color: '#8a8a8e', maxTicksLimit: 6 }, grid: { color: 'rgba(0,0,0,0.05)' } }
        },
        plugins: {
          legend: { display: false },
          zoom: {
            pan: { enabled: true, mode: 'x', onPanComplete: onZoomOrPanComplete },
            zoom: {
              pinch: { enabled: true },
              mode: 'x',
              onZoomComplete: onZoomOrPanComplete
            }
          }
        }
      },
      plugins: [thresholdBands]
    });

    // Tapping the chart (a click with no real movement between press and
    // release) toggles the y-axis fit - dragging to pan shouldn't also
    // trigger it, so a real pointerdown->click is only treated as a tap
    // when the pointer barely moved.
    let tapStart = null;
    ctx.canvas.addEventListener('pointerdown', (e) => {
      tapStart = { x: e.clientX, y: e.clientY };
    });
    ctx.canvas.addEventListener('click', (e) => {
      if (!tapStart) return;
      const moved = Math.hypot(e.clientX - tapStart.x, e.clientY - tapStart.y);
      tapStart = null;
      if (moved > 6) return; // was a drag/pan, not a tap
      yFitted = !yFitted;
      if (!yFitted) { resetYScale(); chart.update(); }
      updateVisibleStats();
    });

    function colorForRssi(rssi) {
      return rssi >= -60 ? '#34c759' : rssi >= -75 ? '#ffcc00' : '#ff3b30';
    }
    function rssiStatus(rssi) {
      return rssi >= -60 ? { label: 'Good', cls: 'pill-good' }
           : rssi >= -75 ? { label: 'Fair', cls: 'pill-fair' }
           : { label: 'Poor', cls: 'pill-poor' };
    }
    function colorForBattery(pct) {
      return pct >= 50 ? '#34c759' : pct >= 20 ? '#ffcc00' : '#ff3b30';
    }

    // "YYYY-MM-DD HH:MM:SS" -> "dd.mm.yyyy, HH:MM:SS"
    function formatTs(ts) {
      const m = /^(\d{4})-(\d{2})-(\d{2})[ T](\d{2}:\d{2}:\d{2})$/.exec(ts);
      if (!m) return ts;
      return `${m[3]}.${m[2]}.${m[1]}, ${m[4]}`;
    }

    async function pollNow() {
      try {
        const r = await fetch('/data');
        const d = await r.json();
        document.getElementById('rssi').textContent = d.connected ? d.rssi : '--';
        document.getElementById('ts').textContent = formatTs(d.ts);
        document.getElementById('ssid').textContent = d.ssid;
        document.getElementById('ipAddr').textContent = d.ip;
        document.getElementById('intervalHeader').textContent = Math.round(d.logIntervalMs / 1000);
        const intervalSelect = document.getElementById('intervalSelect');
        if (document.activeElement !== intervalSelect) {
          intervalSelect.value = String(d.logIntervalMs);
        }
        const sdStatus = document.getElementById('sdStatus');
        sdStatus.textContent = d.sdOk ? 'OK' : 'Not available';
        sdStatus.style.color = d.sdOk ? 'var(--good)' : 'var(--bad)';

        const rssiBar = document.getElementById('rssiBar');
        const pill = document.getElementById('rssiStatus');
        if (d.connected) {
          const rssiPct = Math.max(0, Math.min(100, (d.rssi + 100) / 80 * 100));
          rssiBar.style.width = rssiPct + '%';
          rssiBar.style.background = colorForRssi(d.rssi);
          const status = rssiStatus(d.rssi);
          pill.textContent = status.label;
          pill.className = 'status-pill ' + status.cls;
        } else {
          rssiBar.style.width = '0%';
          rssiBar.style.background = '#8a8a8e';
          pill.textContent = 'Offline';
          pill.className = 'status-pill pill-poor';
        }

        // -1 = battery level unavailable (M5.Power.getBatteryLevel() sentinel)
        const battUnknown = d.batt < 0;
        document.getElementById('batt').textContent = battUnknown ? '--' : d.batt;
        const battBar = document.getElementById('battBar');
        battBar.style.width = (battUnknown ? 0 : Math.max(0, Math.min(100, d.batt))) + '%';
        battBar.style.background = battUnknown ? '#8a8a8e' : colorForBattery(d.batt);
      } catch (e) {}
      setTimeout(pollNow, 2000);
    }

    let historyRaw = []; // [{t: Date, v: number}]
    let rangeHours = 1;

    function fmtTime(d) {
      const time = d.toTimeString().slice(0, 5);
      const date = `${String(d.getDate()).padStart(2, '0')}.${String(d.getMonth() + 1).padStart(2, '0')}`;
      return [time, date];
    }

    function updateAvgNote(filtered) {
      const avgNote = document.getElementById('avgNote');
      if (!filtered.length) { avgNote.textContent = ''; return; }
      const avg = Math.round(filtered.reduce((sum, p) => sum + p.v, 0) / filtered.length);
      avgNote.innerHTML = ` | Avg: <span style="color:${colorForRssi(avg)}">${avg}</span> dBm`;
    }

    // The chart always plots the FULL loaded history now, not just the
    // selected range - a range button only picks where the initial view
    // starts. That's what makes drag-to-pan able to reach further back than
    // "now minus N hours": the earlier points are already loaded, just
    // scrolled out of the initial view rather than absent from the chart.
    function applyRange() {
      chart.resetZoom();
      if (!historyRaw.length) {
        currentFiltered = [];
        chart.data.labels = [];
        chart.data.datasets[0].data = [];
        if (!yFitted) resetYScale();
        chart.update();
        updateVisibleStats();
        return;
      }
      currentFiltered = historyRaw;
      chart.data.labels = historyRaw.map(p => fmtTime(p.t));
      chart.data.datasets[0].data = historyRaw.map(p => p.v);
      if (!yFitted) resetYScale();
      chart.update();

      if (rangeHours !== 'all') {
        const lastT = historyRaw[historyRaw.length - 1].t;
        const cutoff = new Date(lastT.getTime() - rangeHours * 3600 * 1000);
        let startIdx = historyRaw.findIndex(p => p.t >= cutoff);
        if (startIdx === -1) startIdx = historyRaw.length - 1;
        chart.zoomScale('x', { min: startIdx, max: historyRaw.length - 1 }, 'none');
      }
      updateVisibleStats();
    }

    let historyLoaded = false;

    async function loadHistory() {
      try {
        const r = await fetch('/history');
        const text = await r.text();
        const lines = text.trim().split('\n').filter(l => l.length > 0);
        historyRaw = lines.map(l => {
          const parts = l.split(',');
          return { t: new Date(parts[0].replace(' ', 'T')), v: parseInt(parts[1]) };
        }).filter(p => !isNaN(p.t.getTime()) && !isNaN(p.v));
        // Local timestamps repeat an hour at the CEST->CET DST fold-back,
        // so raw log order isn't guaranteed strictly increasing - sort so
        // the chart never draws a line running backwards.
        historyRaw.sort((a, b) => a.t - b.t);
        if (!historyLoaded) {
          // First load: jump to the selected range's initial window.
          historyLoaded = true;
          applyRange();
        } else {
          // Later refreshes: just refresh the data in place. Don't call
          // applyRange() here - it'd reset pan/zoom back to "now" every 30s,
          // which would yank you out of wherever in history you'd scrolled to.
          currentFiltered = historyRaw;
          chart.data.labels = historyRaw.map(p => fmtTime(p.t));
          chart.data.datasets[0].data = historyRaw.map(p => p.v);
          chart.update();
          updateVisibleStats();
        }
      } catch (e) {}
      setTimeout(loadHistory, 30000);
    }

    document.querySelectorAll('.range-picker button').forEach(btn => {
      btn.onclick = () => {
        document.querySelectorAll('.range-picker button').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        rangeHours = btn.dataset.h === 'all' ? 'all' : parseInt(btn.dataset.h);
        document.getElementById('rangeLabel').textContent = btn.dataset.h === 'all' ? 'all time' : `last ${btn.dataset.h}h`;
        applyRange();
      };
    });

    pollNow();
    loadHistory();

    const settingsOverlay = document.getElementById('settingsOverlay');
    document.getElementById('settingsBtn').onclick = () => settingsOverlay.classList.add('open');
    document.getElementById('closeSettings').onclick = () => settingsOverlay.classList.remove('open');
    settingsOverlay.onclick = (e) => { if (e.target === settingsOverlay) settingsOverlay.classList.remove('open'); };

    document.getElementById('intervalSelect').onchange = async (e) => {
      try {
        await fetch('/interval', {
          method: 'POST',
          headers: { 'X-CSRF-Token': CSRF_TOKEN, 'Content-Type': 'application/x-www-form-urlencoded' },
          body: 'ms=' + e.target.value
        });
      } catch (err) {}
    };

    function toCsvTs(d) {
      const p = n => String(n).padStart(2, '0');
      return `${d.getFullYear()}-${p(d.getMonth() + 1)}-${p(d.getDate())} ${p(d.getHours())}:${p(d.getMinutes())}:${p(d.getSeconds())}`;
    }

    document.getElementById('exportCsvBtn').onclick = () => {
      if (!historyRaw.length) { alert('No history to export yet.'); return; }
      const rows = historyRaw.map(p => `${toCsvTs(p.t)},${p.v}`);
      const csv = 'timestamp,rssi_dbm\n' + rows.join('\n') + '\n';
      const blob = new Blob([csv], { type: 'text/csv' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      const ssid = (document.getElementById('ssid').textContent || 'wifi').replace(/[^a-z0-9]+/gi, '_');
      const stamp = new Date().toISOString().slice(0, 10);
      a.href = url;
      a.download = `wifi-signal-${ssid}-${stamp}.csv`;
      document.body.appendChild(a);
      a.click();
      a.remove();
      URL.revokeObjectURL(url);
    };

    document.getElementById('clearLogBtn').onclick = async () => {
      if (!confirm("Delete the signal history for this network and start fresh? This only clears the currently connected network's log.")) return;
      try {
        await fetch('/clearlog', { method: 'POST', headers: { 'X-CSRF-Token': CSRF_TOKEN } });
      } catch (err) {}
      historyRaw = [];
      applyRange();
    };

    document.getElementById('forgetBtn').onclick = async () => {
      if (!confirm('Forget saved Wi-Fi credentials and restart into setup mode?')) return;
      await fetch('/forget', { method: 'POST', headers: { 'X-CSRF-Token': CSRF_TOKEN } });
      document.body.innerHTML = '<h2 style="font-family:-apple-system,sans-serif;color:#1c1c1e;padding:40px">Restarting into Wi-Fi setup mode...</h2>';
    };
  </script>
</body>
</html>
)HTML";
