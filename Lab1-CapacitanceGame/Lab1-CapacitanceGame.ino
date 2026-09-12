#include <LiquidCrystal.h>
#include <vector>
#include <cstdlib> // for rand()
const int LED_PIN = 15;
const int NUM_CALIBRATION_SAMPLES = 50;
const int TOUCH_RED = 32; // right
const int TOUCH_BLUE = 33; // mid
const int TOUCH_GREEN = 4; // left
const int SWIPE_TIME_WINDOW = 1000; // 1 second since first contact to perform swipe
int touchThreshold = 0; // resets during calibration in setup
bool lightOn = false;

// Memory Game Vars
std::vector<int> systemMoves;
std::vector<int> playerMoves;
bool identical = true; //for comparing vectors in the memory game
bool awaitingInput = true;
bool firstRound = true;
bool systemHasDisplayed = false;
bool playerInputDone = true;
bool gameLost = false;

// ==== PIN CLASS & OBJECTS =========
class touchPin{
  public:
    int pin;
    bool isOn;

  touchPin(int pinNum, bool onBool) : pin(pinNum), isOn(onBool) {} //dont forget to make this constructor when doing classes!!!
};
touchPin left(TOUCH_GREEN, false);
touchPin mid(TOUCH_BLUE, false);
touchPin right(TOUCH_RED, false);

// ======== SWIPE VARIABLES ==========  (constants are above with the rest of the constants)
int firstTouched = 0;
int lastTouched = 0;
int swipeStartTime = 0;

void resetSwipeVars(){
  firstTouched = 0;
  lastTouched = 0;
  swipeStartTime = 0;
}

// ============ LCD SETUP =========
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);
byte rightArrow[8] = { //small, single character/rectangle
  B00000, //   
  B00100, //     █
  B00010, //       █
  B11111, // █ █ █ █ █
  B00010, //       █
  B00100, //     █
  B00000, // 
  B00000  //
};
byte leftArrow[8] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000
};

void bigRightArrow(){ // spam a bunch of arrows so it's easy to read on the screen
    lcd.clear();
    lcd.setCursor(5,0);    lcd.write(byte(0));
    lcd.setCursor(6,0);    lcd.write(byte(0));
    lcd.setCursor(7,0);    lcd.write(byte(0));
    lcd.setCursor(8,0);    lcd.write(byte(0));
    lcd.setCursor(9,0);    lcd.write(byte(0));
    lcd.setCursor(10,0);   lcd.write(byte(0));
}

void bigLeftArrow(){
    lcd.clear();
    lcd.setCursor(5,0);    lcd.write(byte(1));
    lcd.setCursor(6,0);    lcd.write(byte(1));
    lcd.setCursor(7,0);    lcd.write(byte(1));
    lcd.setCursor(8,0);    lcd.write(byte(1));
    lcd.setCursor(9,0);    lcd.write(byte(1));
    lcd.setCursor(10,0);   lcd.write(byte(1));
}


// ~~~~~~~~~~~~~ VOID SETUP ~~~~~~~~~~~~~~
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  Serial.println("========== SETUP START ==========");

  randomSeed(esp_random()); // the esp can apparently provide random bits for Arduino's random seed

  Serial.println("1. LED");
  pinMode(LED_PIN, OUTPUT);
  Serial.println("2. LED OFF");
  digitalWrite(LED_PIN, 0);
  // set up the LCD's number of columns and rows:
  Serial.println("3. LCD begin");
  lcd.begin(16, 2);
  //save right and left arrows as custom chars in the built-in array for them
  Serial.println("4. LCD Chars");
  lcd.createChar(0, rightArrow);
  lcd.createChar(1, leftArrow);
  

// === TOUCH SENSOR CALIBRATION ===
  Serial.println("5. Calibration");
  Serial.println("Calibrating Touch Sensors - no touchies >:(");
  long total = 0;
  for (int i = 0; i < NUM_CALIBRATION_SAMPLES; i++) {
    total += touchRead(mid.pin);
    // Serial.println(touchRead(mid.pin));
    delay(50);
  }
  int baseline = total / NUM_CALIBRATION_SAMPLES; // take the average of the readings
  Serial.print("Baseline: ");
  Serial.println(baseline);
  touchThreshold = baseline * 0.6; // need reduce because these boards work backwards.
  Serial.print("Threshold: ");
  Serial.println(touchThreshold);
  Serial.println("6. Calibration Done");


  // DISPLAY GAME INFO
  Serial.println("7. Game info is being printed on the LCD");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MEMORY GAME");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("REPEAT MOVEMENTS");
  lcd.setCursor(0,1);
  lcd.print("WHEN LED IS LIT");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GET READY!");
  delay(4000);

  Serial.println("8. Setup Done");
}

