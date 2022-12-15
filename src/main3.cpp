#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ConfigPortal8266.h>
#include <DHTesp.h>

char*               ssid_pfix = (char*)"test";
String              user_config_html =""
    "<p><input type='text' name='mqttServer' placeholders='mqtt_Broker'>";

const char*         mqttServer;
const int           mqttPort = 1883;
unsigned long       interval = 1000;
unsigned long       lastPublished = - interval;

float distance;
double mean = 0;
double sum = 0;

WiFiClient          espClient;
PubSubClient        client(espClient);

int Vo = A0; 
int V_LED = 12;   
int cnt =0; 

float Vo_value = 0;
float Voltage = 0;
float dustDensity = 0;
double average[20] = {0,};

void callback(char* topic, byte* payload, unsigned int length);

void pubStatus();

void setup() {
    Serial.begin(115200);
    pinMode(V_LED, OUTPUT); // 미세먼지센서
    pinMode(Vo, INPUT);
    loadConfig();
    
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
 
        if (client.connect("Dust")) {
            Serial.println("connected");  
        } else {
            Serial.print("failed with state "); Serial.println(client.state());
            delay(2000);
        }
    }
}

void loop() {
  client.loop();

  digitalWrite(V_LED,LOW);

  delayMicroseconds(280);

  Vo_value = analogRead(Vo); 

  delayMicroseconds(40);

  digitalWrite(V_LED,HIGH); 

  delayMicroseconds(9680);

  Voltage = Vo_value * 5.0 / 1023.0;

  dustDensity = (Voltage+0.1)/0.005;
  
  average[cnt % 20] = dustDensity;

  for(int j =0;j<20;j++){
    if(cnt > 20){
      if(average[j] > (mean*2)){
        sum += average[j-1];
      }else{
        sum += average[j];
      }
    }else
        sum += average[j];
  }

  mean = sum / 20;
  sum = 0;
  
  delay(1000);

  unsigned long currentMillis = millis();
  if(currentMillis - lastPublished >= interval) {
      lastPublished = currentMillis;
      pubStatus();
  }

  cnt++;
}

void pubStatus() {

    char dusty_buf[10];
    char dusty_den_buf[10];

    sprintf(dusty_buf,"%.2f", mean);
    sprintf(dusty_den_buf,"%.2f", dustDensity);


    client.publish("id/Void/LCR/Avg_dust", dusty_buf);
    client.publish("id/Void/LCR/Dust", dusty_den_buf);
    Serial.printf("Dust Density : %f, %f\n",dustDensity,mean);
}

void callback(char* topic, byte* payload, unsigned int length){

}