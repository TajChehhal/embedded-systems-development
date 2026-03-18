/*
  A soil moisture sensor was used alongside an ambient light sensor for this task, since the 
  temperature and humidity sensor wasn't available for collection during the task submission week.
  Field 1 = soil moisture value
  Field 2 = light level in lux
*/

#include <WiFiNINA.h> // Library for WiFi connection
#include <ThingSpeak.h> // Library to send data to ThingSpeak
#include <Wire.h> // Library for I2C communication
#include <BH1750.h> // Library for light sensor

const char* WIFI_SSID     = "WIFI_SSID"; // WiFi name
const char* WIFI_PASSWORD = "WIFI_PASSWORD"; // WiFi password

const unsigned long CHANNEL_ID    = CHANNEL_ID; // ThingSpeak channel ID
const char*         WRITE_API_KEY = "API_KEY"; // ThingSpeak API key

const int MOISTURE_PIN = A0; // Pin for soil moisture sensor

const unsigned long SEND_INTERVAL_MS = 30000UL; // Send data every 30 seconds

WiFiClient client; // Create WiFi client object
BH1750 lightMeter; // Create light sensor object
unsigned long lastSendTime = 0; // Stores last send time

int readMoistureRaw() {
  return analogRead(MOISTURE_PIN); // Read moisture value from sensor
}

float readLightLevel() {
  return lightMeter.readLightLevel(); // Read light level from sensor
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: "); // Print message
  Serial.println(WIFI_SSID); // Show WiFi name
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Start WiFi connection
  int attempts = 0; // Counter for attempts

  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Try to connect
    delay(500); // Wait 0.5 seconds
    Serial.print("."); // Show progress
    attempts++; // Increase attempts
  }

  if (WiFi.status() == WL_CONNECTED) { // If connected
    Serial.println("\nWiFi connected."); // Success message
    Serial.print("IP address: "); // Print IP label
    Serial.println(WiFi.localIP()); // Show IP address
  } else {
    Serial.println("\nFailed to connect to WiFi. Check credentials."); // Error message
  }
}

void sendToThingSpeak(int moistureRaw, float lightLux) {
  if (WiFi.status() != WL_CONNECTED) { // Check WiFi connection
    Serial.println("WiFi disconnected. Reconnecting..."); // Message
    connectToWiFi(); // Reconnect WiFi
  }

  ThingSpeak.setField(1, moistureRaw); // Set field 1 (moisture)
  ThingSpeak.setField(2, lightLux); // Set field 2 (light)

  int responseCode = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY); // Send data

  if (responseCode == 200) { // If success
    Serial.println("ThingSpeak update successful."); // Success message
  } else {
    Serial.print("ThingSpeak error. HTTP code: "); // Error message
    Serial.println(responseCode); // Show error code
  }
}

void printReadings(int moistureRaw, float lightLux) {
  Serial.println("──────────────────────────────");
  Serial.print("Soil Moisture (raw): ");
  Serial.println(moistureRaw); // Show moisture value
  Serial.print("Light Level (lux):   ");
  Serial.print(lightLux, 1); // Show light value to 1 decimal place
  Serial.println(" lx"); // Unit
  Serial.println("──────────────────────────────"); 
}

void setup() {
  Serial.begin(9600); // Start serial communication
  while (!Serial); // Wait for serial monitor

  pinMode(MOISTURE_PIN, INPUT); // Set moisture pin as input

  Wire.begin(); // Start I2C communication

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) { // Start light sensor
    Serial.println("BH1750 initialised."); // Success message
  } else {
    Serial.println("BH1750 failed to initialise. Check wiring."); // Error message
  }

  connectToWiFi(); // Connect to WiFi

  ThingSpeak.begin(client); // Start ThingSpeak

  Serial.println("Setup complete. Sending data every 30 seconds."); // Ready message
}

void loop() {
  unsigned long currentTime = millis(); // Get current time

  if (currentTime - lastSendTime >= SEND_INTERVAL_MS) { // Check if 30s passed
    lastSendTime = currentTime; // Update last send time

    int   moistureRaw = readMoistureRaw(); // Read moisture
    float lightLux    = readLightLevel();  // Read light

    printReadings(moistureRaw, lightLux); // Print values

    sendToThingSpeak(moistureRaw, lightLux); // Send data to cloud
  }
}
