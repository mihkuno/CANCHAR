#define RELAY_1 2
#define RELAY_2 3
#define METAL_1 4
#define METAL_2 5

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD_1(0x24, 16, 2);
LiquidCrystal_I2C LCD_2(0x25, 16, 2);

unsigned long metal1_endTime = 0;
unsigned long metal2_endTime = 0;
bool metal1_active = false;
bool metal2_active = false;
const unsigned long delayTime = 600000;  // 10 minutes (600,000 ms)

void setup() {
  Serial.begin(9600);

  pinMode(METAL_1, INPUT);
  pinMode(METAL_2, INPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);

  LCD_1.init();
  LCD_1.backlight();
  LCD_1.setCursor(0, 0);
  LCD_1.print("Metal Detector 1");

  LCD_2.init();
  LCD_2.backlight();
  LCD_2.setCursor(0, 0);
  LCD_2.print("Metal Detector 2");
}

void loop() {
  int metal_1 = !digitalRead(METAL_1);
  int metal_2 = !digitalRead(METAL_2);
  unsigned long currentMillis = millis();

  Serial.println("Metal1: " + String(metal_1) + "\t\t" + "Metal2: " + String(metal_2));

  // Metal 1 Detection
  if (metal_1) {
    if (!metal1_active) {
      digitalWrite(RELAY_1, LOW);
      metal1_active = true;
    }
    metal1_endTime = max(metal1_endTime, currentMillis) + delayTime;
  }

  // Metal 2 Detection
  if (metal_2) {
    if (!metal2_active) {
      digitalWrite(RELAY_2, LOW);
      metal2_active = true;
    }
    metal2_endTime = max(metal2_endTime, currentMillis) + delayTime;
  }

  // Timer display on LCD for Metal 1
  if (metal1_active) {
    unsigned long remainingTime = max(0, (metal1_endTime - currentMillis) / 1000);
    LCD_1.setCursor(0, 1);
    if (remainingTime >= 60) {
      LCD_1.print("Time Left: ");
      LCD_1.print(remainingTime / 60);
      LCD_1.print(" min  ");
    } else {
      LCD_1.print("Time Left: ");
      LCD_1.print(remainingTime);
      LCD_1.print(" sec  ");
    }

    if (currentMillis >= metal1_endTime) {
      digitalWrite(RELAY_1, HIGH);
      metal1_active = false;
      LCD_1.setCursor(0, 1);
      LCD_1.print("Waiting...     ");
    }
  }

  // Timer display on LCD for Metal 2
  if (metal2_active) {
    unsigned long remainingTime = max(0, (metal2_endTime - currentMillis) / 1000);
    LCD_2.setCursor(0, 1);
    if (remainingTime >= 60) {
      LCD_2.print("Time Left: ");
      LCD_2.print(remainingTime / 60);
      LCD_2.print(" min  ");
    } else {
      LCD_2.print("Time Left: ");
      LCD_2.print(remainingTime);
      LCD_2.print(" sec  ");
    }

    if (currentMillis >= metal2_endTime) {
      digitalWrite(RELAY_2, HIGH);
      metal2_active = false;
      LCD_2.setCursor(0, 1);
      LCD_2.print("Waiting...     ");
    }
  }

  delay(300); // Smooth LCD updates
}
