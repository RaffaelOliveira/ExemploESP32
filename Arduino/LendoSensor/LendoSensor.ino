#include <Adafruit_Sensor.h>
#include <DHT.h>


const int DHTPIN  = 4;

//#define DHTTYPE DHT11
//#define DHTTYPE DHT22
#define DHTTYPE DHT21


DHT dht(DHTPIN, DHTTYPE);

float humidity;
float humidity_atual;
float temperatura;
float temperatura_atual;


void setup() {
  delay(1000);
  Serial.begin(9600);
  
  dht.begin();
}

void loop() {
  sensors_event_t event;
  humidity = dht.readHumidity();
  temperatura = dht.readTemperature();
  
  Serial.println(humidity);
  Serial.print(" ");
  Serial.print(temperatura);

  delay(500);
}
