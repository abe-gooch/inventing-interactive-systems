#include "CalibratedAverageTouchSensor.h"

// Params
#define TOUCH_DEVIATION 100 // this value may depend on the object and the person touching it, you should tune it
#define TOUCH_CALIBRATION_NUM_SAMPLES 30  // number of samples in calibration
#define TOUCH_CALIBRATION_DELAY_MS 20     // delay used between samples in calibration
#define TOUCH_READ_DELAY_MS 20 // delay used for the main "loop"

bool touched = false;
bool isSquare = false;
bool square_done = false;
bool circle_done = false;
bool triangle_done = false;
bool awaiting_input = true;
const int COOLDOWN = 3000;
int curr_time = 0;
int start_time = 0;


// Capacitive Pins
const int SQUARE_PIN = 4; 
CalibratedAverageTouchSensor touchSensor = CalibratedAverageTouchSensor(SQUARE_PIN, TOUCH_DEVIATION);


void setup() {
  Serial.begin(115200);
  initCapacitiveSensor(); // initialize / setup the sensor

}

void initCapacitiveSensor() {
  Serial.println("Init Cap sensor");
  touchSensor.calibrate(TOUCH_CALIBRATION_NUM_SAMPLES, TOUCH_CALIBRATION_DELAY_MS);
  Serial.print("T1 Average: ");
  Serial.println(touchSensor.getAverage());
  
  Serial.println("Calibration Done.");
  delay(1000);
}

// Main loop
void loop() {
    // I need to add something that checks for first drop-in, and removal, so it doesn't play the audio a ton,
    // or just add a cooldown, which would probably be easiest
    // Serial.print("isTouched: ");
    // Serial.println(touchSensor.isTouched());
    touched = (touchSensor.isTouched());
    awaiting_input = true;

    if(touched){
      awaiting_input = false;
      isSquare = true;
      curr_time= millis();
      // PLAY THE AUDIO
      if((curr_time - start_time > COOLDOWN) && square_done){
        Serial.write(2); // send to processing to play the audio
      Serial.println("square hole");
      delay(4000); // allow time for the audio clip to play
      start_time = millis();
      }
    }

    //Logic for question prompting:
    if(!square_done){
      Serial.write(1); // trigger square question
      Serial.println("square_done");
      square_done = true;
      awaiting_input = true;
      delay(500);
    }
    else if(!circle_done && square_done == true && awaiting_input == false){
      Serial.write(3); // trigger circle question
      Serial.println("circle_done");
      circle_done = true;
      awaiting_input = true;
      delay(500);
    }
    else if(!triangle_done && circle_done == true && awaiting_input == false){
      Serial.write(4);
      Serial.println("triangle_done");
      triangle_done = true;
      awaiting_input = true;
      delay(500);
    }
    delay(10);
}

