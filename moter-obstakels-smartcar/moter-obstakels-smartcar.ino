#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

// Definities en instellingen
#define TRIG_PIN A1
#define ECHO_PIN A0
#define MAX_DISTANCE 200
#define MAX_SPEED 190
#define SAFE_DISTANCE 30

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);
Servo myservo;

String inputString = "";    // Variabele om seriële input op te slaan
boolean goesForward = false;
boolean manualStop = false; // Handmatige stop via seriële input
int distance = 100;
int speedSet = 0;

void setup() {
  Serial.begin(9600); // Start seriële communicatie voor zowel seriële monitor als Bluetooth

  myservo.attach(10);
  myservo.write(115);
  delay(2000);
  distance = readPing();
  delay(100);

  pinMode(13, OUTPUT); // Optionele indicator-LED op pin 13
  moveStop();          // Zorg dat de auto initieel stopt
  Serial.println("Setup voltooid. Wacht op commando's...");
}

void loop() {
  handleSerialInput(); // Verwerk seriële of Bluetooth commando's

  if (manualStop) {
    // Als handmatige stop actief is, blijft de auto stilstaan
    moveStop();
    return; // Stop verdere uitvoering van de loop
  }

  // Normale obstakel-omzeilende logica
  int distanceR = 0;
  int distanceL = 0;
  delay(40);

  if (distance <= SAFE_DISTANCE) {
    moveStop();
    delay(50);
    moveBackward();
    delay(300);
    moveStop();
    delay(100);

    distanceR = lookRight();
    delay(100);
    distanceL = lookLeft();
    delay(100);

    if (distanceR >= distanceL) {
      turnRight();
      delay(300);
    } else {
      turnLeft();
      delay(300);
    }
  } else {
    moveForward();
  }
  distance = readPing();
}

// Verwerk seriële input om de auto handmatig te stoppen of starten
void handleSerialInput() {
  // Controleer of er iets via de seriële poort (monitor of Bluetooth) is ontvangen
  if (Serial.available()) {
    while (Serial.available()) {
      char inChar = (char)Serial.read();
      inputString += inChar;
    }
    processInput();
  }
}

// Verwerk het ontvangen commando
void processInput() {
  // Log het ontvangen commando voor debugging
  Serial.println("Ontvangen commando: '" + inputString + "'");

  // Verwijder onzichtbare tekens zoals newline of carriage return
  inputString.trim();

  // Verwerk het ontvangen commando
  if (inputString == "A" || inputString == "a") {
    manualStop = true; // Zet handmatige stop aan
    moveStop();
    Serial.println("Auto gestopt (handmatig).");
  } else if (inputString == "B" || inputString == "b") {
    manualStop = false; // Zet handmatige stop uit
    Serial.println("Auto hervat rijden.");
  } else {
    Serial.println("Ongeldig commando ontvangen: '" + inputString + "'");
  }

  // Wis de inputString voor de volgende invoer
  inputString = "";
}

// Servo-functies
int lookRight() {
  myservo.write(50);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(115);
  return distance;
}

int lookLeft() {
  myservo.write(170);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(115);
  return distance;
}

int readPing() {
  int total = 0;
  for (int i = 0; i < 3; i++) {
    total += sonar.ping_cm();
    delay(50);
  }
  int cm = total / 3;
  if (cm == 0) {
    cm = 250;
  }
  return cm;
}

// Motorbewegingen
void moveStop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void moveForward() {
  if (!goesForward) {
    goesForward = true;
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
    for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
      motor1.setSpeed(speedSet);
      motor2.setSpeed(speedSet);
      motor3.setSpeed(speedSet);
      motor4.setSpeed(speedSet);
      delay(5);
    }
  }
}

void moveBackward() {
  goesForward = false;
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet);
    motor3.setSpeed(speedSet);
    motor4.setSpeed(speedSet);
    delay(5);
  }
}

void turnRight() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  motor1.setSpeed(MAX_SPEED / 2);
  motor2.setSpeed(MAX_SPEED / 2);
  motor3.setSpeed(MAX_SPEED / 2);
  motor4.setSpeed(MAX_SPEED / 2);
  delay(700);
  moveStop();
}

void turnLeft() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  motor1.setSpeed(MAX_SPEED / 2);
  motor2.setSpeed(MAX_SPEED / 2);
  motor3.setSpeed(MAX_SPEED / 2);
  motor4.setSpeed(MAX_SPEED / 2);
  delay(700);
  moveStop();
}
