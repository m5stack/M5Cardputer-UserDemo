# M5Cardputer-UserDemo
M5Cardputer user demo for hardware evaluation.

Press **Q** on the startup screen to enable quiet mode, or press **Q** in the
launcher to toggle it. Quiet mode silences startup audio and UI/keyboard sounds,
shows a muted speaker icon in the status bar, and is remembered across restarts.
Recorded audio playback is unaffected. To silence the first startup sound, hold
**Q** while powering on.

> **Note: If you are using Cardputer ADV, please use the [CardputerADV branch](https://github.com/m5stack/M5Cardputer-UserDemo/tree/CardputerADV).**

#### Tool Chains

[ESP-IDF v4.4.6](https://docs.espressif.com/projects/esp-idf/en/v4.4.6/esp32/index.html)

#### Build

```bash
git clone https://github.com/m5stack/M5Cardputer-UserDemo
```
```bash
cd M5Cardputer-UserDemo
```
```bash
idf.py build
```
