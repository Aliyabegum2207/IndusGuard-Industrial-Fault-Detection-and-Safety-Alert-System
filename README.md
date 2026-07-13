# 🏭 INDUSGUARD – Industrial Fault Detection and Safety Alert System:

## 📌 Overview:
INDUSGUARD is an Embedded Systems project designed to improve industrial safety by continuously monitoring environmental conditions such as **temperature** and **humidity**. The system detects abnormal conditions in real time and immediately alerts the authorized user through **SMS** using a GSM module, helping prevent equipment damage, production loss, and fire hazards.
============================================================================================================================================
## 🚀 Features:

- 🌡️ Real-time Temperature Monitoring
- 💧 Real-time Humidity Monitoring
- 📟 LCD Display for Live Sensor Values
- 📲 SMS Alerts using GSM Module (M660A)
- 🔐 Password Protected System
- 💾 EEPROM Storage for Threshold Values
- ⏰ RTC Time Stamp for Alert Messages
- 🔔 Fault Indication using LED/Buzzer
- 📩 Remote Configuration through SMS
- ⚡ Local Configuration using Keypad & External Interrupt
============================================================================================================================================
## 🛠️ **Hardware Components**
- LPC2148 ARM7 Microcontroller
- DHT11 Temperature & Humidity Sensor
- LCD Display
- GSM Module (M660A)
- AT24C256 EEPROM
- 4×4 Matrix Keypad
- LEDs / Buzzer
- Push Button Switch
- USB-UART Converter / DB9 Cable
==========================================================================================================================================
```
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
```
