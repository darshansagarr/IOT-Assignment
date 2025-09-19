#include <WiFi.h>
#include <PubSubClient.h>

// WiFi Credentials
const char* ssid = "";  
const char* password = "";  

// ThingsBoard Credentials
const char* thingsboardServer = "demo.thingsboard.io";
const char* accessToken = "";  // Replace with your device's access token

// Pins for Ultrasonic Sensor
#define TRIG_PIN 4
#define ECHO_PIN 5

WiFiClient espClient;
PubSubClient client(espClient);

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
    if (client.connect("ESP32_Ultrasonic_Client", accessToken, "")) {
      Serial.println("Connected to ThingsBoard!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Function to measure distance
float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  float distance = duration * 0.034 / 2;

  return distance;
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectWiFi();
  client.setServer(thingsboardServer, 1883);
  connectThingsBoard();
}

void loop() {
  if (!client.connected()) {
    connectThingsBoard();
  }
  client.loop();

  float distance = readDistanceCM();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Create JSON payload
  String payload = "{\"distance\":" + String(distance, 2) + "}";

  // Publish to ThingsBoard
  client.publish("v1/devices/me/telemetry", payload.c_str());
  Serial.println("Data sent to ThingsBoard!");

  delay(1000); // Send every 1 second
} 
uint8_t peerAddress[] = { 0xD8, 0x3B, 0xDA, 0xA5, 0xC0, 0xE4 };
