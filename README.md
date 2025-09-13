## ESP-OS OLED Menu (ESP8266)

Minimal 128x64 SSD1306 OLED menu for ESP8266 with physical button navigation, selection lock (OK requires BACK to exit), and a hardware HALT safety pin (D3 to GND). Clean, blocking-safe logic – no animations, just instant updates.

### Features
* 3-item vertical menu (easy to extend)
* Buttons: Up / Down / OK / Back
* Selection lock: after pressing OK, further OK presses are ignored until Back is pressed
* Back screen feedback ("Back")
* Hardware halt: pulling D3 low shows HALTED screen and permanently stops execution (even after release)
* Simple edge-triggered button processing (state change detection)

### Hardware Wiring
| Function | ESP8266 Pin | Notes |
|----------|-------------|-------|
| OLED SDA | D2 (GPIO4)  | I2C data |
| OLED SCL | D1 (GPIO5)  | I2C clock |
| OLED VCC | 3.3V        | Power |
| OLED GND | GND         | Ground |
| Button Up | D5 (GPIO14) | Active LOW (use to GND) |
| Button Down | D6 (GPIO12) | Active LOW |
| Button OK | D7 (GPIO13) | Active LOW |
| Button Back | D4 (GPIO2) | Active LOW |
| HALT Pin | D3 (GPIO0) | Pull to GND to halt |

Buttons assume: one side to the pin, the other side to GND. Internal pull-ups are enabled in code (`INPUT_PULLUP`). Press = logic LOW.

### Halt Behavior (D3)
* If D3 is LOW at boot: show HALTED screen, wait until released, then stay halted forever.
* If D3 goes LOW during runtime: halt screen appears and device never resumes normal loop.
* Use for safety interlock / maintenance jumper.

### Menu Interaction
State | Action |
------|--------|
Idle menu | Up/Down cycles items (wrap-around) |
Press OK | Shows "<Item> selected!", enters locked selection mode |
While locked | Up/Down/OK ignored; only Back works |
Press Back (locked) | Shows Back screen, unlocks |
Press any button (Back screen) | Returns to menu |

### Extending the Menu
In `src/main.cpp` modify:
```
const char* menuItems[] = { "Item 1", "Item 2", "Item 3" };
```
Add more strings and it will auto-adjust. Each row uses 16px height; with 64px screen you can show up to 4 rows cleanly at size 1 (adjust spacing if needed).

### Libraries (PlatformIO `platformio.ini`)
```
lib_deps =
    adafruit/Adafruit SSD1306@^2.5.7
    adafruit/Adafruit GFX Library@^1.11.9
```

### Build / Upload (PlatformIO)
1. Connect board (NodeMCU / ESP8266 DevKit)
2. Open project folder in VS Code
3. PlatformIO: "Build" then "Upload"

If using CLI:
```
pio run
pio run -t upload
```

### Key Code Sections (`src/main.cpp`)
* Globals: display, menu items, button pins, state flags
* `drawMenu()`: renders current menu
* `showSelectedMessage()` / `showBackMessage()` / `showHaltScreen()`
* `loop()`: handles halt, message mode, navigation, and edge detection

### Customization Ideas
* Add debounce (software timestamp or simple delay guard)
* Add auto-timeout to exit selection/back messages
* Add submenu system (stack of indices)
* Persist last selection in EEPROM / LittleFS
* Add icons or invert effect only on text (no full bar)
* Add low-power sleep when idle

### Troubleshooting
Issue | Cause | Fix |
------|-------|-----|
HALTED screen always | D3 held LOW by wiring | Release or add pull-up/jumper |
Nothing on OLED | Wrong I2C addr (0x3C assumed) | Scan bus / confirm module |
Buttons inverted | Wiring to 3.3V not GND | Rewire to GND (active LOW) |
Random selects | Floating buttons | Add hardware pull-down or rely on internal pull-ups correctly |

### License
Use freely. Attribution appreciated.

---
See `src/main.cpp` for the full implementation.
