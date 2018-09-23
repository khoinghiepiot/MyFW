#include <FS.h> 
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Arduino.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ArduinoJson.h> 



char mqtt_server[40]="";
char mqtt_user[40]="homeassistant";
char mqtt_pass[40]="";
char mqtt_deviceid[40]="relay";
//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config


WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin5 = D5;

const int touchPin = D7;
int st;
int dangan;
bool strelay=0;
int a=0;

int val=0;


char message_buff[100];
String msgString;
String topicStr;
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void onoffnwf()
{
   st=digitalRead(touchPin);
if (st == 0)
dangan=1;

if (st==1 and dangan==1)
{
  if (strelay==0)
  {
    digitalWrite(ledPin5,LOW);
    strelay=1;
  }
  else
  {
    digitalWrite(ledPin5,HIGH);
    strelay=0;
  }
  dangan=0;
}



}
void setup_wifi() 
{
if (SPIFFS.begin()) {
    
    if (SPIFFS.exists("/config.json")) {

      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        
        size_t size = configFile.size();
        
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_user, json["mqtt_user"]);
          strcpy(mqtt_pass, json["mqtt_pass"]);
          strcpy(mqtt_deviceid, json["mqtt_deviceid"]);



        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }

  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 40);
  WiFiManagerParameter custom_mqtt_deviceid("deviceid", "mqtt deviceid", mqtt_deviceid, 40);
 
  WiFiManager wifiManager;


  wifiManager.setSaveConfigCallback(saveConfigCallback);


  //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,9,197), IPAddress(192,168,9,1), IPAddress(255,255,255,0));
  
 
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
wifiManager.addParameter(&custom_mqtt_deviceid);
  
  if (!wifiManager.autoConnect("BAPBAP", "bapbapbap")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    
    ESP.reset();
    delay(5000);
  }


  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_pass, custom_mqtt_pass.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_deviceid, custom_mqtt_deviceid.getValue());
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_pass"] = mqtt_pass;
    json["mqtt_deviceid"] = mqtt_deviceid;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
}
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}
void blink()
{
digitalWrite(LED_BUILTIN, HIGH);
delay(500);
digitalWrite(LED_BUILTIN, LOW);
delay(500);
digitalWrite(LED_BUILTIN, HIGH);
delay(500);
}


void callback(char *topic, byte *payload, unsigned int length)
{
    int i;
    
    topicStr = topic;
    for (i = 0; i < length; i++)
    {
        message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';

    msgString = String(message_buff);
    
    if (topicStr == "relay/lighttest")
{
        if (msgString == "1")
        {
            digitalWrite(ledPin5,LOW); // PIN HIGH will switch OFF the relay
            
            
            
            client.publish("relay/lightteststate","1");
            
            
            
        }
        if (msgString == "0")
        {
            digitalWrite(ledPin5,HIGH); // PIN LOW will switch ON the relay

            client.publish("relay/lightteststate","0");
            
        }
}

    
}
void reconnect()
{
    
    while (!client.connected())
    {
        a=a+1;
        
        delay(100);
        if (a==150)
        {
        a=0;
        
        
        if (client.connect(mqtt_deviceid,mqtt_user,mqtt_pass))
      
            {
            client.subscribe("relay/lightteststate");
            
            client.subscribe("relay/lighttest");
            
            Serial.println("mqtt ok");
            }
    }
        else if (a<150)
        {
         onoffnwf();
         Serial.println("chưa connect mqtt");
        }
}
}

void setup()
{
    
    delay(10);
    
    
   
    pinMode(touchPin, INPUT);
    pinMode(ledPin5, OUTPUT);
    Serial.begin(115200);
    setup_wifi();
    Serial.println(mqtt_server);
    Serial.println(mqtt_user);
    Serial.println(mqtt_pass);
    Serial.println(mqtt_deviceid);
    
    client.setServer(mqtt_server,1883); 
    client.setCallback(callback);
 
    
    
    

    
}

void loop()

    

{
   

st=digitalRead(touchPin);
if (st == 0)
dangan=1;

if (st==1 and dangan==1)
{
  if (strelay==0)
  {
    digitalWrite(ledPin5,LOW);
    strelay=1;
    client.publish("relay/lightteststate","1");
  }
  else
  {
    digitalWrite(ledPin5, HIGH);
    client.publish("relay/lightteststate","0");
    strelay=0;
  }
  dangan=0;
}


  


    
  




  
{

 if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}

  




}
  


