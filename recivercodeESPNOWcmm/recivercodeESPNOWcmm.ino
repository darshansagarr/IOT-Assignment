// recivercode
#include <esp_now.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Airtel_swai_2660";
const char* password = "air01436";
const char* mqttServer = "demo.thingsboard.io";
const int mqttPort = 1883;
const char* thingsboardToken = "vw7r2qJoo4VNOXp3wDKL";

WiFiClient espClient;
PubSubClient client(espClient);

// Struct to receive data
typedef struct struct_message {
  float distance;
} struct_message;

struct_message incomingData;

void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP32Client", thingsboardToken, NULL)) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 3 seconds");
      delay(3000);
    }
  }
}

// ESP-NOW receive callback
void onDataReceive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
  if (len == sizeof(struct_message)) {
    memcpy(&incomingData, data, sizeof(incomingData));
    Serial.print("Received Distance: ");
    Serial.println(incomingData.distance);

    if (!client.connected()) {
      connectToMQTT();
    }

    String payload = "{\"distance\":";
    payload += incomingData.distance;
    payload += "}";
    client.publish("v1/devices/me/telemetry", payload.c_str());
    Serial.println("Published to ThingsBoard: " + payload);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver ESP32 Booting...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  int currentChannel = WiFi.channel();
  Serial.print("WiFi Channel: ");
  Serial.println(currentChannel);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);

  client.setServer(mqttServer, mqttPort);
  connectToMQTT();
}

void loop() {
  client.loop();
}


