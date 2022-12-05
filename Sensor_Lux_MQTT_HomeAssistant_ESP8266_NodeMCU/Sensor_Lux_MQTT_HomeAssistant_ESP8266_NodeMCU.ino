#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <ArduinoJson.h>

//Informações do WiFi
#define WIFISSID "TP-Link(Leo) 2.4ghz"
#define WIDIPASS "qwertyuiop"
#define ONE_WIRE_BUS 16 //GPI016 ou porta D0 do NodeMCU

OneWire oneWire(ONE_WIRE_BUS);

float lux=0.00,ADC_value=0.0048828125,LDR_value;

//Informações do Servidor MQTT
const char* mqttserver = "192.168.0.176";
int mqttserverport = 1883;
const char* mqttuser = "mqtt";
const char* mqttpass = "Naotemsenha0!";

//Variáveis
WiFiClient wifiClient;
PubSubClient ConexaoMQTT(wifiClient);
int UltimoValor = 0;
int TimeCounter = 0;
int PirState = LOW;
float illuminance;
String stateTopic = "home/mqtt_sensors/1/state";
bool statusPub = false;

void reconnect() {
  ConexaoMQTT.setServer(mqttserver, mqttserverport);
  while (!ConexaoMQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT");
    ConexaoMQTT.connect("esp8266",mqttuser,mqttpass);
    delay(3000);
  }
  Serial.println("MQTT conectado");
}

void setup() {
  pinMode(A0,INPUT);    //make analog pin A0 as input
  pinMode(D7, INPUT);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  IPAddress ip(192, 168, 0, 33);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 0, 1);
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(WIFISSID, WIDIPASS);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado! IP address: ");
  Serial.println(WiFi.localIP());

  if (!ConexaoMQTT.connected()) {
    reconnect();
  }

//  sendMQTTSensorLux1DiscoveryMsg();
  
  String LuxSensor = "{\"unique_id\": \"cc849ec8-748e-11ed-a1eb-0242ac120002\", \"icon\":\"mdi:sun-wireless\", \"device_class\":\"temperature\", \"unit_of_measurement\": \"lux\", \"name\": \"MQTT_Sensor_Lux_1\", \"state_topic\": \"homeassistant/sensor/lux/MQTT_Sensor_Lux_1/value\"}";
  
  statusPub = ConexaoMQTT.publish("homeassistant/sensor/lux/config",LuxSensor.c_str());
  Serial.print("Status Pub:");
  Serial.println(statusPub);
  Serial.println(LuxSensor.c_str());
 
  statusPub = ConexaoMQTT.publish("homeassistant/sensor/lux/MQTT_Sensor_Lux_1/availability","online");
  Serial.print("Status Pub:");
  Serial.println(statusPub);
  statusPub = ConexaoMQTT.publish("homeassistant/sensor/lux/MQTT_Sensor_Lux_1/value","1500");
    Serial.print("Status Pub:");
  Serial.println(statusPub);
}

void loop() {
  if (!ConexaoMQTT.connected()) {
    reconnect();
  }

  LDR_value=analogRead(A0);
  lux=((250.000000/(ADC_value*LDR_value))-50.000000)*10;
  char data[7];
  char *valor = dtostrf(lux,4,4,data);
  
  ConexaoMQTT.publish("homeassistant/sensor/lux/MQTT_Sensor_Lux_1/availability","online");
  ConexaoMQTT.publish("homeassistant/sensor/lux/MQTT_Sensor_Lux_1/value",valor);
  delay(5000);
}




//
//void sendMQTTSensorLux1DiscoveryMsg() {
//  String discoveryTopic = "homeassistant/sensor/Quarto/MQTT_Sensor_Lux_1/config";
//
//  DynamicJsonDocument doc(1024);
//  char buffer[256];
//
//  doc["name"] = "MQTT Sensor LUX 1";
//  doc["stat_t"]   = stateTopic;
//  doc["unit_of_meas"] = "lux";
//  doc["dev_cla"] = "illuminance";
//  doc["frc_upd"] = true;
//  doc["val_tpl"] = "{{ value_json.illuminance|default(0) }}";
//
//  size_t n = serializeJson(doc, buffer);
//  
//  Serial.println(discoveryTopic.c_str());
//  Serial.println(buffer);
//  
//  ConexaoMQTT.publish(discoveryTopic.c_str(), buffer, n);
//}
