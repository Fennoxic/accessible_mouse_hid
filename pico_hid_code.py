import time
import board
import analogio
import digitalio
import usb_hid
from adafruit_hid.mouse import Mouse

# Initialize the analog input
analog_x = analogio.AnalogIn(board.GP26)  # Horizontal movement (ADC0)
analog_y = analogio.AnalogIn(board.GP27)  # Vertical movement (ADC1)

# Set up GP03 as input with internal pull-up resistor
button = digitalio.DigitalInOut(board.GP3)
button.switch_to_input(pull=digitalio.Pull.UP)

# Initialize the Mouse
mouse = Mouse(usb_hid.devices)  # Pass the usb_hid.devices to the Mouse object

# Full scale for the analog input (65535 is the max value for 16-bit ADC)
MAX_ADC_VALUE = 65535

LEFT_THRESHOLD = int(MAX_ADC_VALUE * 0.7)
LEFT_MAX = int(MAX_ADC_VALUE * 0.9)
RIGHT_THRESHOLD = int(MAX_ADC_VALUE * 0.3)

BOTTOM_THRESHOLD = int(MAX_ADC_VALUE * 0.7)
BOTTOM_MAX = int(MAX_ADC_VALUE * 0.9)
TOP_THRESHOLD = int(MAX_ADC_VALUE * 0.3)

while True:
    # Read the analog value (0-65535)
    analog_x_val = analog_x.value
    analog_y_val = analog_y.value
    

    if analog_x_val > LEFT_THRESHOLD:
        # If the analog value is less than 30% of max, move mouse left
        if analog_x_val < LEFT_MAX:
            mouse.move(x=-10, y=0)  # Move left by 10 units
    
    elif analog_x_val < RIGHT_THRESHOLD:
        # If the analog value is greater than 70% of max, move mouse right
        mouse.move(x=10, y=0)  # Move right by 10 units
    
    #time.sleep(0.05)  # Small delay to prevent excessive mouse movement

    if analog_y_val > BOTTOM_THRESHOLD:
        if analog_y_val < BOTTOM_MAX:
            mouse.move(x=0, y=10)
    
    elif analog_y_val < TOP_THRESHOLD:
        mouse.move(x=0, y=-10)
          
    if not button.value:  # When the input is LOW (button pressed)
        mouse.click(Mouse.LEFT_BUTTON)  # Perform a left click
        time.sleep(0.5)  # Debounce delay to avoid multiple clicks
    
    time.sleep(0.05)  # Small delay to prevent excessive mouse movement