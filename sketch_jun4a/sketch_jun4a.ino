void setup() {
  // Start the Serial Monitor at 115200 baud
  Serial.begin(115200);
  Serial.println("Hello, World from ESP32!");
}

void loop() {
  Serial.println("Hello, World2!");
  delay(1000); // Wait for 1 second
}

