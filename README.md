🏭 INDUSGUARD - Industrial Fault Detection and Safety Alert System 🚨
Platform Language Protocol Status
📖 About:
INDUSGUARD is an Embedded Systems project designed to provide real-time industrial fault detection and safety monitoring. The system continuously monitors environmental parameters and sends alert messages through GSM whenever abnormal conditions are detected.

It is built around the LPC2148 ARM7 Microcontroller and uses DHT11, EEPROM, LCD, GSM Module, RTC, and Keypad for monitoring and secure user interaction.

✨ Features:
🔹 Real-Time Temperature Monitoring 🌡️.
🔹 Humidity Monitoring 💧.
🔹 GSM Based SMS Alert System 📱
🔹 EEPROM Data Storage 💾
🔹 Password Protected Access 🔐
🔹 RTC Timestamp Support ⏰
🔹 Local Parameter Configuration ⌨️
🔹 Remote Configuration Through SMS 📩
🔹 Fault Indication Using LEDs 🚨
🔹 Industrial Safety Monitoring 🏭
🛠 Hardware Components
⚡ LPC2148 ARM7 Microcontroller
🌡 DHT11 Sensor
📟 16×2 LCD Display
💾 AT24C256 EEPROM
📱 GSM Module (M660A)
⌨️ 4×4 Matrix Keypad
💡 LEDs
🔘 Switches
🔌 USB-UART Converter
💻 Software Used
🖥 Keil μVision
🔥 Flash Magic
⚙ Embedded C
📡 UART Communication
🔄 I2C Protocol

📂 Project Structure
INDUSGUARD
│
├── main.c
├── pro_main.c
├── dummy_pro_main.c
│
├── lcd.c
├── lcd.h
├── lcd1.c
│
├── delay.c
├── delay.h
│
├── dht11.c
├── dht11.h
│
├── GSM.c
├── GSM.h
│
├── UART_INT.c
├── UART_INT.h
├── uart0.h
│
├── rtc.c
├── rtc.h
├── rtc_defines.h
│
├── i2c_peripheral.c
├── i2c_eeprom.c
├── i2c_eeprom.h
├── i2c.h
├── i2c_defines.h
├── i2c_eeprom_defines.h
│
├── kpm.c
├── kpm.h
├── kpm_defines.h
│
├── edit.c
├── edit.h
│
├── exint.c
│
├── defines.h
├── types.h
│
└── README.md

⚙️ Working Principle
DHT11 Sensor
      │
      ▼
LPC2148 Microcontroller
      │
 ┌────┼────┬───────┐
 ▼    ▼    ▼       ▼
LCD EEPROM GSM    RTC
Display Storage SMS Timestamp
      │
      ▼
Fault Detection
      │
      ▼
Alert Message Sent

🏗️ Block Diagram
<img width="777" height="729" alt="image" src="https://github.com/user-attachments/assets/c8301b34-2728-48f4-b6b5-b9ce3e5714be" />


