#include <ConfigManager.h>
ConfigManager configManager;

struct Config {
  char name[255];
  bool enabled;

  //char mqtt_host[255];
  char mqtt_port[10];
  char password[255];
  //char mqtt_topic[255];
} config;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
configManager.setAPName("Demo");
configManager.setAPFilename("/index.html");
configManager.addParameter("name", config.name, 20);
configManager.addParameter("enabled", &config.enabled);
configManager.addParameter("password", config.password, 20, set);
//configManager.addParameter("version", &meta.version, get);
configManager.begin(config);
}

void loop() {
  // put your main code here, to run repeatedly:
configManager.loop();

}
