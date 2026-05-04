# ESP32 Predictive Maintenance Simulation

## English

### Description
This project simulates a predictive maintenance system using an ESP32 microcontroller. It monitors machine vibration and environmental conditions (temperature and pressure) after verifying an authorized technician via an RFID scanner. The system uses visual LED indicators to display the machine's health status in real-time.

### Features
*   **RFID Authentication:** Uses an MFRC522 module to read technician ID cards before initializing sensor readings[cite: 1].
*   **Vibration Monitoring:** Utilizes an MPU6050 accelerometer to calculate the active vibration magnitude (excluding static gravity)[cite: 1].
*   **Environmental Monitoring:** Uses a BMP180 (BMP085 compatible) sensor to read room temperature and pressure[cite: 1].
*   **Visual Status Indicators:** 
    *   🟢 **Green LED:** Normal operation (Vibration < 3.0 m/s²).
    *   🟡 **Yellow LED:** Warning / Indication of anomaly (Vibration between 3.0 and 7.0 m/s²).
    *   🔴 **Red LED:** Critical / High risk of downtime (Vibration >= 7.0 m/s²).
*   **IoT Ready:** Contains configured (but currently commented-out) HTTP POST integration to send JSON data to a Firebase Realtime Database.

### Hardware Components
*   ESP32 DevKit V4
*   MPU6050 (Accelerometer/Gyroscope)
*   BMP180 (Temperature & Pressure Sensor)
*   MFRC522 (RFID Reader)
*   3x LEDs (Red, Yellow, Green)
*   3x 330Ω Resistors

### Pin Configuration
| Component | Pin / Connection |
| :--- | :--- |
| **MFRC522 (RFID)** | SDA/SS: GPIO 5, RST: GPIO 4, SCK: GPIO 18, MISO: GPIO 19, MOSI: GPIO 23 |
| **MPU6050 & BMP180** | SDA: GPIO 21, SCL: GPIO 22 (I2C) |
| **Green LED** | GPIO 14 |
| **Yellow LED** | GPIO 12 |
| **Red LED** | GPIO 13 |

### Required Libraries
To run this project, ensure you have the following libraries installed:
*   `WiFi` & `HTTPClient`
*   `SPI` & `Wire`
*   `MFRC522`
*   `Adafruit_MPU6050` & `Adafruit_Sensor`
*   `Adafruit_BMP085`
*   `ArduinoJson`
