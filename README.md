# accessible_mouse_hid
Prototype code for an accessible mouse using an Arduino Nano to read from 2 ultrasonic sensors (HC-SP04) and a prototype keypad, and feed output to a Raspberry Pi Pico using HID with CircuitPython to control mouse movement and left click.

Hardware Setup:
Connect two HC-SR04 Ultrasonic sensors and any button to an Arduino Nano. Set up the sensors to be pointing perpendicular to each other
Wire two outputs from the Nano that correspond to the HC-SR04 sensors to two analog inputs on a Raspberry Pi Pico using an RC filter, and the digital output from the keypad button to a digital input on the Pico.
The Pico must be connected to your PC via its built-in USB functionality
*CIRCUIT DIAGRAM NOT AVAILABLE AT THIS TIME

Software:

Download the nano_sensor_read.ino to the Adruino Nano (this will likely work on most other Arduino boards as well, but may require some modificiations)

Flash the Pico with CircuitPython
- Download the latest stable release of CircuitPython here: https://circuitpython.org/board/raspberry_pi_pico/
- Copy the resulting .uf2 file onto the 'RPI-RP2 D:' drive that will show up when you plug in the Pico. The board will disappear and then reappear as a CIRCUITPY Drive
- Download the appropriate bundle from the CircuitPython website here: https://circuitpython.org/libraries
- Extract the folder, and in the lib folder, find the adafruit_hid folder. Copy this folder into the lib folder on the CIRCUITPY device
- Download usb-hid-map.py here: https://github.com/treguly/pico-hid/blob/main/src/lib/usb_hid_map.py
- Copy the file into the lib folder on the CIRCUITPY device
- Download pico_hid_code.py and replace the code.py file on the CIRCUITPY device with the pico_hid_code
- MAKE SURE TO RENAME THE FILE BACK TO CODE.PY SO THE PICO WILL RUN THE FILE PROPERLY

(Calibration and editing of thresholds and distances may be required depending on physical setup of the HC-SR04s)
