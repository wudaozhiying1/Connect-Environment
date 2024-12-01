#Arduino IoT Project: Indoor Comfort Monitoring
#This project is an Arduino-based IoT application that uses temperature, humidity, and wind speed data to calculate the Body Comfort Meteorological Index (BCMI). It determines whether indoor conditions are suitable for comfort.

#File Descriptions
#1. arduino_secrets.h
#This file contains sensitive configuration data, such as Wi-Fi and MQTT credentials, required for network and data communication.

#SECRET_SSID: Wi-Fi network SSID.
#SECRET_PASS: Wi-Fi network password.
#SECRET_MQTTUSER: MQTT username.
#SECRET_MQTTPASS: MQTT password.
#2. sketch_nov29a.ino
#The main Arduino sketch file that:

#Connects to a Wi-Fi network.
#Establishes MQTT communication.
#Reads data from temperature, humidity, and wind speed sensors.
#Calculates the BCMI (Body Comfort Meteorological Index) based on the sensor data.
#Determines and reports whether indoor conditions are comfortable.
#Features
#Real-Time Monitoring: Continuously reads data from sensors.
#BCMI Calculation: Combines environmental data into a comprehensive comfort index.
#MQTT Communication: Sends results to a cloud server for further analysis or logging.
#Prerequisites
#Hardware Requirements:
#Arduino board with Wi-Fi capability (e.g., ESP32, Arduino Uno with Wi-Fi shield).
#Sensors:
#Temperature and humidity sensor.
#Wind speed sensor.
#Breadboard and connecting wires.
#Software Libraries:
#WiFi.h or WiFiNINA.h for Wi-Fi communication.
#PubSubClient.h for MQTT communication.
#Sensor-specific libraries (e.g., DHT.h for temperature and humidity sensors).
#Setup Instructions
#Prepare arduino_secrets.h: Update the file with your Wi-Fi and MQTT credentials:

#define SECRET_SSID "Your-WiFi-SSID"
#define SECRET_PASS "Your-WiFi-Password"
#define SECRET_MQTTUSER "Your-MQTT-Username"
#define SECRET_MQTTPASS "Your-MQTT-Password"
#Configure Sensors:

#Connect the temperature, humidity, and wind speed sensors to the Arduino according to their pinout and your board's GPIO mapping.
#Modify the Code (if needed):

#Adjust sensor calibration constants or MQTT server details in the setup() function of sketch_nov29a.ino.
#Upload and Test:

#Upload the sketch to your Arduino board.
#Monitor the serial console to verify sensor readings, BCMI calculation, and MQTT communication.
#Cloud Integration (Optional):

#Set up an MQTT server to receive and analyze data.
#BCMI Calculation
#The BCMI is computed using the following parameters:

#Temperature (°C): Measures thermal comfort.
#Humidity (%): Affects perceived temperature.
#Wind Speed (m/s): Influences air circulation and cooling effect.
#The calculated index categorizes indoor conditions into:

#Comfortable
#Slightly uncomfortable
#Uncomfortable
#Notes
#Sensor Accuracy: Ensure sensors are calibrated for reliable BCMI calculation.
#Keep arduino_secrets.h secure: Protect your credentials to prevent unauthorized access.
#Expandability: You can integrate additional sensors or features (e.g., light levels or CO₂ monitoring) to enhance functionality.

#License
#This project is open-source and can be modified and shared freely. Please adhere to the licenses of any external libraries used.

