# Judo Class Round Timer ⏱️🥋
 
A round timer for martial arts/boxing/whatever.

This project is a compact, battery-powered round timer designed for martial arts classes like judo. It uses an Adafruit Trinket M0, an I2C alphanumeric LED display, and a piezo buzzer to provide audible round signals.  This is used by us at https://austinjudo.club

I've included the FreeCad file for the box.

I need distractions while I'm working on my AI Stock Trader at https://ai-stocktrading.com

---

## 🔧 Features

- Selectable round durations: **30, 45, 60, 120, 180, or 300 seconds**
- **Start/Pause button** with audible feedback
- **Looping rounds**: automatically restarts after each round ends
- **Onboard RGB LED flash** and buzzer at round end
- **Volume control** via button combo
- **Sound style selection** via button combo (10 unique sounds)
- **3-digit countdown display**
- **Sleep mode** after 5 minutes of inactivity (wakes on button press)
- Stores settings (volume, interval, sound) in onboard flash
- Powered by a **3.7V LiPo battery** with a USB charger
- Runs for many sessions on a single charge

---

## 🛠️ Hardware

| Component                      | Purpose                            |
|-------------------------------|------------------------------------|
| Adafruit Trinket M0           | Main controller                    |
https://www.adafruit.com/product/3500
| Adafruit Alphanumeric Display | 4-character countdown display (I²C)|
https://www.adafruit.com/product/879
| Piezo buzzer w/ driver        | Audible round signal               |
https://www.adafruit.com/product/5791
| USB LiPo charger              | Recharge battery between classes.  |
https://www.adafruit.com/product/1905
| Power switch                  | Power off between classes.         |
| 2x pushbuttons                | Mode and Start/Pause               |
| LiPo battery (3.7V)           | Power source                       |

---

## 🔌 Pin Connections

| Trinket M0 Pin | Connected Device        |
|----------------|-------------------------|
| BAT            | LiPo Battery +          |
| GND            | Battery –, display GND, button GNDs |
| 3V             | Display VCC             |
| 0 (SCL)        | Display SCL             |
| 2 (SDA)        | Display SDA             |
| 1              | Buzzer IN               |
| 3              | MODE button             |
| 4              | START/PAUSE button      |
| 7 (DotStar DI) | Onboard RGB data (internal) |
| 8 (DotStar CI) | Onboard RGB clock (internal) |

---

## 🧠 Controls

| Action                         | Result                         |
|--------------------------------|--------------------------------|
| Tap **MODE**                  | Change duration or reset if running |
| Tap **START**                 | Start or pause timer          |
| Hold **START**, tap **MODE** | Cycle volume 1–9              |
| Hold **MODE**, tap **START** | Cycle sound style (0–9)      |

---

## 💾 Flash Memory

Settings are saved using `FlashStorage`:
- Volume: 1–9
- Interval: 0–5 (maps to 30–300 seconds)
- Sound index: 0–9

---

## 💤 Power Saving

- Sleeps after **5 minutes** of inactivity
- Wakes instantly on any button press
- RGB and buzzer are disabled while asleep

---

## 📷 Project Photos

---

## 📜 License

This project is licensed under the MIT License. Feel free to modify and reuse!

---

## 🙌 Acknowledgments

- Adafruit for their Trinket M0 and display libraries.  Lady Ada is awesome!
- Arduino for platform support
- The students of Austin Judo Club 🥋  https://austinjudo.club
