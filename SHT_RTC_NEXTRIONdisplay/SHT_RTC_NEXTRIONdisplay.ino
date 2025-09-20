
// working code for SHT,RTC and Nextion display
#include <Wire.h>
//#include <Wire1.h>
#include <DFRobot_SHT20.h>
#include <RTClib.h>

DFRobot_SHT20 sht20;
RTC_DS3231 rtc;

#define NEXTION Serial2  // Connect Nextion to TX2 (GPIO 17), RX2 (GPIO 16)

void setup() {
  Serial.begin(115200);
  NEXTION.begin(9600);  // Baud rate for Nextion
  delay(1000);

  // Initialize SHT20 on GPIO 9 (SDA), GPIO 8 (SCL)
  Wire.begin(10, 11);
  sht20.initSHT20();
  delay(100);

  // Initialize RTC on GPIO 21 (SDA), GPIO 19 (SCL)
  Wire1.begin(35, 19);
  if (!rtc.begin(&Wire1)) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set to compile time
  }

  Serial.println("Setup completed.");
}

void sendToNextion(String component, String value) {
  NEXTION.print(component);
  NEXTION.print(".txt=\"");
  NEXTION.print(value);
  NEXTION.print("\"");
  NEXTION.write(0xFF);
  NEXTION.write(0xFF);
  NEXTION.write(0xFF);
}

void loop() {
  // Read SHT20 data
  float temperature = sht20.readTemperature();
  float humidity = sht20.readHumidity();

  // Read time from RTC
  DateTime now = rtc.now();
  char timeBuffer[9];  // HH:MM:SS
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  // Debug print
  Serial.print("Time: "); Serial.println(timeBuffer);
  Serial.print("Temp: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");

  // Send data to Nextion
  sendToNextion("Clock", timeBuffer);                 // Clock
  sendToNextion("Temperature", String(temperature, 1));     // Temperature with 1 decimal
  sendToNextion("Humidity", String(humidity, 1));        // Humidity with 1 decimal

  delay(2000);  // Wait 2 seconds
}
