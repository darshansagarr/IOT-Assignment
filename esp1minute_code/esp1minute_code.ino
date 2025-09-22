#include <Wire.h>
#include <DFRobot_SHT20.h>

DFRobot_SHT20 sht20;

float temperature;
float humidity;

void setup() {
  Serial.begin(9600);                                                   
  Serial.println("Setup done");
  Wire.begin(8, 9);  // SDA = GPIO 9, SCL = GPIO 8
  sht20.initSHT20();
}

void loop() {
  temperature = sht20.readTemperature();
  humidity = sht20.readHumidity();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(60000);  // Wait for 60 seconds (1 minute)
}
