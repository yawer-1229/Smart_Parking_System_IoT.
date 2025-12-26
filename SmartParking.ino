#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int TRIG_PIN = 2;
const int ECHO_PIN = 3;
const int BUZZER_PIN = 6;
const int LED_PIN = 13;

const int PARKING_SLOT_DEPTH = 30;
const int DANGER_ZONE = 10;
const int REQUIRED_HITS = 3;

LiquidCrystal_I2C lcd(0x27, 16, 2);

int stableCount = 0;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SYSTEM CALIBRATING");
  delay(1000);
  lcd.clear();
}

void loop() {
  long duration;
  int distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < PARKING_SLOT_DEPTH) {
    stableCount++;
  } else {
    stableCount = 0;
  }

  if (stableCount >= REQUIRED_HITS) {
    updateDisplay(distance);
    handleAlerts(distance);
    stableCount = REQUIRED_HITS;
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Status: FREE    ");
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  delay(50);
}

void updateDisplay(int dist) {
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(dist);
  lcd.print("cm   ");

  lcd.setCursor(0, 1);
  lcd.print("Status: OCCUPIED");
}

void handleAlerts(int dist) {
  if (dist < DANGER_ZONE) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, HIGH);
    int beepSpeed = map(dist, 10, 30, 20, 100);
    delay(beepSpeed);
    digitalWrite(BUZZER_PIN, LOW);
  }
}