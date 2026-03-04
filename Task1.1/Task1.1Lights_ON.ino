// Pin Definitions
const int porchLED   = 11; //porch LED connected to pin 11
const int hallwayLED = 9; //hallway LED connected to pin 9
const int buttonPin  = 4; //button connected to pin 4

// Time Durations 
const unsigned long porchTime = 30000;  // 30 seconds in milliseconds
const unsigned long hallTime  = 60000;  // 60 seconds in milliseconds

// State Variables - stores times the lights are turned on and tracks which light is on currently
unsigned long porchStart = 0;
unsigned long hallStart  = 0;
bool porchOn  = false;
bool hallOn   = false;

// Configures LED pins as OUTPUT and button pin as INPUT
void setupPins() {
  pinMode(porchLED,   OUTPUT); // porch LED is an output
  pinMode(hallwayLED, OUTPUT); // hallway LED is an output
  pinMode(buttonPin,  INPUT); // button is an input using a resistor
}

// Turns both LEDs on and records the start time for independent timing
void turnLightsOn() {
  porchStart = millis(); // records current times for porch and hallway timers
  hallStart  = millis();
  porchOn    = true; // marks porch and hallway light as active
  hallOn     = true;
  digitalWrite(porchLED,   HIGH); // turns porch LED on
  digitalWrite(hallwayLED, HIGH); // turns porch LED on
  Serial.println("Button pressed — lights ON"); // prints to serial monitor
}


// Checks elapsed time and turns each LED off independently when its duration has passed
void updateLights() {
  unsigned long currentTime = millis(); // gets current time

  if (porchOn && currentTime - porchStart >= porchTime) { // if 30 seconds has passed then:
    digitalWrite(porchLED, LOW); // turn porch LED off
    porchOn = false; // set state of porch light as inactive 
    Serial.println("Porch light OFF (30s elapsed)");
  }

  if (hallOn && currentTime - hallStart >= hallTime) { // if 60 seconds has passed then:
    digitalWrite(hallwayLED, LOW); // turn hallway LED off
    hallOn = false; // set state of hallway light as inactive 
    Serial.println("Hallway light OFF (60s elapsed)");
  }
}

// Reads button state and triggers lightsOnly activates if lights are not already running Includes debounce delay to prevent rapid re-triggering
void checkButton() {
  int buttonState = digitalRead(buttonPin); // reads button
  if (buttonState == HIGH && !porchOn && !hallOn) { 
    turnLightsOn();
    delay(300); // debounce
  }
}

// setup() - Runs once on power-up
void setup() {
  Serial.begin(9600);
  setupPins();
}

// loop() - Runs continuously, checks button and timers
void loop() {
  checkButton(); // checks if button is pressed
  updateLights(); // checks if any lights need to be turned off
}
