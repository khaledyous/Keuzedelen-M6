#include <AFMotor.h>
#include <NewPing.h>

// Ultrasone sensor pinnen
#define TRIG_PIN 12
#define ECHO_PIN 11
#define MAX_DISTANCE 200  // Maximale afstand in cm

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Motoren initialiseren
AF_DCMotor motor1(1); // Motor 1 op poort 1
AF_DCMotor motor2(2); // Motor 2 op poort 2
AF_DCMotor motor3(3); // Motor 3 op poort 3
AF_DCMotor motor4(4); // Motor 4 op poort 4

void setup() {
  Serial.begin(9600); // Start seriële monitor
  Serial.println("Robot start...");

  // Snelheid instellen voor motoren (0-255)
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
}

void loop() {
  int distance = sonar.ping_cm(); // Meet afstand in cm

  // Afstand op de seriële monitor weergeven
  if (distance > 0) {
    Serial.print("Afstand: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Geen meting...");
  }

  // Beslissingen op basis van afstand
  if (distance > 0 && distance < 20) { // Obstakel binnen 20 cm
    stopMotors();
    Serial.println("Obstakel gedetecteerd! Draaien...");
    turnLeft();  // Robot draait naar links
    delay(1000); // 1 seconde draaien
  } else {
    moveForward(); // Geen obstakel, vooruit rijden
  }

  delay(100); // Korte pauze
}

// Bewegingen voor motoren
void moveForward() {
  Serial.println("Vooruit...");
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void turnLeft() {
  motor1.run(BACKWARD); // Linker motoren achteruit
  motor2.run(BACKWARD);
  motor3.run(FORWARD);  // Rechter motoren vooruit
  motor4.run(FORWARD);
}

void stopMotors() {
  Serial.println("Stoppen...");
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
