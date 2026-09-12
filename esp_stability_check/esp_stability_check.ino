void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Hello, I am stable!");
}

void loop() {
  delay(1000); // Crucial to prevent WDT resets
}

