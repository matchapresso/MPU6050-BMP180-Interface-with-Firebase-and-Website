#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <ArduinoJson.h>
#include <math.h> // Tambahkan library math untuk perhitungan akar

//Konfigurasi WiFi & Firebase
const char* ssid = //"enter-your-ssid";
const char* password = //"enter-your-password";
const char* firebaseURL = //"https://your-firebase-link/sensor.json"; 

// --- Konfigurasi Pin ---
#define RST_PIN 4
#define SS_PIN 5

// Definisikan Pin LED
#define LED_HIJAU 14
#define LED_KUNING 27
#define LED_MERAH 13

// --- Inisialisasi Sensor ---
MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

bool isAuthorized = false;
String currentUser = "";

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
  turnOffAllLEDs(); // Pastikan mati saat awal
  
  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  // Inisialisasi SPI & RFID
  SPI.begin();
  rfid.PCD_Init();

  // Inisialisasi I2C
  Wire.begin();
  mpu.begin();
  bmp.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  
  Serial.println("Sistem Siap. Silakan Tap Kartu RFID Teknisi.");
}

void loop() {
  // 1. Cek RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String cardUID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      cardUID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUID += String(rfid.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase();
    
    // Asumsi semua kartu bisa masuk untuk simulasi ini
    isAuthorized = true;
    currentUser = cardUID;
    Serial.println("Akses Diterima. Membaca sensor...");
    rfid.PICC_HaltA(); 
  }

  // 2. Baca Sensor, Kontrol LED, & Kirim Data
  if (isAuthorized) {
    sensors_event_t a, g, temp_mpu;
    mpu.getEvent(&a, &g, &temp_mpu);
    
    float temperature = bmp.readTemperature();
    int32_t pressure = bmp.readPressure();

    // -- LOGIKA INDIKATOR LED (Simulasi Predictive Maintenance) --
    // Menghitung total magnitudo getaran: sqrt(x^2 + y^2 + z^2)
    // Gravitasi bumi normal adalah ~9.8 m/s^2. Cari selisihnya dari kondisi diam.
    float totalVibration = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
    float getaranAktif = abs(totalVibration - 9.8); // Menghilangkan bias gravitasi statis
    
    turnOffAllLEDs(); // Reset LED sebelum update status
    
    // -- TAMPILKAN HASIL BACAAN SENSOR DI SERIAL MONITOR --
    Serial.println("=========================================");
    Serial.print("Teknisi ID      : "); Serial.println(currentUser);
    
    Serial.println("--- Data MPU6050 (Vibrasi) ---");
    Serial.print("Accel X         : "); Serial.print(a.acceleration.x); Serial.println(" m/s^2");
    Serial.print("Accel Y         : "); Serial.print(a.acceleration.y); Serial.println(" m/s^2");
    Serial.print("Accel Z         : "); Serial.print(a.acceleration.z); Serial.println(" m/s^2");
    Serial.print("Getaran Aktif   : "); Serial.print(getaranAktif); Serial.println(" (Threshold)");
    
    Serial.println("--- Data BMP180 (Lingkungan) ---");
    Serial.print("Suhu Ruang      : "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Tekanan Ruang   : "); Serial.print(pressure); Serial.println(" Pa");
    Serial.println("=========================================");

    // Tentukan threshold (ambang batas). Nilai ini bisa kamu tuning sendiri di Wokwi
    if (getaranAktif < 3.0) {
      digitalWrite(LED_HIJAU, HIGH); // Mesin Normal
      Serial.println("Status: NORMAL");
    } 
    else if (getaranAktif >= 3.0 && getaranAktif < 7.0) {
      digitalWrite(LED_KUNING, HIGH); // Waspada Anomali
      Serial.println("Status: WASPADA (Indikasi Anomali)");
    } 
    else {
      digitalWrite(LED_MERAH, HIGH); // Kritis / Otw Downtime
      Serial.println("Status: KRITIS (Risiko Downtime!)");
    }

    // -- KIRIM DATA KE FIREBASE --
    StaticJsonDocument<200> doc;
    doc["teknisi_id"] = currentUser;
    doc["vibrasi_aktual"] = getaranAktif; // Mengirim magnitudo lebih efisien untuk Data Science awal
    doc["suhu_ruang"] = temperature;
    doc["tekanan_ruang"] = pressure;
    doc["status_led"] = (getaranAktif < 3.0) ? "Normal" : (getaranAktif < 7.0) ? "Waspada" : "Kritis";

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(firebaseURL);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonOutput);
      http.end();
    }

    // Biarkan LED menyala selama 3 detik agar terlihat hasilnya
    delay(3000); 
    
    turnOffAllLEDs(); // Matikan setelah selesai
    isAuthorized = false; 
    Serial.println("Menunggu tap kartu selanjutnya...");
  }
}
