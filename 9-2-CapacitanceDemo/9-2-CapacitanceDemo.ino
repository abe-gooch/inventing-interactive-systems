const int LED_RED = 27;
const int TOUCH_PIN = 32;
int counter = 0;
bool lightOn = false;
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);
  // while(!Serial);

}

void loop() {
  // LED and Serial Stuff
  // Serial.print("Hello: ");
  // Serial.print(counter);
  // counter +=1;
  // digitalWrite(LED_RED, HIGH);
  // delay(500);
  // digitalWrite(LED_RED, LOW);
  // delay(500);
  // Serial.println();

  int sensedValue = touchRead(TOUCH_PIN); // plug one side of wire to row of touch pin, leave the other side unplugged. That is where the sensing occurs.
  Serial.print("0,2000"); //send dummy values to set the scale of the plot so it doesn't change
  Serial.print(",");
  Serial.println(sensedValue); // the logic on this is inverted, so the number actually goes down as you touch it despite capacitance going up
  delay(10); //prevents overloading the serial port

  if(sensedValue<=600){
    if(lightOn == false){ digitalWrite(LED_RED, HIGH);}
    else{digitalWrite(LED_RED, LOW);}
    lightOn = !lightOn;
  }
}
