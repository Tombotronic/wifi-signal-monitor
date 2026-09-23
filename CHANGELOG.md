# Changelog

Each `## vX.Y.Z` section becomes the notes of that GitHub release (see `.github/workflows/release.yml`).

## v1.5.0

Releases are now built and published automatically by GitHub Actions.

- Firmware is now built for the Cardputer board profile (8MB flash) instead of the generic ESP32-S3 one (4MB). As with every release image, flashing it resets the saved WiFi credentials and logging interval; your SD card logs are untouched.
- The firmware version is shown on the device's IP overlay (press **I**) and in the dashboard's settings panel.

## v1.4.1

Bug fixes and hardening from the full-codebase review in #6.

- **Fixed:** on a brand-new device, choosing "ENTER: retry" after a failed first WiFi connection silently forced a full SSID/password re-entry instead of actually retrying — `haveCreds` was never flipped to `true` after saving fresh credentials. Existing saved credentials were never affected, only first-time setup.
- CSRF check on `/forget`, `/clearlog`, and `/interval` now uses a constant-time comparison instead of a regular string `!=`.
- The dashboard's `/` route now streams the page in three chunks instead of copying the whole ~700-line HTML into a fresh heap `String` on every single request — reduces heap churn on a device meant to stay powered on for weeks.
- Settings panel's interval dropdown now handles a logging interval that doesn't match one of its four fixed presets (e.g. set via a direct API call) by adding a temporary option, instead of showing blank/unselected.

## v1.4.0

Signal history average, chart zoom/pan, and CSV export (#5).

- The Signal history header now shows the average RSSI for whatever's currently visible, colored to match the Good/Fair/Poor status pills.
- Pinch-to-zoom and drag-to-pan on the chart's x-axis; tapping the chart toggles a tight y-axis fit around the visible data. The full log is always loaded now, not just the selected range, so panning can reach arbitrarily far back through history instead of being capped at the range picker's window.
- A new "Export CSV" button in the settings panel downloads the current network's full signal history log.
- Unified the app's name to "Wi-Fi Signal Monitor" across the browser tab title, home-screen title, and on-page heading (previously three different names).

## v1.3.0

Clear log button, plus a lower logging interval option.

- New "Clear Log" button in the settings panel (with a confirm prompt) deletes and resets the currently connected network's log file, so you can start a network's history fresh without pulling the SD card (#4).
- The logging interval picker now offers 5s as well as 10/30/60s.

## v1.2.0

Adjustable logging interval, plus two fixes from a full codebase review (#3).

- The SD-card logging interval is now adjustable from the dashboard's settings panel: 10s / 30s / 60s. Previously hardcoded to 60s (#2). Persisted in NVS flash, survives a reboot. Shown next to "Signal history" as "Interval: Xs".
- Fixed a bug where two different WiFi networks could silently share one log file if their names sanitized to the same filename (e.g. "Home/Net" and "Home:Net"). Plain network names are unaffected.
- The dashboard now shows SD card health (OK / Not available) in the settings panel, so a card pulled or failing mid-session is visible instead of silently dropping data.

## v1.1.0

Per-SSID signal logging.

- Each WiFi network now logs to its own file on the SD card (`/wifi_signal_monitor/<ssid>.csv`) instead of one shared `wifi_log.csv`, so switching networks no longer mixes their signal history together (#1).
- SSIDs are sanitized for FAT-illegal filename characters when building the log path.

## v1.0.0

Initial public release.
