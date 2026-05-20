# Import MQTT library for communication
import paho.mqtt.client as mqtt

# Import Raspberry Pi GPIO library
import RPi.GPIO as GPIO

# Import threading for running MQTT loop separately
import threading

# Import tkinter for GUI
import tkinter as tk

# Import time module for cooldown timer
import time

import requests


#  --- GPIO SETUP ---

# GPIO pin for low brightness LED
LED_LOW = 17

# GPIO pin for medium brightness LED
LED_MEDIUM = 27 

# GPIO pin for high brightness LED
LED_HIGH = 22

# Set GPIO numbering mode to BCM
GPIO.setmode(GPIO.BCM)

# Set LED pins as output
GPIO.setup(LED_LOW, GPIO.OUT)
GPIO.setup(LED_MEDIUM, GPIO.OUT)
GPIO.setup(LED_HIGH, GPIO.OUT)


# --- MQTT SETUP ---

# MQTT broker address
MQTT_BROKER = "broker.hivemq.com"

# MQTT broker port
MQTT_PORT = 1883

# Topic for motion sensor data
TOPIC_MOTION = "tchehhal/desk/motion"

# Topic for light sensor data
TOPIC_LUX = "tchehhal/desk/lux"


# --- STATE ---

# Tracks if motion is detected
motion_active = False

# Stores current lux value
current_lux = 0.0

# Tracks if a session is active
session_active = False

# Stores current manual LED mode
manual_mode = None

# Tracks if cooldown is active
session_cooldown = False

# Stores cooldown end time
cooldown_end_time = 0

# Cooldown duration in seconds
COOLDOWN_SECONDS = 5



# --- THINGSPEAK ---

# ThingSpeak API key
THINGSPEAK_API_KEY = "ENTER_YOUR_API_KEY_HERE"

# ThingSpeak channel URL
THINGSPEAK_URL = "https://api.thingspeak.com/update"

# Function to publish session data to ThingSpeak
def publish_to_thingspeak(session_status, lux):
    try:
        payload = {
            "api_key": THINGSPEAK_API_KEY,
            "field1": session_status,
            "field2": lux
        }
        requests.get(THINGSPEAK_URL, params=payload)
        print(f"ThingSpeak updated — status: {session_status}, lux: {lux}")
    except Exception as e:
        print(f"ThingSpeak error: {e}")



# --- LED CONTROL ---

# Function to control LEDs based on brightness mode
def set_leds(mode):

    # Turn on low LED for low, medium, and high modes
    GPIO.output(LED_LOW, mode in ["low", "medium", "high"])

    # Turn on medium LED for medium and high modes
    GPIO.output(LED_MEDIUM, mode in ["medium", "high"])

    # Turn on high LED only for high mode
    GPIO.output(LED_HIGH, mode == "high")


# --- LIGHTING LOGIC ---

# Function to automatically update lighting
def update_lighting():

    # Check if manual mode is enabled
    if manual_mode is not None:
        set_leds(manual_mode)
        update_gui_mode(manual_mode.capitalize() + " (manual)")
        return

    # Turn LEDs off if no motion detected
    if not motion_active:
        set_leds("off")
        update_gui_mode("Off")
        return

    # Set mode based on lux level
    if current_lux > 400:
        mode = "off"
    elif current_lux > 80:
        mode = "low"
    elif current_lux > 30:
        mode = "medium"
    else:
        mode = "high"

    # Update LEDs and GUI
    set_leds(mode)
    update_gui_mode(mode.capitalize() + " (auto)")


# --- MQTT CALLBACKS ---

# Runs when MQTT client connects
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker.")

    # Subscribe to motion topic
    client.subscribe(TOPIC_MOTION)

    # Subscribe to lux topic
    client.subscribe(TOPIC_LUX)

# Runs when MQTT message is received
def on_message(client, userdata, msg):

    # Access global variables
    global motion_active, current_lux, session_active, session_cooldown, cooldown_end_time

    # Store topic and message payload
    topic = msg.topic
    payload = msg.payload.decode()

    # Handle motion sensor messages
    if topic == TOPIC_MOTION:

        # Check if motion detected
        if payload == "detected":

            # Check cooldown status
            if session_cooldown:

                # Disable cooldown if timer finished
                if time.time() > cooldown_end_time:
                    session_cooldown = False
                else:
                    return

            # Set motion as active
            motion_active = True

            # Start new session if inactive
            if not session_active:
                session_active = True
                session_label.config(text="Session: Active")
                publish_to_thingspeak(1, current_lux)
                print("Session started. ThingSpeak updated.")
                

    # Handle lux sensor messages
    elif topic == TOPIC_LUX:

        # Convert lux value to float
        current_lux = float(payload)

        # Update GUI lux display
        update_gui_lux(current_lux)

    # Update lighting after receiving data
    update_lighting()


