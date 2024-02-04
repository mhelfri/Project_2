Max Helfrich
ECE-218, Project 2 Code
Team member: Zaid Fafouri
02/03/24

Description of the system:
	Zaid and I designed Project 2, The Automatic Headlight Control System, to enhance driving safety and convenience by automating the headlight control system, a feature that is commonly used by most cars. In order to properly implement the system, various components must be accounted for, including: left and right low beam lights, a driver seat occupancy sensor, ignition button, engine on indicator, headlights setting selector, and a light sensor. The system features two primary subsystems: The ignition subsystem, and the headlights subsystem. The ignition subsystem allows the engine to start when the driver's seat is occupied, and the ignition button is pressed and released; in order to communicate that the engine has been turned on, the engine on indicator (blue LED) will be activated. If the driver isn’t seated and the ignition button is pressed, an error message will be displayed to the user: “driver must be seated.” When the ignition button is pressed and released while the engine is running, it turns off the car. The headlight subsystem operates in three modes: ON, OFF, and AUTO that can be set up using the potentiometer (less than 0.33 for OFF, between 0.33 and 0.66 for AUTO, over 0.66 for ON). The ON mode turns on the headlights (LEDs) while the OFF mode turns them off. In AUTO mode, the headlights respond to ambient light levels, turning off after a 2 seconds when it's sufficiently bright and turning on after a 1 second when it's dark. If there is ambient light level, the headlights state doesn’t change, and the light level difference must last for the full delay time for the headlights to toggle accordingly


System behavior tested
Results (Pass/Fail)
Ignition Subsystem
Engine (blue LED ON) start after ignition and driver seat buttons pressed 
Pass
Engine doesn’t start (blue LED OFF) if ignition button is pressed while driver seat button isn’t pressed
Pass
Engine keeps running (blue LED ON) even if the driver isn’t seated anymore
Pass
Engine stops running (blue LED OFF) when ignition button is pressed and engine is running
Pass
Headlights Subsystem
When set up to OFF mode (potentiometer less than 0.33), the headlights turns OFF (LEDs OFF)
Pass
When setup to ON mode (potentiometer over 0.66), the headlights turns ON (LEDs ON)
Pass
When setup to AUTO mode (potentiometer between 0.33 and 0.66) and the light sensor detects brightness, the headlights turns OFF (LEDs OFF) after 2 seconds
Pass
When setup to AUTO mode (potentiometer between 0.33 and 0.66) and the light sensor detects darkness, the headlights turns ON (LEDs ON) after 1 second
Pass
When setup to AUTO mode (potentiometer between 0.33 and 0.66) and the light sensor detects ambient light level, the headlights keep their current state
Pass


