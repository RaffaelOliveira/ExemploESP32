#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <Arduino.h>

#define DHTPIN 4  
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
const int sensorPin = 13;

const int pinoLED = 2;
const int pinoLEDAPI = 18;

const char* ssid     = "Raffael"; 
const char* password = "raffael3141";
float humidityOld = 0;
float temperatureOld = 0;

void setup()
{
    Serial.begin(9600);
    while(!Serial){delay(100);}
    dht.begin();
    pinMode(pinoLED, OUTPUT);
    pinMode(pinoLEDAPI, OUTPUT);
    digitalWrite(pinoLED, HIGH);
    pinMode(sensorPin, INPUT);
    delay(500);
 
    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(pinoLED, LOW);  
}



void loop(){
  int gravaAPI = 0;

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler o sensor DHT!");
    digitalWrite(pinoLED, HIGH);
    digitalWrite(pinoLEDAPI, HIGH); 
    delay(1000);
    digitalWrite(pinoLED, LOW);
    digitalWrite(pinoLEDAPI, LOW); 
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");

  if (temperature!=temperatureOld || humidity!=humidityOld ) {
    humidityOld=humidity;
    temperatureOld=temperature;
    gravaAPI=1;
  } 

  char linkAPI[100];
  char stringtemperature[50];
  char stringhumidity[50];
  sprintf(stringtemperature, "%.2f", temperature);
  sprintf(stringhumidity, "%.2f", humidity);
  strcpy(linkAPI, "https://closetdaay.com.br/clima/api.asp?T=");
  strcat(linkAPI, stringtemperature);
  strcat(linkAPI, "&H=");
  strcat(linkAPI, stringhumidity);

  if (gravaAPI==1) {
    HTTPClient http;   
    digitalWrite(pinoLEDAPI, HIGH);
    Serial.println(linkAPI);
    http.begin(linkAPI);  //Specify destination for HTTP request
    http.addHeader("Content-Type", "text/plain");             //Specify content-type header

    int httpResponseCode = http.POST("POSTING from ESP32");   //Send the actual POST request

    if(httpResponseCode>0){
      String response = http.getString();  
      Serial.println(response); 
    }else{
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    digitalWrite(pinoLEDAPI, LOW);
    http.end();  //Free resources
  }


  int sensorValue = analogRead(sensorPin);  // Ler o valor analógico do sensor
  float voltage = (sensorValue / 1023.0) * 3.3;  // Converter valor para tensão (considerando ESP32 3.3V)
  
  Serial.print("Valor do sensor: ");
  Serial.print(sensorValue);
  Serial.print("\tTensão: ");
  Serial.print(voltage);
  
    if (voltage < 1.0) {
    Serial.println("\tChuva detectada");
  } else {
    Serial.println("\tSem chuva");
  }


  delay(10000);
}







