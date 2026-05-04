#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <ArduinoJson.h> 
#include <math.h>

// --- Konfigurasi WiFi & Firebase ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
// Pastikan URL ini sudah benar sesuai project-mu
const char* firebaseURL = "https://logdatatp2-default-rtdb.firebaseio.com/sensor.json"; 

// --- Definisikan Pin LED ---
#define LED_HIJAU 14
#define LED_KUNING 27 // PIN 27 agar aman dari Bootloop (sebelumnya pin 12)
#define LED_MERAH 13

// --- Inisialisasi Sensor ---
Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

// Fungsi untuk mereset semua LED menjadi mati
void turnOffAllLEDs() {
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_KUNING, LOW);
  digitalWrite(LED_MERAH, LOW);
}

void setup() {
  Serial.begin(115200);
  
  // Set Pin LED sebagai Output
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_KUNING, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  turnOffAllLEDs();
  
  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Inisialisasi I2C (MPU6050 & BMP180)
  Wire.begin();
  
  if (!mpu.begin()) {
    Serial.println("Gagal menemukan MPU6050!");
    while (1) { delay(10); } // Berhenti jika sensor tidak ada
  }
  if (!bmp.begin()) {
    Serial.println("Gagal menemukan BMP180!");
    while (1) { delay(10); }
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  
  Serial.println("Sistem Siap. Memulai Data Logging Otomatis...");
  Serial.println("=========================================");
}

void loop() {
  // 1. Baca Sensor MPU6050 & BMP180
  sensors_event_t a, g, temp_mpu;
  mpu.getEvent(&a, &g, &temp_mpu);
  
  float temperature = bmp.readTemperature();
  int32_t pressure = bmp.readPressure();

  // 2. Logika Indikator LED (Simulasi Predictive Maintenance)
  // Menghitung total magnitudo getaran dikurangi gravitasi
  float totalVibration = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
  float getaranAktif = abs(totalVibration - 9.8); 
  
  turnOffAllLEDs(); // Reset LED
  String statusMesin = "";

  if (getaranAktif < 3.0) {
    digitalWrite(LED_HIJAU, HIGH); 
    statusMesin = "Normal";
  } 
  else if (getaranAktif >= 3.0 && getaranAktif < 7.0) {
    digitalWrite(LED_KUNING, HIGH); 
    statusMesin = "Waspada";
  } 
  else {
    digitalWrite(LED_MERAH, HIGH); 
    statusMesin = "Kritis";
  }

  // 3. Tampilkan di Serial Monitor
  Serial.print("Vibrasi Aktif: "); Serial.print(getaranAktif);
  Serial.print(" | Suhu: "); Serial.print(temperature);
  Serial.print("C | Status: "); Serial.println(statusMesin);

  // 4. Buat Format JSON dan Kirim ke Firebase
  StaticJsonDocument<200> doc;
  doc["mesin_id"] = "Pompa-Utama-01"; // Mengganti teknisi_id menjadi ID Mesin statis
  doc["vibrasi_aktual"] = getaranAktif; 
  doc["suhu_ruang"] = temperature;
  doc["tekanan_ruang"] = pressure;
  doc["status"] = statusMesin;

  String jsonOutput;
  serializeJson(doc, jsonOutput);
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(firebaseURL);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(jsonOutput);
    if (httpResponseCode > 0) {
      Serial.print("Data terkirim (HTTP "); Serial.print(httpResponseCode); Serial.println(")");
    } else {
      Serial.println("Gagal mengirim data.");
    }
    http.end();
  }

  // 5. Jeda sebelum pengambilan data berikutnya
  // Set di 2000ms (2 detik) agar Firebase tidak ter-spam terlalu cepat, tapi cukup untuk dataset
  delay(250); 
}