// #include <ESP8266WiFi.h>
// #include <PubSubClient.h>
// #include <DHTesp.h>
// #include <ConfigPortal8266.h>

// char*               ssid_pfix = (char*)"KoneJ";
// String              user_config_html = ""
//     "<p><input type='text' name='mqttServer' placeholder='mqtt Broker'>";

// DHTesp              dht;
// unsigned long       interval = 2000;
// unsigned long       lastDHTReadMillis = 0;
// float               humidity = 0;
// float               temperature = 0;

// const char*         mqttServer;
// const int           mqttPort = 1883;

// unsigned long       pubInterval = 5000;
// unsigned long       lastPublished = - pubInterval;

// WiFiClient espClient;
// PubSubClient client(espClient);
// void readDHT22();
// void callback(char* topic, byte* payload, unsigned int length);

// void setup() {
//     Serial.begin(115200);
//     WiFi.mode(WIFI_STA); 
//     loadConfig();
//     // *** If no "config" is found or "config" is not "done", run configDevice ***
//     if(!cfg.containsKey("config") || strcmp((const char*)cfg["config"], "done")) {
//         configDevice();
//     }
//     WiFi.begin((const char*)cfg["ssid"], (const char*)cfg["w_pw"]);
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     mqttServer = cfg["mqttServer"];
//     client.setServer(mqttServer, mqttPort);
//     client.setCallback(callback);
//     while (!client.connected()) {
//         Serial.println("Connecting to MQTT...");
 
//         if (client.connect("ESP8266Client")) {
//             Serial.println("connected");  
//         } else {
//             Serial.print("failed with state "); Serial.println(client.state());
//             delay(2000);
//         }
//     }
//     dht.setup(14, DHTesp::DHT22); // Connect DHT sensor to GPIO 14
//     client.subscribe("id/KoneJ/sensor/cmd");
// }

// void loop() {
//     client.loop();
//     delay(2000);
//     unsigned long currentMillis = millis();
//     if(currentMillis - lastPublished >= pubInterval) {
//         lastPublished = currentMillis;
//         readDHT22();
//         Serial.printf("%.1f\t %.1f\n", temperature, humidity);
//         char buf[10];
//         sprintf(buf, "%.1f", temperature);
//         client.publish("id/KoneJ/sensor/evt/temperature", buf);
//         sprintf(buf, "%.1f", humidity);
//         client.publish("id/KoneJ/sensor/evt/humidity", buf);
//     }
// }

// void readDHT22() {
//     unsigned long currentMillis = millis();

//     if(currentMillis - lastDHTReadMillis >= interval) {
//         lastDHTReadMillis = currentMillis;

//         humidity = dht.getHumidity();              // Read humidity (percent)
//         temperature = dht.getTemperature();        // Read temperature as Fahrenheit
//     }
// }

// void callback(char* topic, byte* payload, unsigned int length) {
 
//     char msgBuffer[20];
//     if(!strcmp(topic, "id/KoneJ/relay/cmd")) {
//         int i;
//         for(i = 0; i < (int)length; i++) {
//             msgBuffer[i] = payload[i];
//         } 
//         msgBuffer[i] = '\0';
//         Serial.printf("\n%s -> %s", topic, msgBuffer);
//         if(!strcmp(msgBuffer, "status")) {
//             lastPublished -= pubInterval;
//         }
//     }
// }
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <ConfigPortal8266.h>

char*               ssid_pfix = (char*)"KoneJ";
String              user_config_html = ""
    "<p><input type='text' name='mqttServer' placeholder='mqtt Broker'>";

DHTesp              dht;
unsigned long       interval = 2000;
unsigned long       lastDHTReadMillis = 0;
float               humidity = 0;
float               temperature = 0;
 
const char*         mqttServer;
const int           mqttPort = 1883;

unsigned long       pubInterval = 5000;
unsigned long       lastPublished = - pubInterval;

WiFiClient espClient;
PubSubClient client(espClient);
void readDHT22();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); 
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
    dht.setup(14, DHTesp::DHT22); // Connect DHT sensor to GPIO 14
    client.subscribe("id/KoneJ/sensor/cmd");
}

void loop() {
    client.loop();

    unsigned long currentMillis = millis();
    if(currentMillis - lastPublished >= pubInterval) {
        lastPublished = currentMillis;
        readDHT22();
        Serial.printf("%.1f\t %.1f\n", temperature, humidity);
        char buf[10];
        sprintf(buf, "%.1f", temperature);
        client.publish("id/KoneJ/sensor/evt/temperature", buf);
        sprintf(buf, "%.1f", humidity);
        client.publish("id/KoneJ/sensor/evt/humidity", buf);
        int light = analogRead(0);
        sprintf(buf, "%d", light);
        client.publish("id/KoneJ/sensor/evt/light", buf);
    }
}

void readDHT22() {
    unsigned long currentMillis = millis();

    if(currentMillis - lastDHTReadMillis >= interval) {
        lastDHTReadMillis = currentMillis;

        humidity = dht.getHumidity();              // Read humidity (percent)
        temperature = dht.getTemperature();        // Read temperature as Fahrenheit
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
 
    char msgBuffer[20];
    if(!strcmp(topic, "id/KoneJ/relay/cmd")) {
        int i;
        for(i = 0; i < (int)length; i++) {
            msgBuffer[i] = payload[i];
        } 
        msgBuffer[i] = '\0';
        Serial.printf("\n%s -> %s", topic, msgBuffer);
        if(!strcmp(msgBuffer, "status")) {
            lastPublished -= pubInterval;
        }
    }
}