#include <Arduino.h>
#include <TM1637Display.h>

# assume wai korn
#define BUTTON_PIN 18
#define MIC_PIN 34
#define BUZZER_PIN 5
#define CLK_PIN 21
#define DIO_PIN 22

TM1637Display display(CLK_PIN, DIO_PIN);

unsigned long countdownStartTime = 0;
const unsigned long countdownDuration = 5 * 60 * 1000;
bool countdownActive = false;
bool alarmTriggered = false;

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
    pinMode(BUZZER_PIN, OUTPUT);
    
    display.setBrightness(7);
    display.showNumberDecEx(500, 0b01000000, true);

    Serial.begin(115200);
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