# --- GUI UPDATES ---

# Update current lighting mode on GUI
def update_gui_mode(mode_str):
    mode_label.config(text=f"Mode: {mode_str}")

# Update lux value on GUI
def update_gui_lux(lux):
    lux_label.config(text=f"Lux: {lux:.1f} lx")


# --- MANUAL OVERRIDE ---

# Set manual lighting mode
def set_manual(mode):
    global manual_mode
    manual_mode = mode
    update_lighting()

# Reset system back to automatic mode
def reset_auto():
    global manual_mode
    manual_mode = None
    update_lighting()

# End current session manually
def end_session():

    # Access global variables
    global session_active, manual_mode, session_cooldown, cooldown_end_time, motion_active

    # Reset session states
    motion_active = False
    session_active = False
    manual_mode = None

    # Start cooldown timer
    session_cooldown = True
    cooldown_end_time = time.time() + COOLDOWN_SECONDS

    # Turn LEDs off
    set_leds("off")

    # Update GUI session label
    session_label.config(text="Session: Inactive")
    publish_to_thingspeak(0, current_lux)

    print("Session ended by user. Cooldown started.")


# --- CLEANUP ---

# Cleanly close application
def on_close():

    # Turn LEDs off
    set_leds("off")

    # Reset GPIO pins
    GPIO.cleanup()

    # Disconnect MQTT client
    mqtt_client.disconnect()

    # Close GUI window
    root.destroy()


# --- GUI SETUP ---

# Build the tkinter GUI
def build_gui():

    # Access global GUI elements
    global mode_label, lux_label, session_label, root

    # Create main window
    root = tk.Tk()

    # Set window title
    root.title("Desk Light Controller")

    # Set window size
    root.geometry("350x350")

    # Disable window resizing
    root.resizable(False, False)

    # Main title label
    tk.Label(root, text="Smart Desk Lighting System",
             font=("Arial", 14, "bold")).pack(pady=12)

    # Create status labels
    global lux_label, mode_label, session_label
    lux_label = tk.Label(root, text="Lux: -- lx", font=("Arial", 11))
    mode_label = tk.Label(root, text="Mode: --", font=("Arial", 11))
    session_label = tk.Label(root, text="Session: Inactive", font=("Arial", 11))

    # Display labels on GUI
    lux_label.pack(pady=4)
    mode_label.pack(pady=4)
    session_label.pack(pady=4)

    # Manual override section label
    tk.Label(root, text="Manual Override:",
             font=("Arial", 11)).pack(pady=8)

    # Create frame for buttons
    btn_frame = tk.Frame(root)
    btn_frame.pack()

    # Manual OFF button
    tk.Button(btn_frame, text="Off", width=7,
              command=lambda: set_manual("off")).grid(row=0, column=0, padx=4)

    # Manual LOW button
    tk.Button(btn_frame, text="Low", width=7,
              command=lambda: set_manual("low")).grid(row=0, column=1, padx=4)

    # Manual MEDIUM button
    tk.Button(btn_frame, text="Medium", width=7,
              command=lambda: set_manual("medium")).grid(row=0, column=2, padx=4)

    # Manual HIGH button
    tk.Button(btn_frame, text="High", width=7,
              command=lambda: set_manual("high")).grid(row=0, column=3, padx=4)

    # Button to return to automatic mode
    tk.Button(root, text="Reset to auto", width=16,
              command=reset_auto).pack(pady=10)

    # Button to end session
    tk.Button(root, text="End Session", width=16,
              command=end_session).pack(pady=4)

    # Run cleanup when window closes
    root.protocol("WM_DELETE_WINDOW", on_close)

    # Start GUI loop
    root.mainloop()


# --- MAIN ---

# Create MQTT client
mqtt_client = mqtt.Client()

# Assign connection callback
mqtt_client.on_connect = on_connect

# Assign message callback
mqtt_client.on_message = on_message

# Connect to MQTT broker
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

# Create separate thread for MQTT loop
mqtt_thread = threading.Thread(target=mqtt_client.loop_forever)

# Set thread as daemon
mqtt_thread.daemon = True

# Start MQTT thread
mqtt_thread.start()

# Launch GUI
build_gui()
