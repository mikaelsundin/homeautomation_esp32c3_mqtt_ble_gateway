#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"
#include "pm1006.h"
#include "ble_scanner.h"
#include "ble_whitelist.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient); 
BleAdvestingScanner *BleScanner;

String mqttPrefix = "";
String mqttFilterPrefix = "";
PM1006 pm1006;

uint8_t MqttRetryCount = 5;


int ConnectToWiFi(void){
  int timeout = 30;
  int status;
  
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(DEFAULT_WIFI_SSID);

  Serial.printf("WIFI MAC %s\n", WiFi.macAddress().c_str());

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
     
     Serial.print("IP:");
     Serial.println(WiFi.localIP());
     
     Serial.print("GW:");
     Serial.println(WiFi.gatewayIP());
  }

  return status;
}

void mqttCallback(char* cstr_topic, byte* payload, unsigned int length) 
{
  auto topic = String(cstr_topic);
  auto str = String(payload, length);

  if(topic.endsWith("/filter/add")){
    BleWhitelistStorageAdd(str);
  }

  if(topic.endsWith("/filter/del")){
    BleWhitelistStorageDel(str);
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
  //TODO: limit number of retries before reset MCU...

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
    Serial.println("MQTT Connected");

    delay(10);

    mqttClient.publish(statusTopic.c_str(), "online", false);
    mqttClient.subscribe(filterTopic.c_str() , 0);
  }else{
    MqttRetryCount--;
    Serial.println("MQTT Failed");

    if(MqttRetryCount == 0){
      Serial.println("MQTT retry count zero. Restarting board");
      ESP.restart();
    }   
  }
}

void pm1006_callback_handler(uint16_t pm25){
  auto pm25Topic = mqttPrefix + "/vindrikning/value";
  char str[6];

  //Publish value
  sprintf(str, "%u", pm25);
  mqttClient.publish(pm25Topic.c_str(), str);
}


/**
 * Callback when BLE Manufacture data is received.
 * String will contain binary data
 */
void BleManufactureDataCallbackHandler(char* mac, char* payload){
  //Serial.printf("ManufactureData %s\n", mac);
  //Serial.printf("payload %s\n", payload);

  //Convert MAC to Compressed MAC.
  String adr = String(mac);
  adr.toUpperCase();
  adr.replace(":","");

  /* Build Topic */
  auto topic = mqttPrefix + String("/manufacturedata/") + String(adr) + String("/raw");

  /* Send raw payload. */
  mqttClient.publish(topic.c_str(), payload);
}

void setup() {
  Serial.begin(115200);
  Serial.println("boot");

  //PM1006 serial port.
  Serial1.begin(9600,SERIAL_8N1,5,-1);

  while(ConnectToWiFi() != WL_CONNECTED){
     Serial.println("WiFi Connection failed");
  }
  
  //PM1006 callback handler
  pm1006.RegisterCallback(pm1006_callback_handler);

  //Ble scanner
  BleScanner = BleAdvestingScanner::GetInstance(); 
  BleScanner->RegisterManufactureDataCallback(BleManufactureDataCallbackHandler);
}


void loop() {
  BleScanner->Loop();

  /* Handle MQTT */
  if(mqttClient.loop() == false){
    MqttConnect();    
  }

  //Forward received chars to PM1006
  if(Serial1.available()){
    pm1006.Process((uint8_t)Serial1.read());
  }
}  