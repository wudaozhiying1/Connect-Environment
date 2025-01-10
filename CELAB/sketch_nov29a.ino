#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>
#include "arduino_secrets.h"
#include <Adafruit_SCD30.h>
#include <math.h>
Adafruit_SCD30 scd30;
/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below

#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
 */

// Wi-Fi and MQTT server credentials
const char* ssid = SECRET_SSID;            // Wi-Fi network SSID (stored in a separate secrets file)
const char* password = SECRET_PASS;        // Wi-Fi network password (stored in a separate secrets file)
const char* mqtt_username = SECRET_MQTTUSER; // MQTT username (stored in a separate secrets file)
const char* mqtt_password = SECRET_MQTTPASS; // MQTT password (stored in a separate secrets file)
const char* mqtt_server = "mqtt.cetools.org"; // MQTT broker server address
const int mqtt_port = 1884;                // Port number for MQTT connection
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiServer server(80);
WiFiClient wificlient;

WiFiClient mkrClient;
PubSubClient client(mkrClient);

char mqtt_topic_demo[] = "student/CASA0014/light/31/pixel/";

int R = 0, G = 0, B = 0;

float calculateBCMI(float temperature, float humidity, float windSpeed) {
  float RH = humidity/100;
  float term1 = (1.8 * temperature + 32);
  float term2 = -0.55 * (1 - RH) * (1.8 * temperature - 26);
  float term3 = -3.2 * sqrt(windSpeed);
  return term1 + term2 + term3;
}
// Function to determine and display comfort level based on BCMI
void printComfortLevel(float bcmi) {
  int level = 0;

 // Determine comfort level based on BCMI thresholds
  if (bcmi >= 85) {
    level = 9;
  } else if (bcmi >= 79) {
    level = 8;
  } else if (bcmi >= 75) {
    level = 7;
  } else if (bcmi >= 70) {
    level = 6;
  } else if (bcmi >= 58) {
    level = 5;
  } elseif (bcmi >= 50){
    level = 4; 
  } elseif (bcmi >= 38){
    level = 3; 
  } elseif (bcmi >= 25){
    level = 2; 
  } elseif (bcmi >= 0){
    level = 1; 
  }
  // Set RGB color based on comfort level
  switch (level) {
    case 9:// Extreme discomfort (highest BCMI range)
      R = 255, G = 255, B = 255; 
      break;
    case 8:
      R = 100, G = 0 , B = 100; 
      break;
    case 7:
      R = 100, G = 100, B = 100; 
      break;
    case 6:
      R = 255, G = 255, B = 0;   
      break;
    case 5:
      R = 255, G = 0, B = 255;   
      break;
    case 4:
      R = 0, G = 255, B = 255;  
      break;
    case 3:
      R = 255, G = 0, B = 0;  
      break;
    case 2:
      R = 0, G = 255, B = 0;   
      break;
    case 1:// 
      R = 0, G = 0, B = 255;   
      break;
    default: // Unexpected value
      R = 0, G = 0, B = 0; // white
      break;
  }
}

void setup() {
  // Start the serial monitor to show output
  Serial.begin(115200);
  delay(1000); 
  if (!scd30.begin()) {
    Serial.println("Failed to find SCD30 chip");
    while (1) { delay(10); } // stop 
  }
  WiFi.setHostname("ucfndz0");    
  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("setup complete");
}

void loop() {  
  if (scd30.dataReady()) {
    // read data from SCD30
    if (!scd30.read()) {
      Serial.println("Error reading sensor data");
      return;
    }
  }
  float temperature = scd30.temperature;
  float humidity = scd30.relative_humidity;
  float windSpeed = 2.5;
  float bcmi = calculateBCMI(temperature, humidity, windSpeed);

    Serial.print("BCMI: ");
    Serial.println(bcmi);       
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

  if (!client.connected()) {
    reconnectMQTT();
  }
  if (WiFi.status() != WL_CONNECTED){
    startWifi();
  }

  // check for messages from the broker and ensuring that any outgoing messages are sent.
  client.loop();

  sendmqttgreen(temperature, humidity, windSpeed);

  delay(1000);

  // sendmqttclear();
  // delay(2000);

  
}

void sendmqttgreen(float temperature,float humidity, float windSpeed) {
  // set the color
  char mqtt_message_3[100];
  float bcmi = calculateBCMI(temperature, humidity, windSpeed);
  printComfortLevel(bcmi);
  sprintf(mqtt_message_3, "{\"brightness\": %d}", 119);
  client.publish("student/CASA0014/light/31/brightness/", mqtt_message_3);
  for (int j = 0; j < 12; j++) {
    // send MQTT message
    char mqtt_message[100];
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": 0}", j, R, G, B);

    if (client.publish(mqtt_topic_demo, mqtt_message, false)) {
      // Serial.println("Message published");
    } else {
      // Serial.println("Failed to publish message");
    }
  }
}

void sendmqttclear() {
  for (int j = 0; j < 12; j++) {
    // clear the light
    char mqtt_message[100];
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": 0, \"G\": 0, \"B\": 0, \"W\": 0}", j);

    if (client.publish(mqtt_topic_demo, mqtt_message, false)) {
      Serial.println("Message cleared");
    } else {
      Serial.println("Failed to clear message");
    }
  }
}

void startWifi(){
    
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Function for connecting to a WiFi network
  // is looking for UCL_IoT and a back up network (usually a home one!)
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    // loop through all the networks and if you find UCL_IoT or the backup - ssid1
    // then connect to wifi
    Serial.print("Trying to connect to: ");
    Serial.println(ssid);
    for (int i = 0; i < n; ++i){
      String availablessid = WiFi.SSID(i);
      // Primary network
      if (availablessid.equals(ssid)) {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
          delay(600);
          Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("Connected to " + String(ssid));
          break; // Exit the loop if connected
        } else {
          Serial.println("Failed to connect to " + String(ssid));
        }
      } else {
        Serial.print(availablessid);
        Serial.println(" - this network is not in my list");
      }

    }
  }


  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED){
    startWifi();
  } else {
    //Serial.println(WiFi.localIP());
  }

  // Loop until we're reconnected
  while (!client.connected()) {    // while not (!) connected....
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // ... and subscribe to messages on broker
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, int length) {
  // Handle incoming messages
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}
