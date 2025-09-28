//SendercodeESPNOW

#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h" // Needed for esp_wifi_set_channel

// Ultrasonic Sensor Pins
#define TRIG_PIN 4
#define ECHO_PIN 5

// Data structure
typedef struct struct_message {
  float distance;
} struct_message;

struct_message dataToSend;

// Replace with your receiver’s MAC address
uint8_t peerAddress[] = { 0xD8, 0x3B, 0xDA, 0xA5, 0xC0, 0xE4 };

// Function to measure distance
float getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2.0;
}

// Send callback
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Not connecting to any AP

  // Set the Wi-Fi channel manually (match with receiver)
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE); // 🟢 Change to receiver's Wi-Fi channel

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 11;  // 🟢 Match with receiver's channel
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW Sender Ready");
}

void loop() {
  dataToSend.distance = getDistanceCM();

  esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));

  if (result == ESP_OK) {
    Serial.print("Sent distance: ");
    Serial.println(dataToSend.distance);
  } else {
    Serial.println("Error sending the data");
  }

  delay(1000);
}
