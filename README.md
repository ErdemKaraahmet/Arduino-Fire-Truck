# Arduino-Fire-Truck
**🚒 Arduino FireTruck – Team 18**
  
  Term Project for Microproccesors course in Işık University by: Oğuzhdan Önder, Erdem Karaahmet and Emre Erdoğan 

  A Bluetooth-controlled, Arduino-based fire extinguishing robot car designed to approach and extinguish small fires from a safe distance.

**📋 Project Summary**

    • The Arduino FireTruck is a mobile, battery-powered firefighting vehicle controlled remotely via Bluetooth. It features:

    • A water tank and pump system to extinguish fires

    • Aiming servos for directional water spray

    • Obstacle avoidance using an ultrasonic sensor

    • Real-time water level monitoring via a sensor and LED indicators

    • Bluetooth control using the Dabble app and HC-05 module

    • EEPROM, interrupts, and timer usage for enhanced control and persistence



**🚗 Features**

    🔥 Fire extinguishing mechanism with water pump and directional servos

    🎮 Bluetooth control using the Dabble app Gamepad module

    🚫 Obstacle avoidance with ultrasonic sensor

    💧 Real-time water tank level monitoring with LEDs

    🧠 EEPROM persistence for servo angles and speed mode

    ⚙️ Use of interrupts (tank lid control) and timers (pump duration)

**🧱 Components Used**

    • Arduino Uno

    • HC-05 Bluetooth module

    • 4x DC RPM motors

    • L298N motor driver

    • 3x Servo motors

    • Water pump + silicone tubing

    • 5V Relay module

    • Ultrasonic sensor (HC-SR04)

    • Water level sensor

    • Red, Yellow, Green LEDs

    • AA batteries (16 total via 2x 4-AA and 1x 8-AA holders)

    • Breadboard, jumper wires, resistors, diode, capacitors, manual switch


**🛠️ Design Overview**
    • Mobility: 4 DC motors powered via L298N, controlled by the Arduino and powered by 8 AA batteries.

    • Aiming System: Two stacked servos adjust the water pump's spray direction (horizontal + vertical).

    • Water System: Pump controlled via a 5V relay, sourced from a 6V battery pack.

    • Water Tank Lid: Servo-controlled lid toggled by an external switch via interrupt.

    • Obstacle Avoidance: Ultrasonic sensor stops the vehicle when an obstacle is <10cm ahead.

    • Water Level Feedback: Analog water sensor + LEDs to indicate tank status (Low, Mid, Full).

    • Bluetooth Control: HC-05 + Dabble Gamepad allows for driving, aiming, and pump control.

**🧠 Software & Logic**
  **Libraries Used:**

    • Dabble.h (Bluetooth Gamepad)

    • Servo.h (Servo motors)

    • EEPROM.h (Persistent storage)

  **EEPROM Usage:**

    • Stores last known servo angles and speed mode to resume on power-up.

  **Timers:**

    • Used with millis() to time pump duration and prevent overrun 

  **Interrupts:**

    • External interrupt on digital pin to control the tank lid


