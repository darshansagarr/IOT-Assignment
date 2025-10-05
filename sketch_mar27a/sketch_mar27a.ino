// Motor driver pin definitions
#define in1 3
#define in2 5
#define in3 6
#define in4 11
#define laserPin 9  // Li-Fi Transmitter (Laser Light)
#define buttonPin 10 // Button to activate Li-Fi transmission

// Variables
int command; // Stores received command
int Speed = 204; // Speed range: 0 - 255
int Speedsec;
int Turnradius = 0; // Set turn radius (0 - 255)
int brakeTime = 45; // Brake duration in milliseconds
int brkonoff = 1; // 1 = Electronic braking, 0 = Normal
bool braking = false; // Flag to check if braking is active

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(laserPin, OUTPUT); // Laser Light Control
  pinMode(buttonPin, INPUT_PULLUP); // Button setup
  
  Serial.begin(9600);
  Serial.println("Ambulance Li-Fi Transmitter Ready!");
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.read();
    Serial.print("Command Received: ");
    Serial.println((char)command); // Debugging output

    Stop(); // Stop motors before executing a new command
    
    switch (command) {
      case 'F': forward(); break;
      case 'B': back(); break;
      case 'L': left(); break;
      case 'R': right(); break;
      case 'G': forwardleft(); break;
      case 'I': forwardright(); break;
      case 'H': backleft(); break;
      case 'J': backright(); break;
      case 'S': brakeOn(); break; // Activate braking
      case '0': Speed = 100; break;
      case '1': Speed = 140; break;
      case '2': Speed = 153; break;
      case '3': Speed = 165; break;
      case '4': Speed = 178; break;
      case '5': Speed = 191; break;
      case '6': Speed = 204; break;
      case '7': Speed = 216; break;
      case '8': Speed = 229; break;
      case '9': Speed = 242; break;
      case 'q': Speed = 255; break;
      default: Serial.println("Unknown Command"); break;
    }
    
    // Ensure speed limit
    if (Speed > 255) Speed = 255;

    // Turn radius setup
    Speedsec = Turnradius;

    // Brake control
    if (brkonoff == 1 && !braking) {
      brakeOn();
    } else {
      brakeOff();
    }
  }

  // Check button state to activate Li-Fi transmission
  if (digitalRead(buttonPin) == LOW) {
    digitalWrite(laserPin, HIGH);
    Serial.println("Li-Fi Transmission Activated");
  } else {
    digitalWrite(laserPin, LOW);
  }
}

// Motor Movement Functions
void forward() { 
  analogWrite(in1, Speed);
  analogWrite(in3, Speed);
  Serial.println("Moving Forward");
}

void back() { 
  analogWrite(in2, Speed);
  analogWrite(in4, Speed);
  Serial.println("Moving Backward");
}

void left() { 
  analogWrite(in3, Speed);
  analogWrite(in2, Speed);
  Serial.println("Turning Left");
}

void right() { 
  analogWrite(in4, Speed);
  analogWrite(in1, Speed);
  Serial.println("Turning Right");
}

void forwardleft() { 
  analogWrite(in1, Speedsec);
  analogWrite(in3, Speed);
  Serial.println("Forward Left");
}

void forwardright() { 
  analogWrite(in1, Speed);
  analogWrite(in3, Speedsec);
  Serial.println("Forward Right");
}

void backright() { 
  analogWrite(in2, Speed);
  analogWrite(in4, Speedsec);
  Serial.println("Backward Right");
}

void backleft() { 
  analogWrite(in2, Speedsec);
  analogWrite(in4, Speed);
  Serial.println("Backward Left");
}

// Stop Function
void Stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  Serial.println("Motors Stopped");
}

// Braking Functions
void brakeOn() {
  Serial.println("Braking...");
  braking = true;
  
  // Engaging brake (All HIGH for short circuit braking)
  digitalWrite(in1, HIGH);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, HIGH);
  
  delay(brakeTime); // Apply brakes for set duration
  Stop(); // Stop the motors after braking
  
  braking = false;
}

void brakeOff() {
  Serial.println("Brake Off - Normal Operation");
}

