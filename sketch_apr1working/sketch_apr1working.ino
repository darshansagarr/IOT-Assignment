#include <Wire.h>
#include <DFRobot_SHT20.h>

DFRobot_SHT20 sht20;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Use custom I2C pins for ESP32-S3: SDA = 38, SCL = 39
  Wire.begin(38, 39);
  Wire.setClock(100000);
  delay(100);

  Serial.println("Initializing SHT20 sensor...");
  sht20.initSHT20();
  delay(100); // Wait for sensor to stabilize

  float tempCheck = sht20.readTemperature();
  if (isnan(tempCheck)) {
    Serial.println("❌ Failed to initialize SHT20 sensor. Check wiring!");
    while (true) {
      delay(1000); // Stop here and show error
    }
  }

  Serial.println("✅ SHT20 sensor initialized successfully.");
}

void loop() {
  float temperature = sht20.readTemperature(); // °C
  float humidity = sht20.readHumidity();       // %

  if (!isnan(temperature) && !isnan(humidity)) {
    Serial.print("🌡 Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\t💧 Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.println("❌ Failed to read from SHT20 sensor.");
  }

  delay(2000); // 2 seconds delay
}
