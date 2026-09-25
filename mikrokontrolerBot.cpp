#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ==================== PENGATURAN WI-FI ====================
const char* ssid = "ISI";        // Nama Wi-Fi / Hotspot HP
const char* password = "ISI";   // Password Wi-Fi

// ==================== PENGATURAN TELEGRAM ====================
String botToken = "isi TOKEN"; // Token dari @BotFather
String chatId = "isi ID";                                     // ID dari @myidbot

// ==================== PENGATURAN PIN ====================
const int PIR_PIN = 13;      // Pin Sinyal PIR
const int LED_PIN = 26;      // Pin LED (Active-HIGH: HIGH = Nyala, LOW = Mati)
const int BUZZER_PIN = 14;   // Pin Passive Buzzer
const int BUTTON_PIN = 4;    // Pin Push Button 4-pin (D4)

// ==================== VARIABEL SISTEM ====================
bool isArmed = true;         // Status awal sistem (TRUE = Armed / Aktif)
int statusPIR = LOW;

// Variabel Debounce Tombol
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Cooldown Notifikasi Gerakan
unsigned long lastSendTime = 0;
const unsigned long cooldownTime = 10000; // Jeda 10 detik antar notifikasi gerakan

// Pengecekan Pesan Telegram
unsigned long lastTelegramCheck = 0;
const unsigned long telegramCheckInterval = 2000; // Cek pesan masuk setiap 2 detik
int64_t lastUpdateId = 0;

void bunyiPeringatanHatiHati() {
  // 30 kali perulangan x (250ms bunyi + 250ms diam = 500ms) = 15.000ms (15 detik total)
  for (int i = 0; i < 30; i++) {
    tone(BUZZER_PIN, 1000); // Bunyi nada 1000 Hz
    delay(250);             // Durasi bunyi (0.25 detik)
    noTone(BUZZER_PIN);     // Matikan suara
    delay(250);             // Durasi jeda diam (0.25 detik)
  }
}

void bunyiBeepArmed() {
  tone(BUZZER_PIN, 2000);
  delay(300);
  noTone(BUZZER_PIN);
}

void bunyiBeepDisarmed() {
  tone(BUZZER_PIN, 1500);
  delay(100);
  noTone(BUZZER_PIN);
  delay(100);
  tone(BUZZER_PIN, 1000);
  delay(100);
  noTone(BUZZER_PIN);
}

// ==================== LAGU IBU KITA KARTINI ====================
void playIbuKitaKartini() {
  // Frekuensi Nada (Hz)
  // C4=262, D4=294, E4=330, F4=349, G4=392, A4=440
  int melodi[] = {
    262, 294, 330, 349, 392, 330, 262, // I-bu ki-ta Kar-ti-ni
    294, 330, 349, 392, 330,           // pu-tri se-ja-ti
    262, 294, 330, 349, 392, 330, 262, // Pu-tri In-do-ne-sia
    294, 330, 349, 330, 262,           // na-ma-nya ha-rum
    349, 392, 440, 349, 392, 330,       // Wa-hai I-bu ki-ta
    349, 392, 440, 349, 392, 330,       // Kar-ti-ni pu-tri yang
    294, 330, 349, 330, 262            // mu-li-a...
  };

  int durasi[] = {
    350, 150, 300, 300, 500, 300, 300,
    400, 200, 300, 300, 700,
    350, 150, 300, 300, 500, 300, 300,
    400, 200, 300, 300, 700,
    400, 200, 300, 300, 500, 500,
    400, 200, 300, 300, 500, 500,
    400, 200, 300, 300, 800
  };

  int totalNada = sizeof(melodi) / sizeof(melodi[0]);

  for (int i = 0; i < totalNada; i++) {
    tone(BUZZER_PIN, melodi[i]);
    delay(durasi[i]);
    noTone(BUZZER_PIN);
    delay(50); // Jeda singkat antar-nada
  }
}

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    message.replace(" ", "%20");
    message.replace("\n", "%0A");

    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatId + "&text=" + message;

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("[Telegram] Pesan berhasil terkirim!");
    } else {
      Serial.print("[Telegram] Gagal mengirim pesan. Code: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("[Wi-Fi] Terputus, tidak dapat mengirim pesan.");
  }
}

void checkTelegramMessages() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = "https://api.telegram.org/bot" + botToken + "/getUpdates?offset=" + String((long long)(lastUpdateId + 1)) + "&limit=1&timeout=0";
  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    int updateIdIndex = response.indexOf("\"update_id\":");
    
    if (updateIdIndex != -1) {
      int commaIndex = response.indexOf(",", updateIdIndex);
      String updateIdStr = response.substring(updateIdIndex + 12, commaIndex);
      lastUpdateId = atoll(updateIdStr.c_str());

      String lowerResponse = response;
      lowerResponse.toLowerCase();

      if (lowerResponse.indexOf("putar lagu") != -1) {
        Serial.println("[Telegram] Perintah 'putar lagu' diterima!");
        sendTelegramMessage("🎵 Memutar lagu Ibu Kita Kartini di Buzzer...");
        playIbuKitaKartini();
      }
    }
  }
  http.end();
}

void checkButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    static int buttonState = HIGH;
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        isArmed = !isArmed;

        if (isArmed) {
          Serial.println("[MODE] Sistem di-ARMED (Aktif)");
          bunyiBeepArmed();
          digitalWrite(LED_PIN, LOW); // LED Standby OFF
          sendTelegramMessage("🔒 SISTEM DI-AKTIFKAN (ARMED)\nSensor PIR siap mendeteksi gerakan.");
        } else {
          Serial.println("[MODE] Sistem di-DISARMED (Nonaktif)");
          bunyiBeepDisarmed();
          digitalWrite(LED_PIN, LOW); // LED OFF
          sendTelegramMessage("🔓 SISTEM DI-NONAKTIFKAN (DISARMED)\nSensor PIR diabaikan.");
        }
      }
    }
  }
  lastButtonState = reading;
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, LOW); // LED Mati saat setup (Active-HIGH)

  Serial.println("--- Sistem Alarm PIR, Buzzer, Button & Telegram Siap ---");
  Serial.print("Menghubungkan ke Wi-Fi ");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nTerhubung ke Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  sendTelegramMessage("🟢 Sistem Keamanan ESP32 Online!\nStatus Saat Ini: 🔒 ARMED (Aktif)");
}

void loop() {
  checkButton();

  // Cek pesan baru dari Telegram setiap 2 detik
  if (millis() - lastTelegramCheck > telegramCheckInterval) {
    checkTelegramMessages();
    lastTelegramCheck = millis();
  }

  if (isArmed) {
    statusPIR = digitalRead(PIR_PIN);

    if (statusPIR == HIGH) {
      digitalWrite(LED_PIN, HIGH); // LED Nyala
      Serial.println("[!] GERAKAN TERDETEKSI!");
      bunyiPeringatanHatiHati();

      if (millis() - lastSendTime > cooldownTime || lastSendTime == 0) {
        sendTelegramMessage("⚠️ PERINGATAN!\nAda gerakan terdeteksi oleh Sensor PIR!");
        lastSendTime = millis();
      }
    } else {
      digitalWrite(LED_PIN, LOW); // LED Mati
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(50);
}
