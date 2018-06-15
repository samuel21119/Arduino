// MCU To MPU
#include <Bridge.h>
#include "Barometer.h"
#include <Wire.h>
// DHT
#include "DHT.h"
#define DHTPIN A0
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
//Variable
float Humidity, Temperature, Pressure, Dust;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000;//sampe 30s&nbsp;;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
Barometer Barometer1;
void setup() {
    Serial.begin(9600);
    pinMode(9, INPUT);
    pinMode(8, INPUT);
    dht.begin();
    Bridge.begin();
    Barometer1.init();
}

void loop() {
    lowpulseoccupancy = 0;
    // Get Temperature, Humidity, Pressure and Dust
    Humidity = dht.readHumidity();
    Temperature = dht.readTemperature();
    
    Pressure = Barometer1.bmp085GetTemperature(Barometer1.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
    Pressure = Barometer1.bmp085GetPressure(Barometer1.bmp085ReadUP());

    starttime = millis();
    duration = pulseIn(8, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
    // MCU To MPU
    Bridge.put("Humidity", String(Humidity));
    Bridge.put("Temperature", String(Temperature));
    Bridge.put("Pressure", String(Pressure / 100));
    while (!((millis()-starttime) >= sampletime_ms)) {}
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=&gt;100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Bridge.put("Dust", String(concentration));
    delay(1000);
}
