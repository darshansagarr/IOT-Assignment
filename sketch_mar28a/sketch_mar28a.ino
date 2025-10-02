#define receiver A0  // Solar panel as Li-Fi receiver
#define patternLength 4  // Number of expected pattern bits (e.g., 4 bits for 1010)
#define sampleDelay 200   // Match the pulse duration of the transmitter (200ms)

#define red1 3
#define yellow1 4
#define green1 5

#define red2 6
#define yellow2 7
#define green2 8

int expectedPattern[patternLength] = {1, 0, 1, 0};  // Expected 1010 sequence

void setup() {
    Serial.begin(9600);  // Initialize serial communication for output
    Serial.println("Li-Fi Signal Receiver Initialized");

    pinMode(red1, OUTPUT);
    pinMode(yellow1, OUTPUT);
    pinMode(green1, OUTPUT);
    pinMode(red2, OUTPUT);
    pinMode(yellow2, OUTPUT);
    pinMode(green2, OUTPUT);

    // Initial state: T1 Green, T2 Red
    digitalWrite(green1, HIGH);
    digitalWrite(red1, LOW);
    digitalWrite(yellow1, LOW);

    digitalWrite(green2, LOW);
    digitalWrite(red2, HIGH);
    digitalWrite(yellow2, LOW);
}

void loop() {
    // Continuously monitor the Li-Fi signal
    detectLiFiSignal();

    // Check the Li-Fi signal and control traffic lights accordingly
    if (detectLiFiSignal()) {
        emergencyTrafficClear();
    } else {
        normalTrafficCycle();
    }

    delay(50);  // Small delay to allow the loop to be more responsive
}

bool detectLiFiSignal() {
    int receivedPattern[patternLength];
    int ambientLight = 0;
    
    // Measure ambient light (average over 10 readings)
    for (int i = 0; i < 10; i++) {
        ambientLight += analogRead(receiver);
        delay(50);
    }
    ambientLight /= 10;  // Average ambient light
    int threshold = ambientLight + 20;  // Lower the threshold to make it more sensitive

    Serial.print("Dynamic Threshold: ");
    Serial.println(threshold);  // Output dynamic threshold value
    
    Serial.println("Listening for Li-Fi Signal...");

    // Read the pattern with precise timing, considering the pulse length
    for (int i = 0; i < patternLength; i++) {
        int lightLevel = analogRead(receiver);  // Read light level from the receiver
        
        Serial.print("Light Level: ");
        Serial.print(lightLevel);  // Output the light level
        Serial.print(" -> ");
        
        // Detect HIGH (1) or LOW (0) based on threshold
        receivedPattern[i] = (lightLevel > threshold) ? 1 : 0; 
        Serial.println(receivedPattern[i]);  // Output detected pattern bit

        delay(sampleDelay);  // Wait for the next expected bit (200ms)
    }

    // Compare received pattern with expected 1010
    bool match = true;
    for (int i = 0; i < patternLength; i++) {
        if (receivedPattern[i] != expectedPattern[i]) {
            match = false;
            break;
        }
    }

    if (match) {
        Serial.println("✅ 1010 Signal Detected - Proper Li-Fi Reception!");  // Successful signal match
        return true;
    } else {
        Serial.println("❌ Incorrect Signal - Check Receiver or Transmitter.");  // Signal mismatch or error
        return false;
    }
}

void normalTrafficCycle() {
    // 🚦 Normal traffic cycle remains unchanged
    // Step 1: T1 Green (5s), T2 Red (5s)
    digitalWrite(green1, HIGH);
    digitalWrite(yellow1, LOW);
    digitalWrite(red1, LOW);

    digitalWrite(green2, LOW);
    digitalWrite(yellow2, LOW);
    digitalWrite(red2, HIGH);
    delay(5000);

    // Step 2: T1 Yellow (3s), T2 Red (3s)
    digitalWrite(green1, LOW);
    digitalWrite(yellow1, HIGH);
    delay(3000);

    // Step 3: T1 Red (5s), T2 Green (5s)
    digitalWrite(yellow1, LOW);
    digitalWrite(red1, HIGH);

    digitalWrite(red2, LOW);
    digitalWrite(green2, HIGH);
    delay(5000);

    // Step 4: T1 Red (3s), T2 Yellow (3s)
    digitalWrite(green2, LOW);
    digitalWrite(yellow2, HIGH);
    delay(3000);

    // Restart cycle (T2 turns Red, T1 turns Green)
    digitalWrite(yellow2, LOW);
    digitalWrite(red2, HIGH);

    digitalWrite(red1, LOW);
    digitalWrite(green1, HIGH);
}

void emergencyTrafficClear() {
    Serial.println("🚦 Emergency Mode: T1 Green for 30s, T2 Red for 30s");

    // T1 Green, T2 Red for 30 seconds
    digitalWrite(green1, HIGH);
    digitalWrite(yellow1, LOW);
    digitalWrite(red1, LOW);

    digitalWrite(green2, LOW);
    digitalWrite(yellow2, LOW);
    digitalWrite(red2, HIGH);
    delay(15000); 

    Serial.println("🔄 Resuming Normal Traffic Cycle...");
}
