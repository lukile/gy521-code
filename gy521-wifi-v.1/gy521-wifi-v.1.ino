#include <HTTPClient.h>

#include <WiFiUdp.h>
#include <ETH.h>
#include <WiFiSTA.h>
#include <WiFiMulti.h>
#include <WiFiType.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFiScan.h>
#include <WiFiAP.h>
#include <WiFiGeneric.h>
#include <WiFi.h>



const char* ssid = "ssid-name";
const char* password = "wifi-password";

int counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("Waiting for connection...");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); 
}

void loop() {
  if(WiFI.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("localhost:27027/WaterDigitBuddy/performances/");
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.POST(structArrayAccelero);
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
  
  } else {
    Serial.println("Error in WiFi connection");
  }

  //Must be modified and send request only when session is over
  delay(30000);
}
