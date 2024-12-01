#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>
#include "arduino_secrets.h"
#include <Adafruit_SCD30.h>
#include <math.h>

Adafruit_SCD30 scd30;

// 全局变量
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;

WiFiServer server(80);
WiFiClient wificlient;
WiFiClient mkrClient;
PubSubClient client(mkrClient);

char mqtt_topic_demo[] = "student/CASA0014/light/31/pixel/";

// RGB 颜色全局变量
int R = 0, G = 0, B = 0;

float calculateBCMI(float temperature, float humidity, float windSpeed) {
  float RH = humidity / 100.0;
  float term1 = (1.8 * temperature + 32);
  float term2 = -0.55 * (1 - RH) * (1.8 * temperature - 26);
  float term3 = -3.2 * sqrt(windSpeed);
  return term1 + term2 + term3;
}

void printComfortLevel(float bcmi) {
  int level = 0;

  // 确定等级
  if (bcmi >= 90) {
    level = 10;
  } else if (bcmi >= 86) {
    level = 9;
  } else if (bcmi >= 80) {
    level = 8;
  } else if (bcmi >= 76) {
    level = 7;
  } else if (bcmi >= 71) {
    level = 6;
  } else if (bcmi >= 59) {
    level = 5;
  } else if (bcmi >= 51) {
    level = 4;
  } else if (bcmi >= 39) {
    level = 3;
  } else if (bcmi >= 26) {
    level = 2;
  } else {
    level = 1;
  }

  // 设置 RGB 颜色
  switch (level) {
    case 10:
    case 9:
      R = 255, G = 0, B = 0; // 红色
      break;
    case 8:
    case 7:
      R = 255, G = 165, B = 0; // 橙色
      break;
    case 6:
    case 5:
      R = 255, G = 255, B = 0; // 黄色
      break;
    case 4:
    case 3:
      R = 0, G = 255, B = 0;   // 绿色
      break;
    case 2:
    case 1:
      R = 0, G = 0, B = 255;   // 蓝色
      break;
    default:
      R = 255, G = 255, B = 255; // 白色
      break;
  }
}

void sendmqttgreen(float bcmi) {
  // 设置灯光颜色
  printComfortLevel(bcmi);

  for (int j = 0; j < 12; j++) {
    // 发送 MQTT 消息
    char mqtt_message[100];
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": 0}", j, R, G, B);

    if (client.publish(mqtt_topic_demo, mqtt_message, false)) {
      Serial.println("Message published");
    } else {
      Serial.println("Failed to publish message");
    }
  }
}

void sendmqttclear() {
  for (int j = 0; j < 12; j++) {
    // 清除灯光
    char mqtt_message[100];
    sprintf(mqtt_message, "{\"pixelid\": %d, \"R\": 0, \"G\": 0, \"B\": 0, \"W\": 0}", j);

    if (client.publish(mqtt_topic_demo, mqtt_message, false)) {
      Serial.println("Message cleared");
    } else {
      Serial.println("Failed to clear message");
    }
  }
}
void loop() {
  if (scd30.dataReady()) {
    Serial.println("Data available!");
    if (!scd30.read()) {
      Serial.println("Error reading sensor data");
      return;
    }

    float temperature = scd30.temperature;
    float humidity = scd30.relative_humidity;
    float windSpeed = 2.5;
    float bcmi = calculateBCMI(temperature, humidity, windSpeed);

    Serial.print("BCMI: ");
    Serial.println(bcmi);

    sendmqttgreen(bcmi);
    delay(10000); // 每 10 秒发送一次数据
  }

  client.loop();
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
