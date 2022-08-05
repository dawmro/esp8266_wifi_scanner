#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

#include "ESP8266WiFi.h"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-wifi-password"
#define INFLUXDB_URL "https://europe-west1-1.gcp.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "your-INFLUXDB_TOKEN"
#define INFLUXDB_ORG "your-INFLUXDB_ORG"
#define INFLUXDB_BUCKET "your-INFLUXDB_BUCKET"

// Set timezone string according to https://sites.google.com/a/usapiens.com/opnode/time-zones
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Sensor name on dashboard
Point sensor("scanner_001");

void scanWiFi(void);


void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  // Get accurate time
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
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
    sensor.clearFields();
    sensor.addField(String(WiFi.SSID(i)), WiFi.RSSI(i));
    Serial.print("Writing: ");
    Serial.println(client.pointToLineProtocol(sensor));
    
    // If no Wifi signal, try to reconnect
    if (wifiMulti.run() != WL_CONNECTED) 
    {
      Serial.println("Wifi connection lost");
    }
    
    // Write point
    if (!client.writePoint(sensor)) 
    {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }  
  }
  
}
