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
int cnt=0;
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
void pubStatus_dht22();
void pub_distance();
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
 
        if (client.connect("Outside")) {
            Serial.println("connected");  
        } else {
            Serial.print("failed with state "); Serial.println(client.state());
            delay(2000);
        }
    }
}

void loop() {
    client.loop();
    cnt++;
    unsigned long currentMillis = millis();
    if(currentMillis - lastPublished >= interval) {
        lastPublished = currentMillis;
        pub_distance();
        if(cnt % 50 == 0){
            pubStatus_dht22();
        }
    }
}
void pub_distance(){
    char dist_buf[10];    
    digitalWrite(trigPin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.017;
    sprintf(dist_buf,"%.2f",distance);

    if(distance < 100){
        client.publish("id/Void/Heesane/Outside",dist_buf);
        Serial.printf("A Detected\n");
        delay(1000);
    }
}
void pubStatus_dht22() {
    temperature = dht.getTemperature();
    humidity = dht.getHumidity();

    char tem_buf[10];
    char hum_buf[10];

    sprintf(tem_buf,"%.2f",temperature);
    sprintf(hum_buf,"%.2f",humidity);
    
    client.publish("id/Void/Heesane/Temperature", tem_buf);
    client.publish("id/Void/Heesane/Humidity", hum_buf);

    Serial.printf("Temperautre : %f, Humidity : %f\n",temperature,humidity);
}

void callback(char* topic, byte* payload, unsigned int length){

}

