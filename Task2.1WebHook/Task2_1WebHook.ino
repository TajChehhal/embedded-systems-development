/*
  Since the DHT11/22 Temperature and Humidity and Analogue light sensors were not available for collection
  during this week, I used my soil moisture sensor from a previous unit to complete this task - as advised
  by my tutor via MSTeams.

  Thus, my two fields that were being read were the raw soil moisture sensor readings and the converted
  percentage from 0-100.
 */

// Includes libraries
#include <WiFiNINA.h>
#include <ThingSpeak.h>

// WiFi credentials 
const char* WIFI_SSID     = "WIFI_SSID"; // I have taken out my personal WiFi SSID here as well as password       
const char* WIFI_PASSWORD = "WIFI_PASSWORD";   

// ThingSpeak settings 
const unsigned long CHANNEL_ID      = CHANNEL_ID; // I have taken out my ThingSpeak settings as well       
const char*         WRITE_API_KEY   = "API_KEY"; 

// Pin definitions 
const int MOISTURE_PIN = A0;

// Timing for 30 seconds
const unsigned long SEND_INTERVAL_MS = 30000UL; // 30 seconds

// Globals 
WiFiClient client;
unsigned long lastSendTime = 0;

// Reads the raw analog value from the soil moisture sensor. Returns a value between 0 (dry) and 1023 (wet).
int readMoistureRaw() {
  return analogRead(MOISTURE_PIN);
}

// Converts the raw moisture reading to a percentage (0–100%).
float convertToPercentage(int rawValue) {
  return map(rawValue, 0, 1023, 0, 100);
}

// Connects to WiFi. Blocks until connected or prints an error.
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  int status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Check credentials.");
  }
}


// Sends sensor readings to ThingSpeak - bot raw and percentage
void sendToThingSpeak(int rawValue, float percentage) {
  // If wifi drops, reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }

  ThingSpeak.setField(1, rawValue);
  ThingSpeak.setField(2, percentage);

  int responseCode = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);

  if (responseCode == 200) {
    Serial.println("ThingSpeak update successful.");
  } else {
    Serial.print("ThingSpeak error. HTTP code: ");
    Serial.println(responseCode);
  }
}

// Prints the current sensor readings in the Serial Monitor
void printReadings(int rawValue, float percentage) {
  Serial.println("──────────────────────────────");
  Serial.print("Moisture Raw:        ");
  Serial.println(rawValue);
  Serial.print("Moisture Percentage: ");
  Serial.print(percentage, 1);
  Serial.println("%");
  Serial.println("──────────────────────────────");
}


void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor to open

  pinMode(MOISTURE_PIN, INPUT);

  connectToWiFi();
  ThingSpeak.begin(client);

  Serial.println("Setup complete. Sending data every 30 seconds.");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = currentTime;

    // Read sensor
    int   rawValue   = readMoistureRaw();
    float percentage = convertToPercentage(rawValue);

    // Display locally
    printReadings(rawValue, percentage);

    // Send to ThingSpeak
    sendToThingSpeak(rawValue, percentage);
  }
}
