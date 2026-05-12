#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo gateIn;
Servo gateOut;

int trigPins[4] = {22, 24, 26, 28};
int echoPins[4] = {23, 25, 27, 29};

int redLED[4]  = {32, 33, 34, 35};
int greenLED[4]= {36, 37, 38, 39};

int irIn = 30;
int irOut = 31;

int buzzer = 40;

int emergencyBtn = 41;
int resetBtn = 42;

int freeSlots = 0;

long readDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  lcd.init();
  lcd.backlight();

  gateIn.attach(9);
  gateOut.attach(10);

  for (int i = 0; i < 4; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    pinMode(redLED[i], OUTPUT);
    pinMode(greenLED[i], OUTPUT);
  }

  pinMode(irIn, INPUT);
  pinMode(irOut, INPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(emergencyBtn, INPUT_PULLUP);
  pinMode(resetBtn, INPUT_PULLUP);

  gateIn.write(0);
  gateOut.write(0);
}

void loop() {

  freeSlots = 0;

  for (int i = 0; i < 4; i++) {
    long d = readDistance(trigPins[i], echoPins[i]);

    if (d < 10) { // occupied
      digitalWrite(redLED[i], HIGH);
      digitalWrite(greenLED[i], LOW);
    } else {
      digitalWrite(redLED[i], LOW);
      digitalWrite(greenLED[i], HIGH);
      freeSlots++;
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Free: ");
  lcd.print(freeSlots);

  lcd.setCursor(0, 1);
  lcd.print("Slots: 4");

  // intery
  if (digitalRead(irIn) == LOW) {
    if (freeSlots > 0) {
      gateIn.write(90);
      delay(2000);
      gateIn.write(0);
    } else {
      tone(buzzer, 1000);
      delay(2000);
      noTone(buzzer);
    }
  }

  // exit
  if (digitalRead(irOut) == LOW) {
    gateOut.write(90);
    delay(2000);
    gateOut.write(0);
  }

  // emergency
  if (digitalRead(emergencyBtn) == LOW) {
    gateIn.write(90);
    gateOut.write(90);
  }

  // reset
  if (digitalRead(resetBtn) == LOW) {
    gateIn.write(0);
    gateOut.write(0);
  }
}
