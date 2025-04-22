# Automatic-Med-Dispenser
# 💊 Automatic Medicine Dispenser Using Arduino

An automatic medicine dispenser system that dispenses pills at scheduled times using a keypad for input, RTC for timekeeping, servo motors for dispensing, an IR sensor for presence detection, and a buzzer for alerts.

## 🧠 Features

- Set current time and dispensing schedules via a 4x4 keypad
- Dispenses medicine from 2 compartments using servo motors
- Real-time clock support using DS3231 RTC module
- IR sensor to detect user presence
- Buzzer alarm before dispensing
- Manual refill and reset options using push buttons

---

## 🔧 Hardware Requirements

| Component               | Quantity |
|------------------------|----------|
| Arduino Uno / Nano     | 1        |
| Servo Motor (SG90/MG90) | 2        |
| 4x4 Matrix Keypad      | 1        |
| DS3231 RTC Module      | 1        |
| IR Sensor Module       | 1        |
| Buzzer                 | 1        |
| Push Buttons           | 2        |
| Breadboard + Jumpers   | -        |

### ⚙️ Pin Configuration

| Component      | Pin           |
|----------------|---------------|
| Servo 1        | A0            |
| Servo 2        | A1            |
| IR Sensor      | D12           |
| Buzzer         | D13           |
| Reset Button   | D10           |
| Refill Button  | D11           |
| Keypad Rows    | D9, D8, D7, D6|
| Keypad Columns | D5, D4, D3, D10|

> Make sure the RTC is connected via **I2C** (SDA -> A4, SCL -> A5 on Uno)

---

## 🧩 Required Libraries

Install the following libraries from Arduino Library Manager:

- `Adafruit_Keypad`
- `Servo`
- `RTClib` (by Adafruit)
- `TimeLib`

---

## 🛠️ Setup Instructions

1. **Connect all components** as per the pin diagram above.
2. **Upload the code** using Arduino IDE.
3. **Open Serial Monitor** (9600 baud rate).
4. On first boot, **set current time** using the keypad.
5. **Set schedules** for Servo 1 and Servo 2 (hours & minutes).
6. The system will now beep and dispense medicine at those times.

---

## 🎮 Controls

- `Keypad`:
  - Use digits to enter time
  - Press `#` to confirm input
  - Press `*` to cancel input
- `Reset Button`: Resets dispensing time schedules
- `Refill Button`: Moves servos to open-close position to refill medicine

---

## 🚨 Behavior

- At the set time:
  - The **buzzer beeps**
  - If **user is detected** by IR sensor, servo rotates to dispense
  - Otherwise, a message is printed on Serial saying “No one detected”

---

## 🔄 Customization

- Change `SERVO1_PIN` / `SERVO2_PIN` if using different analog pins
- Adjust servo angles in `refillServos()` or `checkAndDispenseMedicine()` as per your container design
- Use `tone()` and `noTone()` for customized alert patterns

---

## 📸 Future Enhancements

- LCD/LED display to show time and status
- Blynk/IoT integration for mobile alerts
- Battery backup for RTC
- SD card logging for dosage tracking

---

## ✨ Author

**Ajith koli**  
B.Tech Student, RVCE  
GitHub: Ajithkoli(https://github.com/Ajithkoli)  
