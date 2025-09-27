#include <WiFi.h>

// WiFi Credentials
const char* ssid = "Airtel_swai_2660";  
const char* password = "air01436";  

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Print the MAC address once connected
  Serial.println();
  Serial.print("Connected to WiFi. MAC address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Your main code here
}

