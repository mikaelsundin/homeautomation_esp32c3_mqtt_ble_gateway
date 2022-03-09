#include <Arduino.h>
#include "ble.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"


WiFiClient espClient;
PubSubClient mqttClient(espClient); 

String mqttPrefix = "";
String mqttFilterPrefix = "";


int ConnectToWiFi(void){
  int timeout = 30;
  int status;
  
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(DEFAULT_WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.persistent(true);
  WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASS);

  do{
    delay(1000);
    status = WiFi.status();      
  }while( (status != WL_CONNECTED) && (timeout-- > 0) );

  //Print some information.
  if(status == WL_CONNECTED){
     Serial.println("Wifi ok");
     Serial.println("IP:");
     Serial.print(WiFi.localIP());
     Serial.println("GW:");
     Serial.print(WiFi.gatewayIP());
  }

  return status;
}

void mqttCallback(char* cstr_topic, byte* payload, unsigned int length) 
{
  auto topic = String(cstr_topic);

  if(topic.endsWith("/filter/add")){
    Serial.println("Filter add");
  }

  if(topic.endsWith("/filter/del")){
    Serial.println("Filter del");    
  }
}

/**
 * @brief Get GetMqttClientID based on WIFI MAC.
 */
String GetMqttClientID(){
  auto s = WiFi.macAddress();
  s.replace(":","");
  return s;
}


void MqttConnect()
{
  //Construct Prefix
  mqttPrefix = String(DEFAULT_MQTT_PREFIX);
  mqttPrefix.replace("%mac%", GetMqttClientID());

  //build Topics.
  auto statusTopic = mqttPrefix + "/status";
  auto filterTopic = mqttPrefix + "/filter/#";

  //Configure MQTT client
  mqttClient.setCallback(mqttCallback);
  mqttClient.setServer(DEFAULT_MQTT_IP, DEFAULT_MQTT_PORT);

  //Connect to MQTT
  auto connected = mqttClient.connect(
      GetMqttClientID().c_str(), 
      DEFAULT_MQTT_USER, DEFAULT_MQTT_PASS,
      statusTopic.c_str(), 0, true, "offline");


  if(connected){
    Serial.print("MQTT Connected");

    mqttClient.publish(statusTopic.c_str(), "online");
    mqttClient.subscribe(filterTopic.c_str() , 0);
  }else{
    Serial.print("MQTT Failed");
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("boot");


  while(ConnectToWiFi() != WL_CONNECTED){
     Serial.println("WiFi Connection failed");
  }


  /*

  BleScannerInit();
  Serial.println("BleScannerStart Done");

  BleScannerWhitelistAdd("90:FD:9F:4D:60:CB");

  //Example of json document
  StaticJsonDocument<200> doc;

  doc["device"] = "gps";
  doc["rssi"] = 1351824120;
  doc["manufactureData"] = 1351824120;

  serializeJsonPretty(doc, Serial);
  */

}

void loop() {
  //BleScannerTask();

  /* Handle MQTT */
  if(mqttClient.loop() == false){
    MqttConnect();
  }




}  