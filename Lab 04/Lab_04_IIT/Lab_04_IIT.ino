#include "CalibratedAverageTouchSensor.h"

// Params
#define TOUCH_DEVIATION 100 // this value may depend on the object and the person touching it, you should tune it
#define TOUCH_CALIBRATION_NUM_SAMPLES 30  // number of samples in calibration
#define TOUCH_CALIBRATION_DELAY_MS 20     // delay used between samples in calibration
#define TOUCH_READ_DELAY_MS 20 // delay used for the main "loop"

bool isSquare = false;
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
  delay(200);
}

// Main loop
void loop() {
    // I need to add something that checks for first drop-in, and removal, so it doesn't play the audio a ton,
    // or just add a cooldown, which would probably be easiest
    Serial.print("isTouched: ");
    Serial.println(touchSensor.isTouched());
    bool touched = (touchSensor.isTouched());

    if(touched){
      isSquare = true;
      curr_time= millis();
      // PLAY THE AUDIO
      if(curr_time - start_time < COOLDOWN){
        Serial.write(2); // send to processing to play the audio
      Serial.print("square hole");
      delay(250);
      }
    }
    delay(10);
}

