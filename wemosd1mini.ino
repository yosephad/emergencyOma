#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// Ganti dengan kredensial Wi-Fi Anda
const char* ssid = "NFC0X";
const char* password = "k4m1nd0!@#";

const int sirenPin = 14;  // Pin GPIO14 (D5 pada board WEMOS D1 mini)
//const int wifiLedPin = 2; // Pin GPIO2 (D4 pada board WEMOS D1 mini) untuk LED Wi-Fi

void setup() {
  Serial.begin(115200);  // Inisialisasi komunikasi serial dengan baud rate 115200
  WiFi.begin(ssid, password);  // Mulai koneksi Wi-Fi

  // Inisialisasi pin sirene sebagai output
  pinMode(sirenPin, OUTPUT);
  digitalWrite(sirenPin, LOW); // Pastikan sirene mati saat memulai

  // Inisialisasi pin LED Wi-Fi sebagai output
  //pinMode(wifiLedPin, OUTPUT);
  //digitalWrite(wifiLedPin, LOW); // Matikan LED Wi-Fi saat memulai

  // Tunggu sampai terhubung ke Wi-Fi dengan LED berkedip
  Serial.print("Connecting to Wi-Fi");
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 15000; // Timeout 15 detik

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    //digitalWrite(wifiLedPin, HIGH); // Nyalakan LED
    //delay(250);                     // Tunggu 250 ms
    //digitalWrite(wifiLedPin, LOW);  // Matikan LED
    //delay(250);                     // Tunggu 250 ms
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected to Wi-Fi");
    //digitalWrite(wifiLedPin, HIGH); // Nyalakan LED Wi-Fi
  } else {
    Serial.println();
    Serial.println("Failed to connect to Wi-Fi");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) { // Periksa koneksi Wi-Fi
    WiFiClientSecure client;
    HTTPClient http;

    // Nonaktifkan verifikasi sertifikat (untuk tujuan pengujian)
    client.setInsecure();

    // Tentukan URL untuk GET request
    String url = "https://nfc02.id/alarmsystems/alarm.php?entity=receiver&location=bg&source=oma";

    // Mulai koneksi HTTPS menggunakan WiFiClientSecure dan URL
    http.begin(client, url);

    // Lakukan GET request
    int httpCode = http.GET();

    // Periksa respon HTTP
    if (httpCode > 0) {
      // Jika berhasil, baca payload
      String payload = http.getString();
      Serial.println("HTTP GET Response:");
      Serial.println(payload);

      // Periksa apakah respons adalah "true"
      if (payload == "true") {
        Serial.println("Alarm Triggered!");
        digitalWrite(sirenPin, HIGH);  // Nyalakan sirene
      } else {
        digitalWrite(sirenPin, LOW);   // Matikan sirene
      }
    } else {
      // Jika gagal, tampilkan pesan error
      Serial.print("Error on HTTP request: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end(); // Tutup koneksi HTTP
  } else {
    Serial.println("Wi-Fi Disconnected");
    //digitalWrite(wifiLedPin, LOW); // Matikan LED Wi-Fi jika terputus
  }

  delay(5000); // Delay 5 detik sebelum mengulang GET request
}
