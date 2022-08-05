#include "ESP8266WiFi.h"

void scanWiFi(void);

void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
}

void loop() 
{
  scanWiFi();
  Serial.println("--- Loop done ---");
}

void scanWiFi(void)
{
  int numberOfNetworks = WiFi.scanNetworks();
  for(int i=0; i<numberOfNetworks; i++)
  {
    Serial.println(String(WiFi.SSID(i) + " = " + WiFi.RSSI(i)));
  }
 
}
