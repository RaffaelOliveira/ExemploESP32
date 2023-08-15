#include <WiFi.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <DHT.h>

#define DHTPIN 4  
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "RAFFAEL";
const char* password = "raffael3141";
int conectado = 0;
float humidityOld = 0;
float temperatureOld = 0;


void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Conectado ao AP com sucesso!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi conectado");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
  conectado = 1;
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Desconectado do ponto de acesso WiFi");
  Serial.print("WiFi perdeu a conexão. Motivo: ");
  //Serial.println(info.disconnected.reason);
  Serial.println("Tentando reconectar");
  WiFi.begin(ssid, password);
}


void setup(){
  conectado = 0;
  Serial.begin(9600);
  dht.begin();
  delay(1000);
  
  WiFi.disconnect(true);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.begin(ssid, password);
    
  Serial.println();
  Serial.println();
  Serial.println("Aguarde pelo WiFi... ");
}



void loop(){
  delay(2000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int gravaAPI = 0;

  if (temperature!=temperatureOld || humidity!=humidityOld ) {
    humidityOld=humidity;
    temperatureOld=temperature;
    gravaAPI=1;
  } 

  // Verifique se alguma leitura falhou e saia se for o caso
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler o sensor DHT!");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
 
  char stringtemperature[50];
  char stringhumidity[50];
  sprintf(stringtemperature, "%.2f", temperature);
  sprintf(stringhumidity, "%.2f", humidity);

  char linkAPI[100];
  strcpy(linkAPI, "https://closetdaay.com.br/clima/api.asp?T=");
  strcat(linkAPI, stringtemperature);
  strcat(linkAPI, "&H=");
  strcat(linkAPI, stringhumidity);
  
  // && gravaAPI==1
  if (conectado==1 && gravaAPI==1) {
    HTTPClient http;   
    
    //https://closetdaay.com.br/clima/api.asp
    //http://localhost/clima/api.asp

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

    http.end();  //Free resources
  }

}
