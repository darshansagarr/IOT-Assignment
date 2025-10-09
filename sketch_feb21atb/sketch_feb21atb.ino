#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "DFRobot_SHT20.h"

// WiFi Credentials
const char* ssid = "Airtel_swai_2660";  
const char* password = "air01436";  

// ThingsBoard Credentials
const char* thingsboardServer ="demo.thingsboard.io";//"cloud.thingsboard.io";     // "34.117.168.233";  //"192.168.1.18"; //"demo.thingsboard.io";  //  Change if using a local server
const char* accessToken = "C4oRQtVVpIcJbjw6pGGf";  // Replace with your device's access token

#define SDA_PIN 8
#define SCL_PIN 9

WiFiClient espClient;
PubSubClient client(espClient);
DFRobot_SHT20 sht20(&Wire);

// Function to connect to WiFi
void connectWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
}

// Function to connect to ThingsBoard
void connectThingsBoard() {
    while (!client.connected()) {
        Serial.print("Connecting to ThingsBoard...");
        if (client.connect("ESP32_Client", accessToken, "")) {
            Serial.println("Connected!");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Initialize SHT20 Sensor
    sht20.initSHT20();
    delay(100);
    Serial.println("SHT20 Sensor Initialized!");

    // Connect to WiFi
    connectWiFi();

    // Connect to ThingsBoard
    client.setServer(thingsboardServer, 1883);
    connectThingsBoard();
}

void loop() {
    if (!client.connected()) {
        connectThingsBoard();
    }
    client.loop();

    float temperature = sht20.readTemperature();  
    float humidity = sht20.readHumidity();       

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from SHT20 sensor!");
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Create JSON payload
    String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";

    // Publish data to ThingsBoard
    client.publish("v1/devices/me/telemetry", payload.c_str());

    Serial.println("Data sent to ThingsBoard chill man!");
    
    delay(1000); // Send data every 1 seconds
}

