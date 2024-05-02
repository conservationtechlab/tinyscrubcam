void setup() {
    Serial.begin(115200); // Initialize serial communication with Serial monitor
    Serial1.begin(115200); // Initialize serial communication with Serial1 (RX/TX pins)
}

void loop() {
    if (Serial1.available() > 0) { // Check if data is available to read
        String receivedString = Serial1.readStringUntil('\n'); // Read until newline character
        Serial.print("Received: ");
        Serial.println(receivedString); // Print the received line
    }
   
}