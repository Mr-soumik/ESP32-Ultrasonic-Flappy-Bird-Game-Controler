# ESP32 Ultrasonic Flappy Bird

### Play Flappy Bird with your bare hand. No buttons, no touch, just a wave.

![Platform](https://img.shields.io/badge/Board-ESP32-blue)
![Sensor](https://img.shields.io/badge/Sensor-HC--SR04-green)
![Display](https://img.shields.io/badge/Display-SSD1306%20OLED-lightgrey)
![Framework](https://img.shields.io/badge/Arduino-C%2B%2B-00979D)
![License](https://img.shields.io/badge/License-GPL--3.0-red)

An HC-SR04 ultrasonic sensor measures how far your hand is. An ESP32 sends that distance to a browser game over USB (or Bluetooth), and the bird follows your hand in real time. A 0.96" OLED shows the live distance, a bird that mirrors your hand, the Bluetooth status and a **JUMP** indicator.

> Built by **TechTadka360**. 
---

## Demo

<img width="899" height="1600" alt="demo" src="https://github.com/user-attachments/assets/e39d2506-da31-4f12-bc39-8045a5de36c5" />

----

**Watch the full build video on YouTube:** https://youtube.com/shorts/VMZz6EZKw8k?si=n7quUln1K8tfGCBI

**Watch the full build video on Facebook:**

**Watch the full build video on Instagram:** https://www.instagram.com/reel/DfhYAh3J7DC/?utm_source=ig_web_copy_link&mdxt=MzRlODBiNWFlZA==&srtk=MzRlODBiNWFlZA==

---

## Features

- Hand-controlled gameplay: bird height follows your hand distance
- ESP32 instead of an Arduino Nano, with the same `D:<cm>` serial protocol
- Premium OLED status screen: live distance (cm), distance bar, animated bird, JUMP indicator, Bluetooth status
- Median filter on 3 sensor readings for stable values
- Dual output: USB serial and Bluetooth Serial at the same time
- Voltage-divider protection on the 5V ECHO line
- Works in Chrome / Edge through the Web Serial API, with no drivers and no app

---

## How it works

```
 Hand --> HC-SR04 --> ESP32 --> "D:12.3" over USB / Bluetooth --> Browser game (Web Serial) --> Bird moves
                         |
                         +--> OLED: distance, bird, JUMP, Bluetooth status
```

1. The ESP32 triggers the HC-SR04 and measures the echo time.
2. It takes the median of 3 readings and converts it to centimetres.
3. It sends one line per reading, like `D:23.5`, at 115200 baud.
4. The game reads those lines and maps the distance between **Near** and **Far** to the bird's height.
5. The OLED shows the same data, and a quick move toward the sensor lights the JUMP indicator.

---

## Hardware required

| Part | Qty |
|---|---|
| ESP32 Dev Module (30 or 38 pin) | 1 |
| HC-SR04 ultrasonic sensor | 1 |
| 0.96" SSD1306 OLED, I2C (128x64) | 1 |
| 1 kΩ resistor | 1 |
| 1.8 kΩ resistor | 1 |
| Breadboard and jumper wires | - |
| USB cable | 1 |

---

## Circuit diagram

![Circuit diagram](docs/circuit_diagram.png)

### Pin connections

| HC-SR04 | ESP32 |
|---|---|
| VCC | VIN (5V) |
| GND | GND |
| TRIG | GPIO 5 |
| ECHO | GPIO 18 **through the voltage divider** |

| OLED | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Voltage divider (important)

The HC-SR04 ECHO pin outputs 5V, but ESP32 GPIO pins are 3.3V only. Connect:

```
ECHO ---[ 1 kΩ ]---+--- GPIO 18
                    |
                 [ 1.8 kΩ ]
                    |
                   GND
```

This gives about 3.2V at the GPIO.

---

## Software setup

### 1. Arduino IDE

- Install **Arduino IDE**
- Add ESP32 support in Boards Manager and install **esp32 by Espressif, version 2.0.17**
- Install these libraries in Library Manager:
  - `Adafruit SSD1306`
  - `Adafruit GFX Library`
- `BluetoothSerial` is built into the ESP32 core

### 2. Upload the firmware

1. Open `firmware/echo_flap_esp32/echo_flap_esp32.ino`
2. Select **Tools > Board > ESP32 Dev Module**
3. If you get a size error, set **Tools > Partition Scheme > Huge APP (3MB No OTA)**
4. Select the COM port and click **Upload**
5. The OLED shows the "ECHO FLAP" splash, then the live distance

### 3. Run the game

1. Close the Arduino Serial Monitor
2. Open `game/index.html` in **Chrome or Edge** on a PC
3. Click **Connect sensor** and choose the ESP32's COM port
4. Open **Settings** and use the values below
5. Press **Space** or click to start, then move your hand over the sensor

> Web Serial needs `https://` or a local file. For an online version, enable **GitHub Pages** for this repository and open the `game` page from there.

### Recommended game settings

| Setting | Value |
|---|---|
| Near | 5 cm |
| Far | 20 to 30 cm |
| Ignore beyond | a little above Far (for example 25 to 40) |
| Smoothing | 22 to 27 (raise it if the bird jerks) |
| Invert | Turn on if the bird moves the wrong way |

Keep your hand flat, 5 to 30 cm above the sensor.

---

## Bluetooth (optional)

The ESP32 also sends the same data over Bluetooth Serial.

1. Pair **TT360 Flappy** in your PC's Bluetooth settings
2. Windows creates a new COM port
3. Pick that port in **Connect sensor**

USB is the most reliable option. Use Bluetooth if you want a wireless setup.

---

## OLED display

| Area | Shows |
|---|---|
| Top bar | TECHTADKA360 title and Bluetooth icon (blinks until linked) |
| Left | Live distance in cm, "BT LINKED" or "BT WAITING" status, distance bar |
| Right | Bird that follows your hand height |
| JUMP | Arrow and JUMP tag when the hand moves quickly toward the sensor |

---

## Troubleshooting

| Problem | Fix |
|---|---|
| Distance is always `--` | Check TRIG on GPIO 5 and ECHO on GPIO 18, and make sure the sensor has 5V on VIN |
| OLED stays blank | Check SDA on 21 and SCL on 22, and confirm the I2C address is 0x3C |
| Game cannot connect | Close the Serial Monitor, and use Chrome or Edge on a PC |
| Bird moves the wrong way | Turn **Invert** on in Settings |
| Bird jerks or jumps | Raise **Smoothing**, keep your hand flat, and do not go closer than 5 cm |
| Compile error with BluetoothSerial | Use ESP32 core 2.0.17 and the Huge APP partition scheme |

---

## Project structure

```
.
├── firmware/
│   └── echo_flap_esp32/
│       └── echo_flap_esp32.ino
├── game/
│   └── index.html
├── docs/
│   ├── circuit_diagram.svg
│   ├── circuit_diagram.png
│   └── demo.jpg
└── README.md
```

---

## Credits

- Game: **Echo Flap** by **Hak Se Engineer**, [github.com/hakseengineer/FlappyBird](https://github.com/hakseengineer/FlappyBird). The `game/index.html` file is his game with a TechTadka360 credit line added to the title screen.
- ESP32 firmware, OLED interface, wiring and documentation: **TechTadka360**

---

## License

This project includes GPL-3.0 licensed work, so it is released under the **GNU GPL v3.0**. See the `LICENSE` file.

---

## Follow TechTadka360

| Platform | Link |
|---|---|
| YouTube | [ADD LINK](https://youtube.com/) |
| Instagram | [ADD LINK](https://instagram.com/) |
| Facebook | [ADD LINK](https://facebook.com/) |
| GitHub | [@Mr-soumik](https://github.com/Mr-soumik) |

If this project helped you, give it a star on GitHub and subscribe for more ESP32 and Arduino builds.
