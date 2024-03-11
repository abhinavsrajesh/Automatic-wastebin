#define BLYNK_TEMPLATE_ID "TMPL3SFsOwGHb"
#define BLYNK_TEMPLATE_NAME "waste"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include <Servo.h>

char auth[] = "V_cXPwt-ZkMsch-siste1TiJP9AOgn6q";
char ssid[] = "Wifi";
char pass[] = "matrix5005";

#define TRIGGER_PIN D1
#define ECHO_PIN D2
#define SERVO_PIN D3
#define IR_SENSOR_PIN D5
#define BUZZER_PIN D6
#define LIGHT_PIN D4

#define BIN_SIZE 25 // in centimeters
#define MAX_WASTE_LEVEL 90

Servo servo;
BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write(0); // Close the lid initially

  // Setup a function to be called every 1 second
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSensorData() {
  long duration, distance;
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1; // Convert distance to cm

  // Update the waste level gauge
  Blynk.virtualWrite(V0, distance);

  // Update the lid position gauge
  int lidPosition = servo.read();
  Blynk.virtualWrite(V1, lidPosition);

  // Check if the bin is almost full and activate buzzer if needed
  if (distance <= 2) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Check if IR sensor detects presence and adjust lid position accordingly
  int irState = digitalRead(IR_SENSOR_PIN);
  if (irState == HIGH && distance < MAX_WASTE_LEVEL) {
    openLid();
  } else {
    closeLid();
  }
}


void openLid() {
  servo.write(90); // Open the lid
  Blynk.virtualWrite(V1, 90); // Update gauge value
}

void closeLid() {
  servo.write(0); // Close the lid
  Blynk.virtualWrite(V1, 0); // Update gauge value
} 
