#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

int MONTION = 14;
//int BUTTON =  12;
int RELAY = 13;
bool relayState = false;
bool detectMontion = false;
bool ACTIVE = false;

BlynkTimer timer;

char auth[] = "714fc33986fa47da8b54498257028fc8";

char ssid[] = "Trung Tin";
char pass[] = "0968475345";


BLYNK_WRITE(V5) {
  ACTIVE = param.asInt();
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth,ssid,pass);
  pinMode(MONTION, INPUT);
  pinMode(RELAY,OUTPUT);
//  pinMode(BUTTON,INPUT_PULLUP);
//timer.setInterval(1000L, updateButtonWidgetIfNeeded);
}

void loop()
{
  Blynk.run();
  timer.run();
  if(digitalRead(MONTION)==HIGH)
  {
    detectMontion = true;
  }
  else
  {
    detectMontion = false;
  }
  
  if(detectMontion == true && ACTIVE == true)
  {
    digitalWrite(RELAY,HIGH);
    Blynk.notify("Phat hien xam nhap trai phep!");
  }
  else
  {
    digitalWrite(RELAY,LOW);
    relayState = false;
  }
}
  

