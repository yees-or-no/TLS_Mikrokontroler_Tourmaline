#include <Arduino.h>
// tanpa telegrambot

const int PIR_PIN = 13;      
const int LED_PIN = 26;      
const int BUZZER_PIN = 14;   
const int BUTTON_PIN = 4;    

bool isArmed = true;         
int statusPIR = LOW;

int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void bunyiPeringatanHatiHati() {
  for (int i = 0; i < 30; i++) {
    tone(BUZZER_PIN, 1000); 
    delay(250);             
    noTone(BUZZER_PIN);     
    delay(250);             
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

void playIbuKitaKartini() {
  int melodi[] = {
    262, 294, 330, 349, 392, 330, 262, 
    294, 330, 349, 392, 330,           
    262, 294, 330, 349, 392, 330, 262, 
    294, 330, 349, 330, 262,           
    349, 392, 440, 349, 392, 330,       
    349, 392, 440, 349, 392, 330,       
    294, 330, 349, 330, 262            
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
    delay(50); 
  }
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
        } else {
          Serial.println("[MODE] Sistem di-DISARMED (Nonaktif)");
          bunyiBeepDisarmed();
          digitalWrite(LED_PIN, LOW); // LED OFF
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

  digitalWrite(LED_PIN, LOW); 

  Serial.println("--- Sistem Alarm PIR, Buzzer & Button Siap ---");
  Serial.println("Status Saat Ini: ARMED (Aktif)");
}

void loop() {
  checkButton();

  if (isArmed) {
    statusPIR = digitalRead(PIR_PIN);

    if (statusPIR == HIGH) {
      digitalWrite(LED_PIN, HIGH); 
      Serial.println("[!] GERAKAN TERDETEKSI!");
      bunyiPeringatanHatiHati();
    } else {
      digitalWrite(LED_PIN, LOW); 
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(50);
}
