# ESP32 Predictive Maintenance Simulation

## English

### Description
This project simulates a predictive maintenance system using an ESP32 microcontroller[cite: 1]. It monitors machine vibration and environmental conditions (temperature and pressure) after verifying an authorized technician via an RFID scanner[cite: 1]. The system uses visual LED indicators to display the machine's health status in real-time[cite: 1].

### Features
*   **RFID Authentication:** Uses an MFRC522 module to read technician ID cards before initializing sensor readings[cite: 1].
*   **Vibration Monitoring:** Utilizes an MPU6050 accelerometer to calculate the active vibration magnitude (excluding static gravity)[cite: 1].
*   **Environmental Monitoring:** Uses a BMP180 (BMP085 compatible) sensor to read room temperature and pressure[cite: 1].
*   **Visual Status Indicators:** 
    *   🟢 **Green LED:** Normal operation (Vibration < 3.0 m/s²)[cite: 1].
    *   🟡 **Yellow LED:** Warning / Indication of anomaly (Vibration between 3.0 and 7.0 m/s²)[cite: 1].
    *   🔴 **Red LED:** Critical / High risk of downtime (Vibration >= 7.0 m/s²)[cite: 1].
*   **IoT Ready:** Contains configured (but currently commented-out) HTTP POST integration to send JSON data to a Firebase Realtime Database[cite: 1].

### Hardware Components
*   ESP32 DevKit V4[cite: 1]
*   MPU6050 (Accelerometer/Gyroscope)[cite: 1]
*   BMP180 (Temperature & Pressure Sensor)[cite: 1]
*   MFRC522 (RFID Reader)[cite: 1]
*   3x LEDs (Red, Yellow, Green)[cite: 1]
*   3x 330Ω Resistors[cite: 1]

### Pin Configuration
| Component | Pin / Connection |
| :--- | :--- |
| **MFRC522 (RFID)** | SDA/SS: GPIO 5, RST: GPIO 4, SCK: GPIO 18, MISO: GPIO 19, MOSI: GPIO 23[cite: 1] |
| **MPU6050 & BMP180** | SDA: GPIO 21, SCL: GPIO 22 (I2C)[cite: 1] |
| **Green LED** | GPIO 14[cite: 1] |
| **Yellow LED** | GPIO 12[cite: 1] |
| **Red LED** | GPIO 13[cite: 1] |

### Required Libraries
To run this project, ensure you have the following libraries installed:
*   `WiFi` & `HTTPClient`[cite: 1]
*   `SPI` & `Wire`[cite: 1]
*   `MFRC522`[cite: 1]
*   `Adafruit_MPU6050` & `Adafruit_Sensor`[cite: 1]
*   `Adafruit_BMP085`[cite: 1]
*   `ArduinoJson`[cite: 1]
