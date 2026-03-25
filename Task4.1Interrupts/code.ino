#include <Wire.h> // I2C communication library
#include <BH1750.h> // Light sensor library

#define PIR_PIN     2  
#define SLIDER_PIN  3 
#define LED1_PIN    4  
#define LED2_PIN    5 

#define DARK_THRESHOLD 80.0  

BH1750 lightMeter; // Create light sensor object

volatile bool pirTriggered    = false; // Flag for PIR interrupt
volatile bool sliderTriggered = false; // Flag for slider interrupt

unsigned long lastPirTime    = 0; // Last PIR trigger time
unsigned long lastSliderTime = 0; // Last slider trigger time
const unsigned long DEBOUNCE_MS = 500; // Debounce delay (ms)

void onPIR() {
  unsigned long now = millis(); // Get current time

  if (now - lastPirTime > DEBOUNCE_MS) { // Check debounce
    pirTriggered = true; // Set PIR flag to True
    lastPirTime  = now;  // Update last PirTime
  }
}

void onSlider() {
  unsigned long now = millis(); // Get current time

  if (now - lastSliderTime > DEBOUNCE_MS) { // Check debounce
    sliderTriggered = true; // Set slider flag
    lastSliderTime  = now;  // Update last time
  }
}

void setLEDs(bool state) {
  digitalWrite(LED1_PIN, state ? HIGH : LOW); // Set LED1 ON/OFF
  digitalWrite(LED2_PIN, state ? HIGH : LOW); // Set LED2 ON/OFF
}

void setup() {
  Serial.begin(9600); // Start serial monitor

  pinMode(LED1_PIN, OUTPUT); // Set LED1 as output
  pinMode(LED2_PIN, OUTPUT); // Set LED2 as output
  setLEDs(false); // Turn LEDs OFF

  pinMode(PIR_PIN, INPUT); // Set PIR as input
  pinMode(SLIDER_PIN, INPUT); // Set slider as input

  attachInterrupt(digitalPinToInterrupt(PIR_PIN), onPIR, RISING); // Interrupt for PIR
  attachInterrupt(digitalPinToInterrupt(SLIDER_PIN), onSlider, CHANGE); // Interrupt for slider

  Wire.begin(); // Start I2C

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) { // Start light sensor
    Serial.println("[SYSTEM] BH1750 light sensor initialised."); // Success message
  } else {
    Serial.println("[ERROR]  BH1750 not found — check SDA/SCL wiring."); // Error message
  }

  Serial.println("[SYSTEM] Task 4.1 Interrupts ready."); // Ready message
  Serial.println("[SYSTEM] Waiting for motion or switch event..."); // Waiting message
  Serial.println("-----------------------------------------"); 
}

void loop() {

  if (pirTriggered) { // If PIR triggered
    pirTriggered = false; // Reset flag

    float lux = lightMeter.readLightLevel(); // Read light level

    if (lux < DARK_THRESHOLD) { // If dark
      setLEDs(true); // Turn LEDs ON

      Serial.print("[PIR]    Motion detected in the dark ("); 
      Serial.print(lux); // Show light value
      Serial.println(" lux) — lights turned ON."); 
      Serial.println("-----------------------------------------"); 

    } else { // If bright
      Serial.print("[PIR]    Motion detected but it is bright ("); 
      Serial.print(lux); // Show light value
      Serial.println(" lux) — lights left OFF.");
      Serial.println("-----------------------------------------");
    }
  }

  if (sliderTriggered) { // If slider triggered
    sliderTriggered = false; // Reset flag

    bool sliderOn = digitalRead(SLIDER_PIN); // Read slider state
    setLEDs(sliderOn); // Set LEDs based on slider

    if (sliderOn) { // If switch ON
      Serial.println("[SWITCH] Backup switch activated — lights turned ON.");
    } else { // If switch OFF
      Serial.println("[SWITCH] Backup switch deactivated — lights turned OFF."); 
    }

    Serial.println("-----------------------------------------"); 
  }

  delay(50); 
}
