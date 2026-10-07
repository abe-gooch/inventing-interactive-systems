// Motor connections
const int enA = 13;
const int in1 = 12;
const int in2 = 14;
const int TOUCH_PIN = 32;

int sensedValue = 0;
int outputValue = 0;
int duration = 0; // in ms
int speed = 0;
int direction = 1;
int motorStartTime = 0;
bool motorRunning = false;
int currTime = 0;

// TO-DO:
// work on first paragraph of part 2 _ DONE
// integrate touchsensor _ IN PROGRESS

void setup() {
    // Set all the pins as outputs
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    // Turn off the motor to start with
    // digitalWrite(enA, LOW); // swap to analogWrite for analog input
    analogWrite(enA, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    
    Serial.begin(115200);
    Serial.println("Setup complete");
}

void loop() {

    sensedValue = analogRead(TOUCH_PIN);
    outputValue = map(sensedValue, 0, 1023, 0, 255); // take analog input and map to range of motor writing
    Serial.println(sensedValue);
    // delay(10);
    
    if(!motorRunning){
        motorStartTime = millis();
    }
    duration = 1000;
// ramp up to full from off in 1 second
    rampUp(1, duration);
    // delay(500); // run at full speed for 0.5 seconds --> instead of a delay, use millis and if statements to check if it's been running long enough, that way the loop isn't halted
    if(motorRunning == true){
        currTime = millis();
        if(currTime - motorStartTime >= duration)

    }
    rampDown(1, 600); //turn off in 0.6 seconds
    delay(500); // remain off for 0.5 seconds

}

// ================== FUNCTIONS ===========================
void forward(int speed) { // direction = 1
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enA, speed);
}

void backward(int speed) { // direction = 0
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enA, speed);
}

void stop() {
    digitalWrite(enA, LOW);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}

void rampUp(int direction, int duration){
    if(direction == 0){ // backward
        for(int speed = 0; speed <=255; speed++){
            backward(speed);
            delay(duration/255);
        }
    }
    else{ // forward
        for(int speed = 0; speed <=255; speed++){
            forward(speed);
            delay(duration/255);
        }
    }
    motorRunning = true;
}

void rampDown(int direction, int duration){
    if(direction == 0){ // backward
        for(int speed = 255; speed >=0; speed--){
            backward(speed);
            delay(duration/255);
        }
    }
    else{ // forward
        for(int speed = 255; speed >=0; speed--){
            forward(speed);
            delay(duration/255);
        }
    }
    motorRunning = false;
}

// ==== GRAVEYARD ========
// Move the motor in forward direction for 2 seconds
//  forward(); // see def below loop
//     delay(2000);

//     // Stop for 1 second
//     stop();
//     delay(1000);

//     // Move the motor in the backward direction for 2 seconds
//     backward();
//     delay(2000);

//     // Stop for 1 second
//     stop();
//     delay(1000);