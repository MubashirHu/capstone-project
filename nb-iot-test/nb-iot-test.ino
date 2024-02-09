void setup() {
  // Start serial communication with the computer and the SIM7020G module
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(3000); // Wait for everything to initialize

  Serial.print("hello");
  // Send various AT commands to check the status of the SIM7020G module
  sendATCommand("AT"); // Basic AT command test
  sendATCommand("AT+CPIN?"); // Check SIM card status
  sendATCommand("AT+CREG?"); // Check network registration
  sendATCommand("AT+CSQ"); // Check signal quality
  sendATCommand("AT+COPS?"); // Check connected operator
  sendATCommand("AT+CGATT?"); // Check network attachment for data services
  sendATCommand("AT+CGMI"); // Get manufacturer information
  sendATCommand("AT+CGMM"); // Get model information
  sendATCommand("AT+CGMR"); // Check firmware version
  sendATCommand("AT+CGSN"); // Get IMEI number
  // sendATCommand("AT+CBC"); // Uncomment if battery voltage check is supported
}

void loop() {
  // Read and print responses from the SIM7020G module
  if (Serial1.available()) {
    String response = Serial1.readString();
    Serial.println(response);
  }
}

void sendATCommand(String command) {
  Serial1.println(command);
  delay(2000); // Wait for a response
}