// ~~~~~~~~~~~ VOID LOOP ~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
if(gameLost == true){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TRY AGAIN");
    delay(5000);
    systemMoves.clear();
    playerMoves.clear();
    identical = true;
    awaitingInput = true;
    firstRound = true;
    systemHasDisplayed = false;
    playerInputDone = true;
    gameLost = false;
}  
else{
  int sensedGreen = touchRead(left.pin);
  int sensedBlue = touchRead(mid.pin);
  int sensedRed = touchRead(right.pin);
  if(sensedGreen<=touchThreshold){left.isOn=true;} else{left.isOn=false;}
  if(sensedBlue<=touchThreshold){mid.isOn=true;}   else{mid.isOn=false;}
  if(sensedRed<=touchThreshold){right.isOn=true;}  else{right.isOn=false;}
  playerInputDone = false;

// ============= SWIPE TO TURN LED ON ============================  --> I did look up pseudocode for this section because the time component was giving me trouble. 
// if green, blue, and red are all touched within in a span of time, and in order, that can be considered a swipe

  // Serial.println(left.isOn);
  // Serial.println(sensedGreen);
  // Serial.print(mid.isOn);
  // Serial.println(right.isOn);
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
          // digitalWrite(LED_PIN, HIGH); // old swipe on-off demonstration
          lcd.setCursor(0, 0);
          lcd.clear();
          // lcd.print("RIGHT SWIPE");
          // delay(500);
          bigRightArrow();
          delay(750);
          lcd.clear();

          playerMoves.push_back(1);
          awaitingInput = false;
          resetSwipeVars();

          Serial.print("playerMoves size: ");
          Serial.println(playerMoves.size());
          Serial.print("Player Moves Vector: ");
          for(int i = 0; i<playerMoves.size(); i++){
            Serial.print(playerMoves.at(i));
            Serial.print(" ");
          }
          Serial.println();
        }
      }
    }
    // left swipe
    else if(firstTouched==3){
      if(mid.isOn==true){
        lastTouched =2;
        if(left.isOn==true){
          Serial.println("LEFT SWIPE DETECTED");
          // digitalWrite(LED_PIN, LOW); // old swipe on-off demonstration
          lcd.setCursor(0, 0);
          lcd.clear();
          // lcd.print("LEFT SWIPE");
          // delay(500);
          bigLeftArrow();
          delay(750);
          lcd.clear();

          playerMoves.push_back(0);
          awaitingInput = false;
          resetSwipeVars();

          // Serial.print("playerMoves size: ");
          // Serial.println(playerMoves.size());
          // Serial.print("Player Moves: ");
          // for(int i = 0; i<playerMoves.size(); i++){
          //   Serial.print(playerMoves.at(i));
          //   Serial.print(" ");
          // }
          // Serial.println();
        }
      }
    }
    delay(250);
  }
  delay(50);

  if(playerMoves.size()==systemMoves.size()){
        playerInputDone = true;
        Serial.println("Player input done");
    
    for(int i = 0; i < systemMoves.size(); i++){
    if(playerMoves.at(i) != systemMoves.at(i)){
      identical = false;
      Serial.print("INCORRECT. GAME OVER!");
      gameLost = true;
      digitalWrite(LED_PIN, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GAME OVER :(");
      delay(5000);
      return;
    }
  }
  }

// ======= MEMORY GAME IMPLEMENTATION ==================

  if(identical==true){ // keep adding more directions to game if player input has been correct
    if(firstRound == true || playerInputDone == true){
      int systemDirection = random(0,2); // either 0 or 1, 0 being left and 1 being right
      Serial.print("systemDirection: ");
      Serial.println(systemDirection);
      systemMoves.push_back(systemDirection); // add to vector
      Serial.print("systemMove vector size = ");
      Serial.println(systemMoves.size());

    //display the moves to the player
      if(systemHasDisplayed == false){ // so it won't constantly rerun
        digitalWrite(LED_PIN, LOW); // indicate the player should stop inputs
        Serial.print("System Moves Vector: ");
        for(int i = 0; i < systemMoves.size(); i++){
          Serial.print(systemMoves.at(i));
          Serial.print(" ");
          lcd.clear();
          delay(200);
          if(systemMoves.at(i) == 0){
            bigLeftArrow();
            // Serial.println("The system played a left arrow");
          }
          else{
            bigRightArrow();
            // Serial.println("The system played a right arrow");
          }
          delay(750);
        }
        systemHasDisplayed = true;
        firstRound = false;
        if(playerMoves.size()==systemMoves.size()){
          playerInputDone = true;
        }
        lcd.clear();
        digitalWrite(LED_PIN, HIGH);
        Serial.println("System Done Displaying");
      }

      //empty the player's vector to take new inputs
      if(playerInputDone == true){
        playerMoves.clear();
        Serial.println("Player moves cleared");
      }
      else{
        if(systemMoves.size()==playerMoves.size()){
        for(int i = 0; i < systemMoves.size(); i++){
          if(systemMoves.at(i) != playerMoves.at(i)){
            identical = false;
            break;
            Serial.print("Incorrect player input! Game over!");
            delay(10000);
          }
          
        }
        awaitingInput = true; // prepare for next input
      }
      }
      systemHasDisplayed = false; // prepare system to display next set
    }

  }
}} // end else and void loop


/* NEED INTEGRATE:
 - LCD Screen [done]
 - L/R arrow display [done]
 - memory for machine output  -> prob just use 0's and 1's to track L/R. Array or vector ig. 
    - if vector, could just keep going until user messes up. Display the number they got right?
 - memory for user input
 - comparison of the two

 - serial prints to give directions before entering void loop somehow, or a while loop that will only run once (bool once user gives okay to move on past directions)

*/
