#define DIR 3
#define STEP 2
#define POT A1

//#define is used to set pins.
//DIR on the driver controlls direction
//STEP on the driver is what is used to drive the motor

//We can use MS1 and MS2 (look at the pinout for TMC2209) to set micro steps.
//in this case, i have not defined it, but i can do #define MS1 and MS2
//not setting MS1 and MS2 makes it so that 1600 steps are a full rotation of the motor.

//MS1 = LOW, MS2 = LOW = 8 microsteps
//MS1 = HIGH, MS2 = LOW = 32 microsteps
//MS1 = LOW, MS2 = HIGH = 64 microsteps
//MS1 = HIGH, MS2 = HIGH = 16 microsteps (yes, 16. i thought it would be 128 as well. its weird.)

//Again, we do not define MS1 and MS2, so we dont set it to any voltage. therefore, they are both LOW, so we are at 8 microsteps.

// how microsteps work, first you need to know that on a regular stepper, 200 steps is a revolution
// if microsteps is 8, then we multiply each step into 8 little steps. hence, now it is 1600 steps.

void setup() {
  // of course, DIR and STEP are set to output, because we are sending a signal to the stepper driver.
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);

//We set DIR to LOW, but it can be set to high to change direction. 
//it does not have to be set here, but in this case i did it just for testing.
  digitalWrite(DIR, LOW);
  Serial.begin(9600);
}


//A step happens when STEP is sent a HIGH signal, followed by a LOW signal. this rotates the motor a small amount.
//A delay is put between the HIGH and LOW to control speed. lower delay = faster, longer delay = slower

void step(int speed, int steps){
  for(int i = 0; i < steps; i++){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(speed);
    digitalWrite(STEP, LOW);
    delayMicroseconds(speed);
  }
}

void turnPlanetary(int turns, int speed){
  step(speed, (1600 * 4.5) * turns);
}

void potvaluedrive() {
    int value = analogRead(POT);
    int minSpeed = 1000;     // Fastest (microseconds)
    int maxSpeed = 5000;   // Slowest (microseconds)
    int deadzone = 25;     // Half-width of deadzone around center
    int center = 512;

    if (value < center - deadzone) {
        digitalWrite(DIR, HIGH);
        int speed = map(value, center - deadzone - 1, 0,maxSpeed, minSpeed);
        step(speed, 25);
    }
    else if (value > center + deadzone) {
        digitalWrite(DIR, LOW);
        int speed = map(value, center + deadzone + 1, 1023, maxSpeed, minSpeed);
        step(speed, 25);
    }
    else {
        step(0, 0);
    }
}

void loop() {
  Serial.println(analogRead(POT));
  potvaluedrive();
}
