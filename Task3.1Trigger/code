#include <Wire.h> // Library for I2C communication
#include <BH1750.h> // Library for the BH1750 light sensor
#include <WiFiNINA.h> // Library to connect Arduino to WiFi
#include <ArduinoMqttClient.h> // Library for MQTT messaging

const char* WIFI_SSID = "WIFI_NAME";       
const char* WIFI_PASSWORD = "WIFI_PASSWORD";   
const char* MQTT_BROKER = "broker.hivemq.com"; // MQTT broker address

const int   MQTT_PORT = 1883; // MQTT broker port
const char* TOPIC_LUX = "terrarium/lux"; // Topic to send light data
const char* TOPIC_EVENT = "terrarium/event"; // Topic to send events
const float SUNLIGHT_THRESHOLD = 100.0; // Lux value that counts as sunlight
const unsigned long SEND_INTERVAL = 5000; // Time between sends (5 seconds)

BH1750 lightMeter; // Create light sensor object
WiFiClient wifiClient; // Create WiFi client
MqttClient mqttClient(wifiClient); // Create MQTT client using WiFi

bool sunlightOn = false; // Track if sunlight is currently detected
unsigned long lastSendTime = 0; // Store last time data was sent

void setup() {
  Serial.begin(9600); // Start serial communication
  Wire.begin(); // Start I2C communication
  lightMeter.begin(); // Start the light sensor

  // Connect to WiFi
  Serial.print("Connecting to WiFi"); // Print connection message
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) { // Try to connect to WiFi
    Serial.print("."); // Show connection progress
    delay(1000); // Wait 1 second before retrying
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString()); // Show IP address

  mqttClient.setId("terrarium_taj_001"); // Set MQTT client ID
  while (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) { // Try to connect to MQTT broker
    Serial.print("."); // Show connection progress
    delay(1000); // Wait 1 second before retrying
  }

  Serial.println("\nMQTT connected!"); // Confirm MQTT connection
}

void loop() {
  mqttClient.poll(); // Keep MQTT connection active

  unsigned long now = millis(); // Get current time
  if (now - lastSendTime >= SEND_INTERVAL) { // Check if 5 seconds passed
    lastSendTime = now; // Update last send time

    float lux = lightMeter.readLightLevel(); // Read light level from sensor
    Serial.print("Light: "); // Print label
    Serial.print(lux); // Print lux value
    Serial.println(" lx"); // Print unit

    // Publish lux reading every 5 seconds
    mqttClient.beginMessage(TOPIC_LUX); // Start MQTT message
    mqttClient.print(lux); // Add lux value to message
    mqttClient.endMessage(); // Send message

    // Trigger: sunlight just started
    if (lux >= SUNLIGHT_THRESHOLD && !sunlightOn) { // If lux crosses threshold
      sunlightOn = true; // Mark sunlight as on
      mqttClient.beginMessage(TOPIC_EVENT); // Start event message
      mqttClient.print("SUNLIGHT_START"); // Send sunlight start event
      mqttClient.endMessage(); // Send message
      Serial.println(">> Trigger: SUNLIGHT_START"); // Print event

    // Trigger: sunlight just stopped
    } else if (lux < SUNLIGHT_THRESHOLD && sunlightOn) { // If lux drops below threshold
      sunlightOn = false; // Mark sunlight as off
      mqttClient.beginMessage(TOPIC_EVENT); // Start event message
      mqttClient.print("SUNLIGHT_STOP"); // Send sunlight stop event
      mqttClient.endMessage(); // Send message
      Serial.println(">> Trigger: SUNLIGHT_STOP"); // Print event
    }
  }
}
