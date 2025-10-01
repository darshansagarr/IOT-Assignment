// UART DO sensor pins
#define RXD2 16  // ESP32 GPIO16 → Sensor TX
#define TXD2 17  // ESP32 GPIO17 → Sensor RX

float DOX_VAL_SD = 0;
float DOX_VAL_TB = 0;

const float DO_MIN = 1.0;
const float DO_MAX = 10.0;
const float MAX_DELTA = 3.0;

float movingAvgBuffer[5] = {0};
int avgIndex = 0;
int avgCount = 0;

float getMovingAverage(float newVal) {
  movingAvgBuffer[avgIndex] = newVal;
  avgIndex = (avgIndex + 1) % 5;
  if (avgCount < 5) avgCount++;

  float sum = 0;
  for (int i = 0; i < avgCount; i++) {
    sum += movingAvgBuffer[i];
  }
  return sum / avgCount;
}

bool isValidDO(float current, float last) {
  return current >= DO_MIN && current <= DO_MAX &&
         (last == 0 || fabs(current - last) <= MAX_DELTA);
}

// 📡 Read DO from Atlas UART sensor via Serial2
float readDOsensor() {
  Serial2.flush(); // clear buffer
  Serial2.println("R");  // send Read command
  delay(1000);           // wait for response

  String response = "";
  while (Serial2.available()) {
    char c = Serial2.read();
    if (isPrintable(c)) {
      response += c;
    }
  }

  response.trim();  // clean up

  if (response.length() == 0) {
    Serial.println("⚠️ No response from DO sensor.");
    return -1;
  }

  // Example valid response: "6.45"
  float doValue = response.toFloat();
  return doValue;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  Serial.println("🧪 Atlas DO Sensor Filtering System Starts...");
}

void loop() {
  float raw_dox = readDOsensor();
  if (raw_dox < 0.0) {
    Serial.println("⚠️ Skipping due to invalid sensor read.");
    delay(2000);
    return;
  }

  float avg_dox = getMovingAverage(raw_dox);

  Serial.print("Raw DO: "); Serial.print(raw_dox, 2);
  Serial.print(" | Smoothed: "); Serial.print(avg_dox, 2);

  // ✅ SD Card Logging
  float final_sd_val = DOX_VAL_SD;
  if (isValidDO(avg_dox, DOX_VAL_SD)) {
    DOX_VAL_SD = avg_dox;
    final_sd_val = avg_dox;
    Serial.print(" | 📁 Saved to SD: ");
  } else {
    Serial.print(" | ❌ SD Rejected: ");
  }
  Serial.print(final_sd_val, 2);

  // ✅ ThingsBoard Sending
  float final_tb_val = DOX_VAL_TB;
  if (isValidDO(avg_dox, DOX_VAL_TB)) {
    DOX_VAL_TB = avg_dox;
    final_tb_val = avg_dox;
    Serial.print(" | ☁️ Sent to TB: ");
  } else {
    Serial.print(" | ❌ TB Filtered: ");
  }
  Serial.println(final_tb_val, 2);

  delay(2000); // Delay between reads
}