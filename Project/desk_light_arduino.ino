// Include libraries
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>

// WiFi name and password
const char* ssid = "YOUR_WIFI"; 
const char* password = "YOUR_WIFI_PASSWORD"; 

// MQTT broker details
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* client_id = "arduino_desk_light";

// MQTT topics used for sending data
const char* topic_motion = "tchehhal/desk/motion";
const char* topic_lux = "tchehhal/desk/lux";

// PIR motion sensor pin
const int PIR_PIN = 2;

// Variables to track motion and timing
bool motionDetected = false;
bool sessionActive = false;

// Stores the last time lux was published
unsigned long lastLuxPublish = 0;

// Time interval for sending lux data (2 seconds)
const unsigned long LUX_INTERVAL = 2000;

// Create WiFi and MQTT client objects
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Create BH1750 light sensor object
BH1750 lightMeter;

void setup() {

  // Start serial monitor
  Serial.begin(9600);

  // Set PIR sensor pin as input
  pinMode(PIR_PIN, INPUT);

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Keep trying until WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" Connected.");

  // Set MQTT server and port
  mqttClient.setServer(mqtt_server, mqtt_port);

  // Start I2C communication for BH1750 sensor
  Wire.begin();

  // Start light sensor
  lightMeter.begin();

  Serial.println("System ready.");
}

// Function to reconnect to MQTT if disconnected
void reconnectMQTT() {

  // Keep trying until MQTT connects
  while (!mqttClient.connected()) {

    Serial.print("Connecting to MQTT...");

    // Attempt MQTT connection
    if (mqttClient.connect(client_id)) {

      Serial.println(" Connected.");

    } else {

      // Wait 5 seconds before retrying
      Serial.print(" Failed. Retrying in 5s...");
      delay(5000);
    }
  }
}

void loop() {

  // Check MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  // Keep MQTT communication running
  mqttClient.loop();

  // Read the PIR motion sensor
  int pirState = digitalRead(PIR_PIN);

  // If motion is detected
  if (pirState == HIGH) {

    // Only send message once per detection
    if (!motionDetected) {

      motionDetected = true;

      // Publish motion detected message
      mqttClient.publish(topic_motion, "detected");

      Serial.println("Motion detected.");
    }

  } else {

    // Reset motion flag when no motion
    motionDetected = false;
  }

  // Send lux value every 2 seconds
  if (millis() - lastLuxPublish > LUX_INTERVAL) {

    // Read light level from BH1750 sensor
    float lux = lightMeter.readLightLevel();

    // Convert lux value to string with 1 decimal place
    String luxString = String(lux, 1);

    // Publish lux value to MQTT topic
    mqttClient.publish(topic_lux, luxString.c_str());

    // Print lux value to serial monitor
    Serial.print("Lux: ");
    Serial.println(luxString);

    // Update last publish time
    lastLuxPublish = millis();
  }
}
