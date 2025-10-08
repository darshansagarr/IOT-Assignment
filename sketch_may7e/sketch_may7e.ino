#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <DFRobot_SHT20.h>

// WiFi credentials
const char* ssid = "Airtel_swai_2660";
const char* password = "air01436";

// Backend server URL
const char* serverUrl = "http://192.168.1.32:3001/api/sensor-data/esp32-data";

// Define custom I2C pins for ESP32
#define SDA_PIN 8
#define SCL_PIN 9

DFRobot_SHT20 sht20;
WiFiClient espClient;

unsigned long previousMillis = 0;
const long interval = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting...");

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  // Initialize SHT20 sensor
  Serial.println("Initializing SHT20...");
  sht20.initSHT20();
  delay(100);

  float testTemp = sht20.readTemperature();
  if (isnan(testTemp)) {
    Serial.println("❌ Error: SHT20 sensor not responding. Check connections!");
    while (true) {
      delay(500); // Halt here
    }
  }

  Serial.println("✅ SHT20 Initialized Successfully");
  setup_wifi();
}

void setup_wifi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi connection failed.");
  }
}

void reconnect_wifi_if_needed() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    setup_wifi();
  }
}

void sendSensorData() {
  float temperature = sht20.readTemperature();
  float humidity = sht20.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Error reading from SHT20 sensor!");
    return;
  }

  Serial.printf("📡 Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);

  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;

  String jsonString;
  serializeJson(doc, jsonString);

  HTTPClient http;
  http.begin(espClient, serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);

  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {
    Serial.println("✅ HTTP POST Success");
    Serial.print("Response code: ");
    Serial.println(httpResponseCode);
    Serial.println("Response: " + http.getString());
  } else {
    Serial.print("❌ HTTP POST Failed. Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop() {
  reconnect_wifi_if_needed();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendSensorData();
  }
}

