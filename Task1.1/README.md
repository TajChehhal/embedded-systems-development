The goal of this system is to help Linda enter her home safely at night using 2 lights. This is achieved by pressing the button at the front door, turning on two lights - the porch light and the hallway light at the same time.
The porch light should stay on for 30 seconds which is enough time to find her key and open the door, and the hallway light stays on for 60 seconds, afterwhich, the lights turn off and ready to be pressed again.

Looking at the code in the 'Task1.1Lights_ON.ino' file, this design is implemented by first defining which Arduino pins the LEDs and button are connected to, as well as how long the specific light should stay on for and 
variables that track/remember which light is currently on and when it was switched on.

The setupPins() function runs once when the Arduino is powered on. This essentially lets the Arduino know which pins are inputs - the button - and which are outputs - the LEDs.
The turnLightsOn() function is called when the button is pressed and switches both lights on at the same time, as well as recording the current time. This is so that the program knows exactly when to turn off the specific
light later on.
The updateLights() function is the one that runs in the background continuously and checks how much time has passed since the lights were turned on and can then turn off each LED at its required time - the porch light at 30 
seconds and the hallway light at 60 seconds.
The checkButton() function just reads whether the button is being pressed or not. If the button is being pressed, and the lights are off, it calls the turnLightsOn() function and the LEDs are turned on. There is also a small
delay implemented that basically prevents multiple button triggers.
The setup() function runs only at startup to initialise everything, and the loop() function runs continuously while calling the checkButton() and updateLights() functions, making sure the system is responsive throughout.
