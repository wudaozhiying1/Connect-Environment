# Arduino IoT Project: Indoor Comfort Monitoring
This project is an Arduino-based IoT application that uses temperature, humidity, and wind speed data to calculate the Body Comfort Meteorological Index (BCMI). It determines whether indoor conditions are suitable for comfort.</br>

# File Descriptions
1. arduino_secrets.h</br>
This file contains sensitive configuration data, such as Wi-Fi and MQTT credentials, required for network and data communication.</br>

#SECRET_SSID: Wi-Fi network SSID.</br>
#SECRET_PASS: Wi-Fi network password.</br>
#SECRET_MQTTUSER: MQTT username.</br>
#SECRET_MQTTPASS: MQTT password.</br>
# sketch_nov29a.ino
The main Arduino sketch file that:</br>

Connects to a Wi-Fi network.</br>
Establishes MQTT communication.</br>
Reads data from temperature, humidity, and wind speed sensors.</br>
Calculates the BCMI (Body Comfort Meteorological Index) based on the sensor data.</br>
Determines and reports whether indoor conditions are comfortable.</br>
# Features
Real-Time Monitoring: Continuously reads data from sensors.</br>
BCMI Calculation: Combines environmental data into a comprehensive comfort index.</br>
MQTT Communication: Sends results to a cloud server for further analysis or logging.</br>
# Prerequisites
1.Hardware Requirements:
Arduino board with Wi-Fi capability (e.g., ESP32, Arduino Uno with Wi-Fi shield).</br>
2.Sensors:</br>
Temperature and humidity sensor.</br>
3.Wind speed sensor.</br>
Breadboard and connecting wires.</br>
4.Software Libraries:</br>
WiFi.h or WiFiNINA.h for Wi-Fi communication.</br>
# Setup Instructions</br>
Prepare arduino_secrets.h: Update the file with your Wi-Fi and MQTT credentials:</br>
#define SECRET_SSID "Your-WiFi-SSID"</br>
#define SECRET_PASS "Your-WiFi-Password"</br>
#define SECRET_MQTTUSER "Your-MQTT-Username"</br>
#define SECRET_MQTTPASS "Your-MQTT-Password"</br>
# Configure Sensors:
Connect the temperature, humidity, and wind speed sensors to the Arduino according to their pinout and your board's GPIO mapping.</br>
Modify the Code:
Adjust sensor calibration constants or MQTT server details in the setup() function of sketch_nov29a.ino.</br>
# Upload the sketch to your Arduino board.
Monitor the serial console to verify sensor readings, BCMI calculation, and MQTT communication.</br>
# Cloud Integration :

Set up an MQTT server to receive and analyze data.</br>
BCMI Calculation</br>
The BCMI is computed using the following parameters:</br>

Temperature (°C): Measures thermal comfort.</br>
Humidity (%): Affects perceived temperature.</br>
Wind Speed (m/s): Influences air circulation and cooling effect.</br>
The calculated index categorizes indoor conditions into:</br>
Quite comfortable</br>
Comfortable</br>
Slightly uncomfortable</br>
Uncomfortable</br>
Very uncomfortable</br>
# Notes
Sensor Accuracy: Ensure sensors are calibrated for reliable BCMI calculation.</br>
Keep arduino_secrets.h secure: Protect your credentials to prevent unauthorized access.</br>
Expandability: You can integrate additional sensors or features (e.g., light levels or CO₂ monitoring) to enhance functionality.</br>

# License
This project is open-source and can be modified and shared freely. Please adhere to the licenses of any external libraries used.

