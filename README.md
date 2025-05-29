# ESP32_MQTT_START
Repo for ESP32-based start button to interface with M3 for EscapeWorks Denver


This program was developed for the ESP32 DEVKIT1. Please see the attached devkit1_pinout.jpg as a reference. Please attach the wires as follows:



GND: Connect to one terminal of the switch, and one terminal of the LED.

D2 (GPIO2): Connect to other terminal of the LED.

D4 (GPIO4): Connect to other terminal of the switch.



The button by default is set to publish the topic "M3/Stories/TE/State" with the payload of "Start", but these values can be changed in the definition section of the code from lines 6-11.

The LED is supposed to flash twice for successful publication, and then remain on while the game is in the "Start" state. When the game is in "Idle", it should remain off. The LED will flash 5 times rapidly if there is an error with network connectivity, MQTT connection, or publication of the topic.



In order to reflash the device with new settings, there are a few setup steps:

1. You must install the CP210x driver from Silicon Labs to interface correctly with the development board: https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers

2. Install the arduino IDE and install the ESP32MQTTClient by Chen Yijun available in the library manager within the program.

3. While connected, for debugging purposes you must select "Serial Monitor" from tools, and within the serial monitor, select "No Line Ending" and "115200 baud" from the drop down menus. This will now display feedback directly from the ESP32 in real time.
