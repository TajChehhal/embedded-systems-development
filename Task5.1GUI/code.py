import tkinter as tk
import RPi.GPIO as GPIO

# Use BCM pin numbering - the GPIO numbers printed on the Pi
GPIO.setmode(GPIO.BCM)

# Map each room name to its GPIO pin number
LED_PINS = {
    "Living Room": 17,
    "Bathroom": 27,
    "Closet": 22
}

# Set each pin as an output and make sure all LEDs start off
for pin in LED_PINS.values():
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)


# Turns on the selected room's LED and turns off all others
def update_leds(selected_room):
    for room, pin in LED_PINS.items():
        GPIO.output(pin, GPIO.HIGH if room == selected_room else GPIO.LOW)


# Called when a radio button is clicked — passes the selection to update_leds
def on_radio_change():
    update_leds(selected_room.get())


# Turns off all LEDs, releases the GPIO pins, and closes the window
def on_exit():
    for pin in LED_PINS.values():
        GPIO.output(pin, GPIO.LOW)
    GPIO.cleanup()
    root.destroy()


# Main window 
root = tk.Tk()
root.title("Linda's Light Control")
root.geometry("320x280")
root.resizable(False, False)
root.configure(bg="#1e1e2e")

# Title and subtitle labels
title = tk.Label(
    root,
    text="Linda's Light Control",
    font=("Helvetica", 16, "bold"),
    bg="#1e1e2e",
    fg="#cdd6f4"
)
title.pack(pady=(20, 5))

subtitle = tk.Label(
    root,
    text="Select a room to turn on its light",
    font=("Helvetica", 10),
    bg="#1e1e2e",
    fg="#a6adc8"
)
subtitle.pack(pady=(0, 15))

# Tracks which radio button is currently selected
selected_room = tk.StringVar(value="")

# Frame to group the radio buttons together
radio_frame = tk.Frame(root, bg="#313244", bd=0, relief="flat", padx=20, pady=15)
radio_frame.pack(padx=30, fill="x")

# Create one radio button for each room
rooms = ["Living Room", "Bathroom", "Closet"]

for room in rooms:
    rb = tk.Radiobutton(
        radio_frame,
        text=room,
        variable=selected_room,  # All buttons share the same variable
        value=room,               # Each button sets the variable to its room name
        command=on_radio_change,  # Calls on_radio_change when clicked
        font=("Helvetica", 12),
        bg="#313244",
        fg="#cdd6f4",
        selectcolor="#45475a",
        activebackground="#313244",
        activeforeground="#cdd6f4",
        indicatoron=True,
        anchor="w",
        padx=10,
        pady=6,
        cursor="hand2"
    )
    rb.pack(fill="x")

# Exit button — calls on_exit when clicked
exit_btn = tk.Button(
    root,
    text="Exit",
    command=on_exit,
    font=("Helvetica", 11, "bold"),
    bg="#f38ba8",
    fg="#1e1e2e",
    activebackground="#eb6f92",
    activeforeground="#1e1e2e",
    relief="flat",
    padx=20,
    pady=8,
    cursor="hand2",
    bd=0
)
exit_btn.pack(pady=20)

# Also handle the window's X button the same way as the exit button
root.protocol("WM_DELETE_WINDOW", on_exit)

# Start the GUI event loop
root.mainloop()
