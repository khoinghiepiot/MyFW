#include <FS.h> 
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Arduino.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <ArduinoJson.h> 

bool shouldSaveConfig = false;
//Mqtt info
char mqtt_server[40]=".duckdns.org";
char mqtt_user[40]="homeassistant";
char mqtt_pass[40]="";
char mqtt_deviceid[40]="relay1";

WiFiClient espClient;
PubSubClient client(espClient);

const byte ledPin5 = D5;
const byte ledPin6 = D6;
const byte ledPin7 = D7;

const int buttond1 = D1;
const int buttond2 = D2;
const int buttond3 = D3;
const int resetpin = D4;

int b=0;
int st;
int a=0;

int tthaicu;
int tthai1cu;
int tthai2cu;
int tthai3cu;

int stt =1 ;
int stt1=1;
int stt2 =1 ;
int stt3=1;
int sttold =1;
int sttold1=1;
int sttold2 =1;
int sttold3=1;

int sttoldkwf=1;
int sttoldkwf1=1;
int sttoldkwf2=1;
int sttoldkwf3=1;
int sttkwf;
int sttkwf1;
int sttkwf2;
int sttkwf3;

char message_buff[100];
String msgString;
String topicStr;

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void reset()
{
  st=digitalRead(resetpin);
  if (st == 0) {
    Serial.println(st);
    Serial.println(b);
    b++;
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);  
    if (b == 50)
    {
         b = 0;
        digitalWrite(LED_BUILTIN, HIGH);
        WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
        WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
        WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 40);
        WiFiManagerParameter custom_mqtt_deviceid("deviceid", "mqtt deviceid", mqtt_deviceid, 40);
        WiFiManager wifiManager;
        wifiManager.setConfigPortalTimeout(180);
        wifiManager.setSaveConfigCallback(saveConfigCallback);
        //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,9,197), IPAddress(192,168,9,1), IPAddress(255,255,255,0));
        wifiManager.addParameter(&custom_mqtt_server);
        wifiManager.addParameter(&custom_mqtt_user);
        wifiManager.addParameter(&custom_mqtt_pass);
        wifiManager.addParameter(&custom_mqtt_deviceid);
        if (!wifiManager.startConfigPortal("SetupSwitch", "lbminh2018")) {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
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
  }
}

void onoffnwf()
{          
    sttkwf = digitalRead(buttond1);
    sttkwf1 = digitalRead(buttond2);
    sttkwf2 = digitalRead(buttond3);   
          //d1 tuong ung d5
          {
          
          if (sttkwf != sttoldkwf)
          {
            
             sttoldkwf = sttkwf;
             if (sttkwf == 1)
        {
             digitalWrite(ledPin5, LOW);
             
        }
        else if (sttkwf == 0)
        {
          digitalWrite(ledPin5, HIGH);
            
            
        }
          }
          else if (sttkwf == sttoldkwf)
               sttoldkwf = sttkwf;
    }

//d2 tuong ung d6
    
    {
          
          if (sttkwf1 != sttoldkwf1)
          {
             sttoldkwf1 = sttkwf1;
             if (sttkwf1 == 1)
        {
             digitalWrite(ledPin6, LOW);
             
        }
        else if (sttkwf1 == 0)
        {
          digitalWrite(ledPin6, HIGH);
            
            
        }
          }
          else if (sttkwf1 == sttoldkwf1)
               sttoldkwf1 = sttkwf1;
    }

//d3 tuong ung d7
        {
          
          if (sttkwf2 != sttoldkwf2)
          {
             sttoldkwf2 = sttkwf2;
             if (sttkwf2 == 1)
        {
             digitalWrite(ledPin7, LOW);
             
        }
        else if (sttkwf2 == 0)
        {
          digitalWrite(ledPin7, HIGH);
            
            
        }
          }
          else if (sttkwf2 == sttoldkwf2)
               sttoldkwf2 = sttkwf2;
    }

}
    
