#include <Wire.h>
#include <BH1750.h>

// Create a BH1750 object
BH1750 lightMeter;

// Define custom I2C pins
#define I2C_SDA 9
#define I2C_SCL 8

void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for serial monitor

  // Begin I2C with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize the sensor
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 sensor initialized successfully");
  } else {
    Serial.println("Error initializing BH1750 sensor. Please check wiring!");
    while (1);
  }
}

void loop() {
  // Read light level in lux
  float lux = lightMeter.readLightLevel();

  // Print the lux value to the Serial Monitor
  Serial.print("Light Level: ");
  Serial.print(lux);
  Serial.println(" lx");

  delay(1000); // Update every second
}
