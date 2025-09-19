
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <DFRobot_SHT20.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// MQTT Broker settings
const char* mqtt_server = "";
const int mqtt_port = ;
const char* mqtt_topic = "";

// I2C custom pins
#define SDA_PIN 8
#define SCL_PIN 9

DFRobot_SHT20 sht20;
WiFiClient espClient;
PubSubClient client(espClient);

// NTP client for timestamp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // IST

unsigned long previousMillis = 0;
const long interval = 5000;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  sht20.initSHT20();
  delay(100);

  float testTemp = sht20.readTemperature();
  if (isnan(testTemp)) {
    Serial.println("❌ SHT20 not responding");
    while (true) delay(500);
  }

  setup_wifi();
  timeClient.begin();
  client.setServer(mqtt_server, mqtt_port);
}

void setup_wifi() {
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

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("✅ MQTT Connected");
    } else {
      Serial.print("❌ MQTT Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying...");
      delay(5000);
    }
  }
}

String getCurrentDate() {
  timeClient.update();
  time_t rawTime = timeClient.getEpochTime();
  struct tm* timeinfo = localtime(&rawTime);

  char buffer[11];
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
           timeinfo->tm_year + 1900,
           timeinfo->tm_mon + 1,
           timeinfo->tm_mday);
  return String(buffer);
}

void sendSensorData() {
  float temperature = sht20.readTemperature();
  float humidity = sht20.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Sensor error");
    return;
  }

  String dateStr =  getCurrentDate();
  String fileName = "sensor_data_" + dateStr + ".csv";

  StaticJsonDocument<256> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["timestamp"] = dateStr;
  doc["fileName"] = fileName;

  String jsonString;
  serializeJson(doc, jsonString);

  if (client.publish(mqtt_topic, jsonString.c_str())) {
    Serial.println("✅ MQTT Published");
  } else {
    Serial.println("❌ MQTT Publish Failed");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendSensorData();
  }
}
