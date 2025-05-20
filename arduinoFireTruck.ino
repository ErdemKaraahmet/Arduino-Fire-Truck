#include <Dabble.h>

#include <Servo.h>

Servo bottomServo;  // Horizontal (left-right)
Servo topServo;     // Vertical (up-down)
int bottomServoValue = 90;
int topServoValue = 90;

#define TRIG_PIN A1
#define ECHO_PIN A2

// Motor control pins
#define ENA 2
#define IN1 7
#define IN2 6

#define ENB 3
#define IN3 4
#define IN4 5

int speed = 200;

int waterSensorPin = A0;

int redLed = 13;
int yellowLed = 12;
int greenLed = 11;

void setup() {
  Serial.begin(9600);        // Start hardware Serial for Bluetooth
  Dabble.begin(Serial);      // Use Dabble with hardware Serial

  bottomServo.attach(9);   // Bottom servo to pin 9
  topServo.attach(8);     // Top servo to pin 8

  //Ultrasonic 
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  //Leds
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  // Setup motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();  // Prevent motors from running on boot
}

void loop() {
  Dabble.processInput();     // Refresh Dabble input


  //Speed Setting
  if(GamePad.isSelectPressed()) {
    if(speed >= 400) {
      speed = 200;
    }
    else {
      speed += 100;
    }
  }

  //Water
  int level = analogRead(waterSensorPin);
  Serial.print("Water Level: ");
  Serial.println(level);

  if (level > 500) {
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, HIGH);
  }
  else if (level > 300) {
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(greenLed, LOW);
  }
  else {
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  }

  //Ultrasonic 
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  float distance = duration * 0.034 / 2;

  if (distance > 2 && distance < 400) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Out of range");
  }

   //Servo Motors
  if(GamePad.isTrianglePressed()) {
    if(bottomServoValue < 120) {
    bottomServoValue++;
    }
    Serial.println("Bottom Servo ++");
    Serial.println(bottomServoValue);
    bottomServo.write(bottomServoValue);
    delay(50);
  } 
  else if (GamePad.isCrossPressed()){
    if(bottomServoValue > 60) {
    bottomServoValue--;
    }
    Serial.println("Bottom Servo --");
    Serial.println(bottomServoValue);
    bottomServo.write(bottomServoValue);
    delay(50);
  }
  else if (GamePad.isSquarePressed()) {
    if(topServoValue < 140) {
    topServoValue++;
    }
     Serial.println("topServoValue++");
     Serial.println(topServoValue);
    topServo.write(topServoValue);
    delay(50);
  }
  else if (GamePad.isCirclePressed()) {
    if(topServoValue > 50) {
    topServoValue--;
    }
     Serial.println("topServoValue--");
     Serial.println(topServoValue);
    topServo.write(topServoValue);
    delay(50);
  }


  // if (GamePad.isUpPressed() && distance > 30)   ---> to implement the distance limit
  if (GamePad.isUpPressed()) {
    moveForward(speed);
  } 
  else if (GamePad.isDownPressed()) {
    moveBackward(speed);
  } 
  else if (GamePad.isLeftPressed()) {
    turnLeft(speed);
  } 
  else if (GamePad.isRightPressed()) {
    turnRight(speed);
  } 
  else {
    stopMotors();
  }



}

// ---- Motor control functions ----

void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void moveBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnLeft(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnRight(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
