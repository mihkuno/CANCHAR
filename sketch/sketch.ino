#define RELAY 2
#define BUZZER 3
#define METAL 4

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD(0x27, 20, 4);

unsigned long endTime = 0;                // millis() timestamp when session ends
bool active = false;                      // true while timer > 0
bool metal_detect = false;                // tracks sensor state to detect edges
const unsigned long delayTime = 600000UL; // 10 minutes in milliseconds

void setup() {
  Serial.begin(9600);
  pinMode(METAL, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(RELAY, HIGH); // relay inactive (HIGH)
  digitalWrite(BUZZER, LOW);

  LCD.init();
  LCD.backlight();
  LCD.setCursor(0,0);
  LCD.print("Metal Detector");
  LCD.setCursor(0,1);
  LCD.print("Waiting...");
}

void loop() {
  int metal = !digitalRead(METAL); // adjust depending on sensor polarity
  unsigned long now = millis();

  // --- edge detection: no-metal -> metal : add time once per detection ---
  if (!metal_detect && metal) {
      metal_detect = true;

      // beep beep
      digitalWrite(BUZZER, HIGH);
      delay(120);
      digitalWrite(BUZZER, LOW);
      delay(120);
      digitalWrite(BUZZER, HIGH);
      delay(120);
      digitalWrite(BUZZER, LOW);

      // If there is still remaining time, extend it; otherwise start from now
      if (endTime > now) {
        endTime += delayTime;    // add on top of existing remaining time
      } else {
        endTime = now + delayTime; // start fresh
      }

      digitalWrite(RELAY, LOW);
      active = true;
  }


  // when metal is removed, allow next detection to add again
  if (metal_detect && !metal) {
    metal_detect = false;
  }

  // --- timer / UI / buzzer ---
  if (active) {
    unsigned long remainingSec = (endTime > now) ? (endTime - now) / 1000 : 0;

    // Display minutes if >= 60s, else seconds
    LCD.setCursor(0,1);
    if (remainingSec >= 60) {
      unsigned long mins = remainingSec / 60;
      unsigned long secs = remainingSec % 60;
      LCD.print("Time Left: ");
      LCD.print(mins);
      LCD.print("m ");
      if (secs < 10) LCD.print('0');
      LCD.print(secs);
      LCD.print("s   ");
    } else {
      LCD.print("Time Left: ");
      LCD.print(remainingSec);
      LCD.print(" sec   ");
    }

    // Buzzer pattern only in the last 5 seconds: odd seconds = beep, even = silent
    if (remainingSec <= 5 && remainingSec > 0) {
      if (remainingSec % 2 == 1) digitalWrite(BUZZER, HIGH); // 5,3,1 -> beep
      else                    digitalWrite(BUZZER, LOW);    // 4,2 -> silent
    } else {
      digitalWrite(BUZZER, LOW);
    }

    // finished
    if (remainingSec == 0) {
      active = false;
      digitalWrite(RELAY, HIGH);
      digitalWrite(BUZZER, LOW);
      LCD.setCursor(0,1);
      LCD.print("Waiting...       ");
      Serial.println("Session ended.");
    }
  }

  delay(200); // smooth updates
}
