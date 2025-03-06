#include <Arduino.h>
#include <TM1637Display.h>
#include <WiFi.h>
#include <HTTPClient.h>

# assume wai korn
#define BUTTON_PIN 18
#define MIC_PIN 34
#define BUZZER_PIN 5
#define CLK_PIN 21
#define DIO_PIN 22
#define WIFI_SSID "ssid"
#define WIFI_PASS "1q2w3e4r"
#define LINE_NOTIFY_TOKEN ""

WifiClient client;

TM1637Display display(CLK_PIN, DIO_PIN);

unsigned long countdownStartTime = 0;
const unsigned long countdownDuration = 5 * 60 * 1000;
bool countdownActive = false;
bool alarmTriggered = false;

void wifi_connect();
void line_notify();

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
    pinMode(BUZZER_PIN, OUTPUT);
    
    display.setBrightness(7);
    display.showNumberDecEx(500, 0b01000000, true);

    Serial.begin(115200);
    wifi_connect();
    // 
}

void loop() {
    if (digitalRead(BUTTON_PIN) == HIGH) {
        countdownStartTime = millis();
        countdownActive = true;
        alarmTriggered = false;
        Serial.println("Countdown started!");
        display.showNumberDecEx(500, 0b01000000, true);
        delay(300);
    }

    if (countdownActive) {
        unsigned long elapsed = millis() - countdownStartTime;
        unsigned long remaining = countdownDuration - elapsed;

        if (remaining > 0) {
            int minutes = remaining / 60000;
            int seconds = (remaining % 60000) / 1000;
            int displayTime = minutes * 100 + seconds;
            display.showNumberDecEx(displayTime, 0b01000000, true);
        }

        int micValue = analogRead(MIC_PIN);
        if (micValue > 2000) {
            Serial.println("Loud sound detected! Triggering alarm...");
            triggerAlarm();
            alarmTriggered = true;
        }

        if (remaining <= 0) {
            Serial.println("Time up! Triggering alarm...");
            triggerAlarm();
            countdownActive = false;
            display.showNumberDec(0, true);
        }

        if (alarmTriggered) {
            Serial.println("Reset!");
            countdownActive = false;
        }
    }
}

void wifi_connect() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
    Serial.println(WiFi.channel());
}

void line_notify() {
    HTTPClient http;
    char* message = "HAHAHA";
    http.begin(lineNotifyEndpoint);
    
    // Add headers
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", String("Bearer ") + LINE_NOTIFY_TOKEN);
    
    // Prepare the data in application/x-www-form-urlencoded format
    String httpRequestData = "message=" + String(message);
    
    // Send POST request
    int httpResponseCode = http.POST(httpRequestData);
    
    // Handle response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error sending HTTP POST: ");
      Serial.println(httpResponseCode);
    }
    
    // Free resources
    http.end();

}