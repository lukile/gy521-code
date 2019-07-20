#include <ArduinoJson.h>

#include <PageBuilder.h>
#include <PageStream.h>

#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include <WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>

WebServer Server;          // Replace with WebServer for ESP32
AutoConnect      Portal(Server);
void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  //WiFi.disconnect(false,true);
  Serial.println();

  Server.on("/", rootPage);
   Portal.handleClient();
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
}

void loop() {
  Portal.handleClient();
}