void setup_wifi() 
{
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

  wifiManager.setConfigPortalTimeout(120);
  wifiManager.setSaveConfigCallback(saveConfigCallback);


  //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,9,197), IPAddress(192,168,9,1), IPAddress(255,255,255,0));
  
 
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
 wifiManager.addParameter(&custom_mqtt_deviceid);
  
  if (!wifiManager.autoConnect("SetupSwitch", "lbminh2018")) {
    Serial.println("failed to connect and hit timeout");
    delay(1000);
    

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
    
    if (topicStr == "relay/test")
{
        if (msgString == "1")
        {
            digitalWrite(ledPin5, LOW); // PIN HIGH will switch OFF the relay
            client.publish("relay/teststate","1");
            
        }
        if (msgString == "0")
        {
            digitalWrite(ledPin5, HIGH); // PIN LOW will switch ON the relay
            client.publish("relay/teststate","0");
            
        }
}
//test1
    else if (topicStr == "relay/test1")
    { // write your unique ID here
        if (msgString == "1")
        {
            digitalWrite(ledPin6, LOW); // PIN HIGH will switch OFF the relay
            client.publish("relay/teststate1","1");
            
        }
        if (msgString == "0")
        {
            digitalWrite(ledPin6, HIGH); // PIN LOW will switch ON the relay
            client.publish("relay/teststate1","0");
           
        }
    }

//test2

 else if (topicStr == "relay/test2")
    { // write your unique ID here
        if (msgString == "1")
        {
            digitalWrite(ledPin7, LOW); // PIN HIGH will switch OFF the relay
            client.publish("relay/teststate2","1");
            
        }
        if (msgString == "0")
        {
            digitalWrite(ledPin7, HIGH); // PIN LOW will switch ON the relay
            client.publish("relay/teststate2","0");
           
        }
    }
 //test3

 
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
            client.subscribe("relay/teststate");
            client.subscribe("relay/teststate1");
            client.subscribe("relay/teststate2");
            
            client.subscribe("relay/test");
            client.subscribe("relay/test1");

            client.subscribe("relay/test2");
            
            Serial.println("mqtt connect");
            }
    }
        else if (a<150)
        {
         onoffnwf();
         reset();
         Serial.println("chua connect mqtt");
        }
}
}

void setup()
{
    
    delay(10);
    pinMode(LED_BUILTIN, OUTPUT);
     
    pinMode(buttond2, INPUT_PULLUP);
    pinMode(buttond1, INPUT_PULLUP);
    pinMode(buttond3, INPUT_PULLUP);
    pinMode(resetpin, INPUT_PULLUP);
    pinMode(ledPin5, OUTPUT);
    pinMode(ledPin6, OUTPUT);
    pinMode(ledPin7, OUTPUT);
    
    Serial.begin(115200);
    
    setup_wifi();
    digitalWrite(LED_BUILTIN, HIGH);
    client.setServer(mqtt_server, 1883); // change port number as mentioned in your cloudmqtt console
    client.setCallback(callback);
 
    
    
    

    
}

void loop()
{ 
  

  
  
    



   
    stt = digitalRead(buttond1);
    stt1 = digitalRead(buttond2);
    stt2 = digitalRead(buttond3);
   
 st=digitalRead(resetpin);

 if (st==1)
 {
 b=0;
 digitalWrite(LED_BUILTIN, HIGH);
 }
    //test1
    {
    if (topicStr == "relay/teststate")
    { 
        if (msgString == "1")
        {
            tthaicu = 1;
         
        }
        if (msgString == "0")
        {
            tthaicu=0;
            
        }
    }
    
    }
     
     //test2
     
     {
    if (topicStr == "relay/teststate1")
    { 
        if (msgString == "1")
        {
            tthai1cu = 1;
         
        }
        if (msgString == "0")
        {
            tthai1cu=0;
            
        }
    }
    
    }
    
    
    //test3
         
     {
    if (topicStr == "relay/teststate2")
    { 
        if (msgString == "1")
        {
            tthai2cu = 1;
         
        }
        if (msgString == "0")
        {
            tthai2cu=0;
            
        }
    }
    
    }
    
 //test4

     
   
///HET

    //MAINSTATE1
    {
    if (stt != sttold)
    { 
      {
      if (tthaicu == 1)
        
        {
            digitalWrite(ledPin5, HIGH); // PIN HIGH will switch OFF the relay
            client.publish("relay/teststate","0");
            
        }
        if (tthaicu == 0)
        {
            digitalWrite(ledPin5, LOW); // PIN LOW will switch ON the relay
            client.publish("relay/teststate","1");
            
        }
      }
      
      sttold = stt;


      }
else if (stt == sttold)
{

    sttold = stt;

}
    }



    //MAINSTATE2

{       
if (stt1 != sttold1)
    { 
      
        {
       if (tthai1cu == 1)
        
        {
            digitalWrite(ledPin6, HIGH); // PIN HIGH will switch OFF the relay
            client.publish("relay/teststate1","0");
            
        }
        if (tthai1cu == 0)
        {
            digitalWrite(ledPin6, LOW); // PIN LOW will switch ON the relay
            client.publish("relay/teststate1","1");
            
        }
        
        }
   sttold1 = stt1;


      }
else if (stt1 == sttold1)
{

    sttold1 = stt1;

}
} 
    //MAINSTATE3

   {
    if (stt2 != sttold2)
    { 
      {
      if (tthai2cu == 1)
        
        {
            digitalWrite(ledPin7, HIGH); // PIN HIGH will switch OFF the relay
            client.publish("relay/teststate2","0");
            
        }
        if (tthai2cu == 0)
        {
            digitalWrite(ledPin7, LOW); // PIN LOW will switch ON the relay
            client.publish("relay/teststate2","1");
            
        }
      }
      
      sttold2 = stt2;


      }
else if (stt2 == sttold2)
{

    sttold2 = stt2;

}
    }
  

    //MAINSTATE4

   


  
{

 if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}

  reset();

}




  



