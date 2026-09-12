const int LED_PIN = 27;
const int NUM_CALIBRATION_SAMPLES = 50;
const int TOUCH_RED = 32; // right
const int TOUCH_BLUE = 33; // mid
const int TOUCH_GREEN = 4; // left
const int SWIPE_TIME_WINDOW = 1000; // 1 second since first contact to perform swipe
int touchThreshold = 0; // resets during calibration in setup
bool lightOn = false;

//would be more efficient to make a class
class touchPin{
  public:
    int pin;
    bool isOn;

  touchPin(int pinNum, bool onBool) : pin(pinNum), isOn(onBool) {} //dont forget to make this constructor when doing classes!!!
};

// pin class objects
touchPin left(TOUCH_GREEN, false);
touchPin mid(TOUCH_BLUE, false);
touchPin right(TOUCH_RED, false);

// swipe variables  (constants are above with the rest of the constants)
int firstTouched = 0;
int lastTouched = 0;
int swipeStartTime = 0;

void resetSwipeVars(){
  firstTouched = 0;
  lastTouched = 0;
  swipeStartTime = 0;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  delay(150);
  digitalWrite(LED_PIN, 0);

  // Sensor calibration  --> why are these not printing? Serial should be ready?
  Serial.print("Calibrating Touch Sensors - no touchies >:(");
  long total = 0;
  for (int i = 0; i < NUM_CALIBRATION_SAMPLES; i++) {
    total += touchRead(mid.pin);
    delay(10);
  }
  int baseline = total / NUM_CALIBRATION_SAMPLES; // take the average of the readings

  touchThreshold = baseline * 0.45; // ~600 usually. need reduce because these boards work backwards.
  Serial.print("Done calibrating");
}

void loop() {    
  int sensedGreen = touchRead(left.pin);
  int sensedBlue = touchRead(mid.pin);
  int sensedRed = touchRead(right.pin);
  if(sensedGreen<=touchThreshold){left.isOn=true;} else{left.isOn=false;}
  if(sensedBlue<=touchThreshold){mid.isOn=true;}   else{mid.isOn=false;}
  if(sensedRed<=touchThreshold){right.isOn=true;}  else{right.isOn=false;}

// ============= SWIPE TO TURN LED ON ============================  --> I did look up pseudocode for this section because the time component was giving me trouble. 
// if green, blue, and red are all touched within in a span of time, and in order, that can be considered a swipe

  Serial.print(left.isOn);
  Serial.print(mid.isOn);
  Serial.println(right.isOn);
//  Serial.println(touchThreshold);

  int currTime = millis();  

// if something is pressed but the swipe window has passed
  if(firstTouched != 0 && (currTime - swipeStartTime > SWIPE_TIME_WINDOW)){
    resetSwipeVars();
  }

  // Check if something is being pressed
  if(firstTouched == 0){  // FIRST CONTACT
      if(left.isOn==true){
        firstTouched=1; //set both since its first contact
        lastTouched=1;
      }
      else if(right.isOn==true){
        firstTouched=3;
        lastTouched=3;
      }
      swipeStartTime = currTime;
  }
  else{ // L/R HAS ALREADY BEEN CONTACTED
    //right swipe
    if(firstTouched == 1){
      if(mid.isOn==true){ // we know the left input has already been activated, so we need to check that the middle one is on now
        lastTouched = 2; //update to 2 so we can then check for 3
        if(right.isOn == true){
          Serial.println("RIGHT SWIPE DETECTED");
          digitalWrite(LED_PIN, HIGH);

          resetSwipeVars();
          delay(300);
        }
      }
    }
    // left swipe
    else if(firstTouched==3){
      if(mid.isOn==true){
        lastTouched =2;
        if(left.isOn==true){
          Serial.println("LEFT SWIPE DETECTED");
          digitalWrite(LED_PIN, LOW);

          resetSwipeVars();
          delay(300);
        }
      }
    }
  }
  delay(10);

}
