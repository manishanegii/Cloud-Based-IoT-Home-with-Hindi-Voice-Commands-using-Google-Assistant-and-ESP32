#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "WiFiName"                                                                 //Enter your Wi-Fi name in the double qoutes.
#define WIFI_PASS "Password"                                                                 //Enter your Wi-Fi password in the double qoutes.
 
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "________"                                                                 //Enter your adafruit.io user name 
#define MQTT_PASS "__________________"                                                       //Enter your adafruit.io feed active key. 
                                                                                           
WiFiClient client;                                                                         //Set up MQTT and WiFi clients
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
Adafruit_MQTT_Subscribe OnOff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/OnOff");  //Set up the feed you're subscribing to. Here my feed name is onoff


void setup()
{
  Serial.begin(115200);
  Serial.print("\n\nConnecting Wifi... ");               //Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.println("OK!");
  mqtt.subscribe(&OnOff);                                //Subscribe to the onoff feed
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
}

/* main code*/

void loop()
{
  MQTT_connect();                                        //Read from our subscription queue until we run out, or
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))   //wait up to 5 seconds for subscription to update
  {
    if (subscription == &OnOff)                          //If we are in here, a subscription is updated
    {
      Serial.print("OnOff: ");                           //Print the new value to the serial monitor
      Serial.println((char*) OnOff.lastread);
      if (!strcmp((char*) OnOff.lastread, "ON"))         //If the new value is  "ON", turn the light on.
      {
        digitalWrite(5, HIGH);                           //Active low logic 
      }
      else                                               //Otherwise, turn it off.
      {
        digitalWrite(5, LOW);
      }
    }
  }
  if (!mqtt.ping())                                      // ping the server to keep the mqtt connection alive
  {
    mqtt.disconnect();
  }
} 
 
void MQTT_connect() 
{
  int8_t ret;
  if (mqtt.connected())                                   // Stop if already connected.
  {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)                     // connect will return 0 for connected
  { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);                                       // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         while (1);                                       // basically die and wait for WDT to reset me
       }
  }
  Serial.println("MQTT Connected!");
}
