#include <Arduino.h>

const int trigPin = 13;
const int echoPin = 12;
long duration;
float distance;

void setup() {
    Serial.begin(115200);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    delay(500);
    Serial.println("starting...");
}

void loop() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.017;            // distance = duration / 29 / 2;

    Serial.printf("Duration = %6ld, Distance = %6.2fcm\n", duration, distance);
    delay(1000);
}