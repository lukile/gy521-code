#include <WiFi.h>
#include <time.h>

const char* ssid = "under-wifi";
const char* password = "4F3EC76435781E341151852682";

int timezone = 3;
int dst = 0;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void loop() {
   time_t currentTime;
    struct tm *localTime;
    time( &currentTime );
    currentTime -= 2*30*60;
    localTime= localtime(&currentTime);

  Serial.println(localTime);
    
  //time_t now = localtime(nullptr);
  //Serial.println(ctime(&now));
  delay(1000);
}
