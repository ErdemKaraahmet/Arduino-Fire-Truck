#include <Dabble.h>
#include <Servo.h>
#include <EEPROM.h>

Servo bottomServo;
Servo topServo;
Servo tankDoorServo;

int bottomServoValue = 90;
int topServoValue = 90;

#define BOTTOM_SERVO_ADDR 0
#define TOP_SERVO_ADDR 1
#define SPEED_MODE_ADDR 2

#define TRIG_PIN A1
#define ECHO_PIN A2

#define ENA 3
#define IN1 6
#define IN2 7
#define ENB 5
#define IN3 12
#define IN4 11

int speed = 400;
bool isFastMode = true;
bool lastSelectState = false;

int PumpPin = 8;
bool pumpRunning = false;
unsigned long pumpStartTime = 0;
const unsigned long pumpDuration = 5000;
bool lastStartState = false;

int waterSensorPin = A0;

int redLed = 13;
int yellowLed = A3;
int greenLed = A4;

volatile bool moveable = true;
volatile bool dryrun = false;

const int tankButtonPin = 2;
volatile bool doorShouldBeOpen = false;
volatile bool doorStateChanged = false;

unsigned long lastSensorCheck = 0;
const unsigned long sensorInterval = 300;

// ===== Door Button ISR =====
void tankButtonISR() {
  doorShouldBeOpen = (digitalRead(tankButtonPin) == LOW);
  doorStateChanged = true;
}

void setup() {
  Serial.begin(9600);
  Dabble.begin(Serial);

  bottomServoValue = EEPROM.read(BOTTOM_SERVO_ADDR);
  topServoValue = EEPROM.read(TOP_SERVO_ADDR);

  byte savedMode = EEPROM.read(SPEED_MODE_ADDR);
  isFastMode = (savedMode == 1);
  speed = isFastMode ? 255 : 120;

  if (bottomServoValue < 0 || bottomServoValue > 180) bottomServoValue = 90;
  if (topServoValue < 0 || topServoValue > 180) topServoValue = 90;

  bottomServo.attach(9);
  topServo.attach(10);
  bottomServo.write(bottomServoValue);
  topServo.write(topServoValue);

  tankDoorServo.attach(A5);
  tankDoorServo.write(0);  
  pinMode(tankButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(tankButtonPin), tankButtonISR, CHANGE);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(PumpPin, OUTPUT);
  digitalWrite(PumpPin, HIGH);

  stopMotors();
}

void loop() {
  if (doorStateChanged) {
    doorStateChanged = false;
    if (doorShouldBeOpen) {
      tankDoorServo.write(90);
      Serial.println("Door Opened");
    } else {
      tankDoorServo.write(0);
      Serial.println("Door Closed");
    }
  }

  Dabble.processInput();

  if (millis() - lastSensorCheck >= sensorInterval) {
    lastSensorCheck = millis();

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    float distance = duration * 0.034 / 2;
    moveable = (distance >= 10);

    int waterLevel = analogRead(waterSensorPin);
    dryrun = (waterLevel < 300);

    if (waterLevel > 500) {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);
      digitalWrite(yellowLed, LOW);
    } else if (waterLevel > 300) {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, HIGH);
    } else {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, LOW);
    }
  }

  bool currentSelectState = GamePad.isSelectPressed();
  if (currentSelectState && !lastSelectState) {
    isFastMode = !isFastMode;
    speed = isFastMode ? 255 : 120;
    EEPROM.write(SPEED_MODE_ADDR, isFastMode ? 1 : 0);
  }
  lastSelectState = currentSelectState;

  if (GamePad.isTrianglePressed()) {
    if (bottomServoValue < 120) {
      bottomServoValue++;
      bottomServo.write(bottomServoValue);
      EEPROM.write(BOTTOM_SERVO_ADDR, bottomServoValue);
    }
    delay(50);
  } else if (GamePad.isCrossPressed()) {
    if (bottomServoValue > 60) {
      bottomServoValue--;
      bottomServo.write(bottomServoValue);
      EEPROM.write(BOTTOM_SERVO_ADDR, bottomServoValue);
    }
    delay(50);
  } else if (GamePad.isSquarePressed()) {
    if (topServoValue < 140) {
      topServoValue++;
      topServo.write(topServoValue);
      EEPROM.write(TOP_SERVO_ADDR, topServoValue);
    }
    delay(50);
  } else if (GamePad.isCirclePressed()) {
    if (topServoValue > 50) {
      topServoValue--;
      topServo.write(topServoValue);
      EEPROM.write(TOP_SERVO_ADDR, topServoValue);
    }
    delay(50);
  }

  if (GamePad.isUpPressed() && moveable) {
    moveForward(speed);
  } else if (GamePad.isDownPressed()) {
    moveBackward(speed);
  } else if (GamePad.isLeftPressed()) {
    turnLeft(speed);
  } else if (GamePad.isRightPressed()) {
    turnRight(speed);
  } else {
    stopMotors();
  }

  if (!dryrun) {
    bool currentStartState = GamePad.isStartPressed();
    if (currentStartState && !lastStartState && !pumpRunning) {
      pumpRunning = true;
      pumpStartTime = millis();
      digitalWrite(PumpPin, LOW);
    }

    if (pumpRunning && (millis() - pumpStartTime >= pumpDuration)) {
      digitalWrite(PumpPin, HIGH);
      pumpRunning = false;
    }

    lastStartState = currentStartState;
  }
}

// ----- Motor Functions -----
void moveForward(int speed) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}
void moveBackward(int speed) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}
void turnLeft(int speed) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}
void turnRight(int speed) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}
void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0); analogWrite(ENB, 0);
}
