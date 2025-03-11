#include <Arduino.h>
#include <TM1637Display.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define BUTTON_PIN 17
#define MIC_PIN 4
#define BUZZER_PIN 16
#define CLK_PIN 21
#define DIO_PIN 22
#define WIFI_SSID "ssid"
#define WIFI_PASS "1q2w3e4r"
#define LINE_NOTIFY_TOKEN ""
WiFiClient client;

TM1637Display display(CLK_PIN, DIO_PIN);

unsigned long countdownStartTime = 0;
const unsigned long countdownDuration = 0.05 * 60 * 1000;
bool countdownActive = false;

void wifi_connect();
void line_notify();
void triggerAlarm();

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    
    // display.setBrightness(7);
    // display.showNumberDecEx(500, 0b01000000, true);

    Serial.begin(115200);
    // wifi_connect();
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
      if (countdownActive) {
        Serial.println("Restart!");
        countdownActive = false;
        delay(300);
      }
      else {
        countdownStartTime = millis();
        countdownActive = true;
        Serial.println("Countdown started!");
        // display.showNumberDecEx(500, 0b01000000, true);
        delay(300);
      }
    }

    if (countdownActive) {
        unsigned long elapsed = millis() - countdownStartTime;
        long remaining = countdownDuration - elapsed;
        Serial.println("remaining: " + String(remaining));

        if (remaining > 0) {
            int minutes = remaining / 60000;
            int seconds = (remaining % 60000) / 1000;
            int displayTime = minutes * 100 + seconds;
            // display.showNumberDecEx(displayTime, 0b01000000, true);
        }

        int micValue = analogRead(MIC_PIN);
        Serial.println("micValue: " + String(micValue));
        if (micValue < 250) {
            Serial.println("Loud sound detected! Triggering alarm...");
            // triggerAlarm();
            countdownActive = false;
        }

        if (remaining <= 0) {
            Serial.println("Time up! Triggering alarm...");
            // triggerAlarm();
            countdownActive = false;
            // display.showNumberDec(0, true);
        }
        delay(200);
    }
}

void triggerAlarm() {
  Serial.println("High");
  for (int i=0; i<2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(1000);
  }
}

// void wifi_connect() {
//     WiFi.mode(WIFI_AP_STA);
//     WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//         digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     }
//     Serial.println("");
//     Serial.print("Connected to WiFi network with IP Address: ");
//     Serial.println(WiFi.localIP());
//     Serial.println(WiFi.macAddress());
//     Serial.println(WiFi.channel());
// }

// void line_notify() {
//     HTTPClient http;
//     char* message = "HAHAHA";
//     http.begin(lineNotifyEndpoint);
    
//     // Add headers
//     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//     http.addHeader("Authorization", String("Bearer ") + LINE_NOTIFY_TOKEN);
    
//     // Prepare the data in application/x-www-form-urlencoded format
//     String httpRequestData = "message=" + String(message);
    
//     // Send POST request
//     int httpResponseCode = http.POST(httpRequestData);
    
//     // Handle response
//     if (httpResponseCode > 0) {
//       String response = http.getString();
//       Serial.println("HTTP Response code: " + String(httpResponseCode));
//       Serial.println("Response: " + response);
//     } else {
//       Serial.print("Error sending HTTP POST: ");
//       Serial.println(httpResponseCode);
//     }
    
//     // Free resources
//     http.end();

// }