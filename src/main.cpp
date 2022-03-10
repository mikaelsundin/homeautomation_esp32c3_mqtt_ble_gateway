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

  Serial1.begin(9600,SERIAL_8N1,5,-1);


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

//16 11 0B DF1-DF4 DF5-DF8 DF9-DF12 DF13 DF14 DF15 DF16[CS]

void pm1006_fsm(uint8_t data){
  static uint8_t buffer[20];
  static uint8_t index=0;
  buffer[index++] = data;

  /* Check Header */
  if(index >= 1 && buffer[0] != 0x16 ){
    index = 0;
  }

  if(index >= 2 && buffer[1] != 0x11 ){
    index = 0;
  }

  if(index >= 3 && buffer[2] != 0x0B ){
    index = 0;
  }

  /* Check if we received whole package */
  if(index >= 20){
      uint8_t checksum=0;
      for (uint8_t i = 0; i < 20; i++) {
            checksum += buffer[i];
      }

      if(checksum != 0){
        Serial.println("Invalid checksum"); 
      }else{
        uint16_t pm25 = (buffer[5] << 8) | buffer[6];


        for(int i=0;i<20;i++){
          Serial.printf("%02X ", buffer[i]);
        }
        Serial.println();
        Serial.printf("PM2.5 %u\n", pm25);

      }



    index=0;
  }

  
  





}



void loop() {
  //BleScannerTask();

  /* Handle MQTT */
  if(mqttClient.loop() == false){
    MqttConnect();
  }

  //Forward chars
  if(Serial1.available()){
    pm1006_fsm((uint8_t)Serial1.read());
  }

  //Example of payload.
  //16 11 0B DF1-DF4 DF5-DF8 DF9-DF12 DF13 DF14 DF15 DF16[CS]
  //Wait for header 16 11 0B 
  //Wait until whole packet is received
  //Check CS
  //Calculate PM2.5
  //MQTT Publish PM2.5

  //Implement state machine based on index.



}  