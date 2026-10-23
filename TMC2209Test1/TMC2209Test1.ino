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

// For this four-phase stepper function, this waits twice the "speed" variable at the start.
// It then approaches "speed" itself at the center two loops, then takes longer again at the end.

void FourPhaseStep(int speed, float rotations) {
  if (rotations <= 0) return;

  long steps = (long)(rotations * 1600);

  // Fixed acceleration and deceleration distances
  long accelSteps = 1600;
  long decelSteps = 3200;
  // if you are reading this christian, After experimenting, I have found that four phases in which the accelaration + constant phase + decel phase
  // do not work to well. as a result, I have a fixed accelaration and decelaration phase. these correspond tio the variables above.

  //Also, Instead of linearly accelarating, it exponentially accelarates and decelarates. this is kind of like a tween in roblox.

  // Handle short movements
  if (accelSteps + decelSteps > steps) {
    accelSteps = steps / 2;
    decelSteps = steps - accelSteps;
  }

  long cruiseSteps = steps - accelSteps - decelSteps;

  int slowDelay = speed * 4;

  // ACCELERATION: exponential
  for (long i = 0; i < accelSteps; i++) {
    float t = (float)i / accelSteps;
    float curve = t * t;

    int delayTime = slowDelay -
      (int)((slowDelay - speed) * curve);

    digitalWrite(STEP, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(STEP, LOW);
    delayMicroseconds(delayTime);
  }

  // CONSTANT SPEED
  for (long i = 0; i < cruiseSteps; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(speed);
    digitalWrite(STEP, LOW);
    delayMicroseconds(speed);
  }

  // DECELERATION: exponential
  for (long i = 0; i < decelSteps; i++) {
    float t = (float)i / decelSteps;
    float curve = t * t;

    int delayTime = speed +
      (int)((slowDelay - speed) * curve);

    digitalWrite(STEP, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(STEP, LOW);
    delayMicroseconds(delayTime);
  }
}

//ignore this function, its for testing.
void PlanetaryTurn(int turns, double ratio){
  FourPhaseStep(75, 1600 * ratio);
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
 FourPhaseStep(75, 46.24/4);
 digitalWrite(DIR, HIGH);
 FourPhaseStep(75, 46.24/4);
 digitalWrite(DIR, LOW);
}
