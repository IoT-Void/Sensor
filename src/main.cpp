#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ConfigPortal8266.h>
#include <DHTesp.h>

char*               ssid_pfix = (char*)"test";
String              user_config_html =""
    "<p><input type='text' name='mqttServer' placeholders='mqtt_Broker'>";

const int trigPin = 13;
const int echoPin = 12;
long duration;
float distance;
const char*         mqttServer;
const int           mqttPort = 1883;
unsigned long       interval = 100;
unsigned long       lastPublished = - interval;

WiFiClient          espClient;
PubSubClient        client(espClient);
DHTesp              dht;
float               humidity = 0;
float               temperature = 0;
void callback(char* topic, byte* payload, unsigned int length);
void pubStatus();

void setup() {
    Serial.begin(115200);
    dht.setup(14, DHTesp::DHT22);
    loadConfig();
    pinMode(trigPin,OUTPUT);
    pinMode(echoPin,INPUT);

    if(!cfg.containsKey("config") || strcmp((const char*)cfg["config"],"done")){
        configDevice();
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin((const char*)cfg["ssid"],(const char*)cfg["w_pw"]);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    mqttServer = cfg["mqttServer"];

    Serial.println("Connected to the WiFi network");
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
 
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
 
        if (client.connect("Inside")) {
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
    if(currentMillis - lastPublished >= interval) {
        lastPublished = currentMillis;
        pubStatus();
    }
}

void pubStatus() {

    digitalWrite(trigPin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.017;

    char dist_buf[10];

    sprintf(dist_buf,"%f",distance);
    if(distance <100){
      printf("B Detected\n");
      client.publish("id/Void/KoneJ/Inside",dist_buf);
      delay(1000);
    }
    


    // Serial.printf("Temperautre : %f, Humidity : %f\n",temperature,humidity);
    Serial.printf("Distance = %6.2fcm\n",distance);
}

void callback(char* topic, byte* payload, unsigned int length){

}
