#include <Arduino.h>

int Vo = A0; 

int V_LED = 12;   
int cnt =0; 
  

float Vo_value = 0;

float Voltage = 0;

float dustDensity = 0;
double average[20] = {0,};
 

void setup(){

  Serial.begin(9600);

  pinMode(V_LED, OUTPUT);

  pinMode(Vo, INPUT);

}

 

void loop(){

  
  double sum=0;

  digitalWrite(V_LED,LOW);

  delayMicroseconds(280);

  Vo_value = analogRead(Vo); 

  delayMicroseconds(40);

  digitalWrite(V_LED,HIGH); 

  delayMicroseconds(9680);

  Voltage = Vo_value * 5.0 / 1023.0;

  dustDensity = (Voltage - 0.9)/0.005;
  
  average[cnt % 20] = dustDensity;
  for(int j =0;j<20;j++){
    sum += average[j];
  } 
  double mean = sum / 20;
  Serial.printf(" Dust Density: %lf \n",dustDensity);
  Serial.printf(" Average Dust Density : %lf\n",mean);
  

  delay(1000);

}