//Pin Definitions
const int porchLED   = 11;
const int hallwayLED = 9;
const int buttonPin  = 4;

//Time Durations 
const unsigned long porchTime = 30000;  // 30 seconds
const unsigned long hallTime  = 60000;  // 60 seconds

//State Variables 
unsigned long porchStart = 0;
unsigned long hallStart  = 0;
bool porchOn  = false;
bool hallOn   = false;

// setupPins() - Configures LED pins as OUTPUT and
// button pin as INPUT
void setupPins() {
  pinMode(porchLED,   OUTPUT);
  pinMode(hallwayLED, OUTPUT);
  pinMode(buttonPin,  INPUT);
}

// turnLightsOn() - Turns both LEDs on and records
// the start time for independent timing
void turnLightsOn() {
  porchStart = millis();
  hallStart  = millis();
  porchOn    = true;
  hallOn     = true;
  digitalWrite(porchLED,   HIGH);
  digitalWrite(hallwayLED, HIGH);
  Serial.println("Button pressed — lights ON");
}


// updateLights() - Checks elapsed time and turns each
// LED off independently when its duration has passed
void updateLights() {
  unsigned long currentTime = millis();

  if (porchOn && currentTime - porchStart >= porchTime) {
    digitalWrite(porchLED, LOW);
    porchOn = false;
    Serial.println("Porch light OFF (30s elapsed)");
  }

  if (hallOn && currentTime - hallStart >= hallTime) {
    digitalWrite(hallwayLED, LOW);
    hallOn = false;
    Serial.println("Hallway light OFF (60s elapsed)");
  }
}

// checkButton() - Reads button state and triggers lights
// Only activates if lights are not already running
// Includes debounce delay to prevent rapid re-triggering
void checkButton() {
  int buttonState = digitalRead(buttonPin);
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
  checkButton();
  updateLights();
}
