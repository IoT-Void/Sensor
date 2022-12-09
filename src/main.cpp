#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <ConfigPortal8266.h>
#include <Arduino.h>

const int trigPin = 13;
const int echoPin = 12;
long duration;
float distance;
char*               ssid_pfix = (char*)"KoneJ";
String              user_config_html = ""
    "<p><input type='text' name='mqttServer' placeholder='mqtt Broker'>";

unsigned long       interval = 2000;
unsigned long       lastDHTReadMillis = 0;
const char*         mqttServer;
const int           mqttPort = 1883;
unsigned long       pubInterval = 5000;
unsigned long       lastPublished = - pubInterval;
WiFiClient espClient;
PubSubClient client(espClient);
void callback(char* topic, byte* payload, unsigned int length);
void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); 
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    loadConfig();
    // *** If no "config" is found or "config" is not "done", run configDevice ***
    if(!cfg.containsKey("config") || strcmp((const char*)cfg["config"], "done")) {
        configDevice();
    }
    WiFi.begin((const char*)cfg["ssid"], (const char*)cfg["w_pw"]);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    mqttServer = cfg["mqttServer"];
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
 
        if (client.connect("ESP8266Client")) {
            Serial.println("connected");  
        } else {
            Serial.print("failed with state "); Serial.println(client.state());
            delay(2000);
        }
    }
}

void loop() {
    client.loop();

    unsigned long currentMillis = millis();
    if(currentMillis - lastPublished >= pubInterval) {
        char buf[10];
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        duration = pulseIn(echoPin, HIGH);
        distance = duration * 0.017;            // distance = duration / 29 / 2;

        Serial.printf("Duration = %6ld, Distance = %6.2fcm\n", duration, distance);
        sprintf(buf,"%lf",distance);
        client.publish("id/Void/sensor/first", buf);
    }
}


void callback(char* topic, byte* payload, unsigned int length) {

}