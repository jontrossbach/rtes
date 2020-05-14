#include <Adafruit_CircuitPlayground.h>

float X, Y, Z;
uint8_t i, recordState; 

//rtrc is the ready to read couter
uint8_t rtrc; 
bool readyToRead;

bool leftButtonPressed;
bool rightButtonPressed;

enum Gesture {
        RIGHTWARD,
        LEFTWARD,
        FORWARD,
        BACKWARD,
        UPSIDE_DOWN,
        NO_GESTURE
};

enum Gesture currentGesture;
enum Gesture unlockGestures[4];


#define GRAVITATIONAL_NOISE 9.50
#define MOVE_THRESHOLD 6
#define RESET_THRESHOLD 100
#define NUM_LEDS 10

enum Gesture getGesture(){
        //Get accelerometer values from the circuit playground
        X = CircuitPlayground.motionX();
        Y = CircuitPlayground.motionY();
        Z = CircuitPlayground.motionZ();

        //Subtract the acceleration caused by gravity
        Z = Z - GRAVITATIONAL_NOISE;

        Serial.print("X: ");
        Serial.print(X);
        Serial.print("  Y: ");
        Serial.print(Y);
        Serial.print("  Z: ");
        Serial.println(Z);
        
        if(X > MOVE_THRESHOLD)
                return RIGHTWARD;
        else if(X < -MOVE_THRESHOLD)
                return LEFTWARD;
        else if(Y > MOVE_THRESHOLD)
                return FORWARD;
        else if(Y < -MOVE_THRESHOLD)
                return BACKWARD;
        else
                return NO_GESTURE;
                
}

uint8_t getButtonPress(){
        //Get accelerometer values from the circuit playground
        leftButtonPressed = CircuitPlayground.leftButton();
        rightButtonPressed = CircuitPlayground.rightButton();
  
        //button to reset all gestures i=0;
        //Serial.print("Left Button: ");
        if (leftButtonPressed) {
                Serial.print("DOWN \n");
                i=0;
        }
        //button that sets i=4; to reset comparison gestures
        //Serial.print("   Right Button: \n");
        if (rightButtonPressed) {
                Serial.print("DOWN \n");
                i=4; 
        }
        
        return i;
}

bool readyToReadDiscriminator(){
         if (rtrc < RESET_THRESHOLD){
                 rtrc = rtrc + 1;
                 //Serial.print(rtrc);
                 return 0;
         }
         else {
                 return 1;
         }
}

void unlockLights(int wait){
        for (uint8_t j=0; j<NUM_LEDS; j++){
                CircuitPlayground.setPixelColor(j, 0, 200, 0);
        }
        delay(wait);
        CircuitPlayground.clearPixels();
}

void wrongGestureIndicator(){
        CircuitPlayground.setPixelColor(i, 200, 0, 0);
        delay(1000);
        CircuitPlayground.clearPixels();
}

void setup() {
        Serial.begin(9600);
        CircuitPlayground.begin();
        i = 0;
        readyToRead = 0;
        rtrc = RESET_THRESHOLD;
        recordState = 1;
        for(int j=0; j<4; j++){
                unlockGestures[i] = NO_GESTURE;
        }
}

void loop() {

        currentGesture = getGesture();

        i = getButtonPress();

        if (i==8){
                unlockLights(1000);
        }
        else if (readyToRead && currentGesture != NO_GESTURE ) {
                Serial.print("Ready to roll \n");
                if ( i < 4 ) {
                        Serial.print("GESTURE STORED \n");
                        unlockGestures[i] = currentGesture;
                }
                else if ( i > 4 && unlockGestures[i-4] != currentGesture ) {
                        i=4;
                        wrongGestureIndicator();
                }
                else {
                        Serial.print("GESTURE COMPARED \n");
                }
                readyToRead = 0;
                rtrc=0;
                i++;
        }
        else if ( currentGesture == NO_GESTURE ) {
                Serial.print("NO GESTURE \n");
                readyToRead = readyToReadDiscriminator();
        }
        else {
                //do nothing
        }
                
}

