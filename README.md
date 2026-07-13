## 🏭 **INDUSGUARD – Industrial Fault Detection and Safety Alert System**

## 📌 **Overview:**
INDUSGUARD is an Embedded Systems project designed to improve industrial safety by continuously monitoring environmental conditions such as **temperature** and **humidity**. The system detects abnormal conditions in real time and immediately alerts the authorized user through **SMS** using a GSM module, helping prevent equipment damage, production loss, and fire hazards.


## 🚀 **Features:**

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


## 🛠️ **Hardware Components:**
- LPC2148 ARM7 Microcontroller
- DHT11 Temperature & Humidity Sensor
- LCD Display
- GSM Module (M660A)
- AT24C256 EEPROM
- 4×4 Matrix Keypad
- LEDs / Buzzer
- Push Button Switch
- USB-UART Converter / DB9 Cable


## 💻 **Software Used:**

-🖥 Keil μVision 

-🔥 Flash Magic

-⚙ Embedded C

-📡 UART Communication

-🔄 I2C Protocol


## 📂 **Project Structure**

```
INDUSGUARD
│
├── main.c
│
├── lcd.c
├── lcd.h
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

## ⚙️ **Working Principle**
```
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
```


## 🏗️ **Block Diagram**

<img width="777" height="729" alt="image" src="https://github.com/user-attachments/assets/e5734185-75cd-49bd-b2da-5b3676ffacfa" />



## 🔌 **Peripheral Interfaces**


| Peripheral         | Interface                  |
| ------------------ | -------------------------- |
| GSM Module (M660A) | UART                       |
| AT24C256 EEPROM    | I2C                        |
| DHT11 Sensor       | GPIO                       |
| LCD Display        | GPIO                       |
| 4×4 Matrix Keypad  | GPIO                       |
| RTC                | Internal RTC               |
| Switch             | External Interrupt (EINT0) |




## 📱 **SMS Commands**

🌡 Change Temperature Threshold
```
0786T38$
```
📞 Update Mobile Number
```
0786M9876543210$
```
📊 Request Sensor Information
```
0786I$
```


## 🔐 **Security Features**

✅ Password Protected Access 🔑

✅ Authorized Mobile Number Verification 📱

✅ EEPROM-Based Secure Storage 💾

✅ Wrong Password Detection 🚫

✅ System Lock After Multiple Incorrect Attempts 🔒


## 🚀 **Modules Implemented**

### 📟 LCD Interface

- Character Display
- String Display
- Integer Display

### 🌡 DHT11 Driver

- Temperature Reading   
- Humidity Reading  

### 📱 GSM Module

- SMS Send
- SMS Receive
- AT Commands

### 💾 EEPROM Interface

- Byte Write
- Byte Read
- Page Write
- Sequential Read

### ⌨️ Keypad Interface

- Password Entry
- Menu Navigation

### ⏰ RTC Module

- Date and Time Stamp

### 📡 UART Interrupt

- Interrupt Driven Communication

### 🔄 I2C Communication

- EEPROM Communication

### 🚨 External Interrupt

- Local Configuration Mode


## 🎯 **Applications:**

🏭 Industrial Monitoring

⚙ Machine Safety Systems

🌡 Temperature Monitoring

💧 Humidity Monitoring

🚨 Fault Detection Systems

📱 Remote Alert Systems

🏢 Smart Factory Automation



## 👩‍💻 **Project Presented By**

### ALIYA BEGUM

B.Tech Electronics and Communication Engineering (ECE)

Vector India Major Project

Embedded Systems | LPC2148 | I2C





