#include <ArduinoJson.h>

#include <MPU6050_tockn.h>

#include <SPI.h>

#include <time.h>

#include<Wire.h>
#include <array>

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

MPU6050 mpu6050(Wire);

int red = 0;
int green = 4;
int blue = 16;
String device_id = "WDB-001A";
String user_id;
String user_token;

long timer = 0;
long timerStartSession = 0;
double arraySpeeds[1000];
int cpt = 0;
double currentV = 0;
double currentY = 0;
double previousV = 0;
double previousY = 0;

double distance = 0;
double averageSpeed = 0;

String averageSpeedJson;
String distanceJson;

float minimalHighAccY = 0.20;
float minimalLowAccY = -0.20;

String datePerformance;
String startTime;
String endTime;

int flagEclairage = 0;
int firstTimer = 0;
int lastTimer = 0;

int isPerformanceStart = 0;
int isWifiStart = 0;

const char* ssid = "PriseDeLaBastille";
const char* password = "flow4321";

int PinSeuilLumiere = 2;   // Broche Numérique mesure d'éclairement

void setup(){
  Serial.begin(9600);
  //Wire.begin(SDA, SCL);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  timerStartSession = millis();

  pinMode(PinSeuilLumiere, INPUT);

  
  /*WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Waiting for connection...\n");
  }
  
  Serial.println();*/

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  
  /*Serial.print("Connected, IP address: \n");
  Serial.println(WiFi.localIP()); */

  connection();
}

void loop(){
  int eclaire = digitalRead(PinSeuilLumiere); 

  Serial.println(eclaire);

   if(eclaire == 1 && flagEclairage == 0) {
      flagEclairage = 1;
      firstTimer = millis();
      delay(2000);
  }
  
  if(eclaire == 0 && flagEclairage == 1) {
    lastTimer = (millis() - firstTimer) / 1000; // en sec
    Serial.println(lastTimer);
    
    if(lastTimer >= 3 && lastTimer < 6) // start or stop performances
    {
      Serial.println("start/stop performance");
      delay(800);
      flagEclairage = 0;
      if(isPerformanceStart == 0) {
        isPerformanceStart = 1;
        digitalWrite(blue, HIGH);
      } else if(isPerformanceStart == 2) {
        averageSpeed = averageArray(arraySpeeds, cpt);
        averageSpeedJson = "\"speed\" : " + (String)averageSpeed + ",";
  
        distance = currentV * ((millis() - timerStartSession)/1000);
        distanceJson = "\"distance\" : " + (String)distance + ",";
  
        Serial.print("\nvitesse moyenne : ");Serial.print(currentV);Serial.print("m/s\n");
        Serial.print("Distance moyenne : ");Serial.print(distance); Serial.print("m\n");

        endTime = "\"endTime\" : \"" + (getCurrentDate() + getCurrentTime()) + "\"";
        sendToApi(datePerformance, startTime, averageSpeedJson, distanceJson, endTime);
        isPerformanceStart = 0; // Arrete les mesures
        digitalWrite(blue, LOW);
        sleep(50);
      } 
      
    } else if(lastTimer >= 6)  // start or stop synchronization Wifi
    {
      Serial.println("start/stop wifi");
      delay(800);
       if(isWifiStart == 0) {
        getWifi();
        isWifiStart = 1;
        digitalWrite(green, HIGH); // wifi started
      } else if(isWifiStart == 1) {
        postWifi();
        isWifiStart = 0;
        digitalWrite(green, LOW); // wifi stoped
      }
      flagEclairage = 0;
    }else{
      //flag = 0;
    }
  }

 
  if(isPerformanceStart == 1) {
    datePerformance = "\"datePerformance\" : \"" +  (getCurrentDate() + getCurrentTime()) + "\"" + ",";
    startTime = "\"startTime\" : \"" + (getCurrentDate() + getCurrentTime()) + "\"" + ",";
    user_id = getUserId();
    isPerformanceStart = 2;
  }
  
  if(isPerformanceStart == 2) {
    mpu6050.update();
    if(mpu6050.getAccY() > minimalHighAccY || mpu6050.getAccY() < minimalLowAccY) {
      Serial.print("accY > 0.20: "); Serial.print(mpu6050.getAccY());
      Serial.println("==================================================\n");
      timer = millis();
   
      currentV = previousV + mpu6050.getAccY()* 9.8 * 0.05;
      currentY = previousY + currentV * 0.05;
      previousV = currentV;
      previousY = currentY;
      arraySpeeds[cpt] = currentV;
  
      Serial.print("current vitesse : ");Serial.print(mpu6050.getAccY()*9.8*0.05); Serial.print("m/s");
      Serial.print("vitesse : ");Serial.print(currentV);Serial.print("m/s\n");

      cpt += 1;
    }
  }
  delay(50);
}

void getWifi() {
  user_id = getUserId();
}

void postWifi() {
  sendToApi(datePerformance, startTime, averageSpeedJson, distanceJson, endTime);
}

void connection() {
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
}

double averageArray(double *array, int arraySize) {
  double sum = 0;
  for (int i = 0; i < arraySize; i++) {
    sum += array[i];
  }
  return ((double) sum) / arraySize;
}

void sendToApi(String datePerformance, String startTime, String averageSpeed, String distance, String endTime) {
    HTTPClient http;

    //Change
    http.begin("http://192.168.1.32:3000/performances/"+user_id);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-access-deviceid", device_id);
    String json = "{\"performance\" : { \"deviceId\" : \""+ device_id + "\"" + "," + "\n" + datePerformance + "\n" + startTime + "\n" + averageSpeed + "\n" + distance + "\n" + endTime + "\n"+ "}\n}";

    Serial.print("json : ");Serial.print(json);

    int httpCode = http.POST(json);

    Serial.print("json : ");Serial.print(json);

    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
    delay(1000);
}


String getUserId() {
   // StaticJsonDocument<600> doc;
    HTTPClient http;
    
    //Change
    http.begin("http://192.168.1.32:3000/users/current/deviceId");
    http.addHeader("x-access-deviceid", device_id);

    int httpCode = http.GET();

    Serial.print("http code : ");Serial.print(httpCode);

    String payload = http.getString();

    Serial.print("http code : ");  Serial.print(httpCode);
    Serial.print(payload);

    http.end();
    delay(1000);
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(error.c_str());
      return "error json parsed";
    }
    String userId = doc["data"]["user"]["_id"];
    Serial.print("user_id : ");  Serial.print(userId);
    //user_token = doc["data"]["token"];
    //Serial.print("token : ");  Serial.print(user_token);
    return userId;
}



String getCurrentDate() {
  struct tm* localTime = getLocalDateTime();
  String zero = "0";
  String date;

  if(localTime->tm_mon < 10 && localTime->tm_mday < 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + zero + "" + ((String)(localTime->tm_mon + 1)) + "-" + zero + "" + ((String)localTime->tm_mday) + "T");
    return date;
  }
  if(localTime->tm_mon < 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + zero + "" + ((String)(localTime->tm_mon + 1)) + "-" + ((String)localTime->tm_mday) + "T");
    return date;
  }
  if(localTime->tm_mday < 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + ((String)(localTime->tm_mon + 1)) + "-" + zero + "" + ((String)localTime->tm_mday) + "T");
    return date;
  } 
  
  if(localTime->tm_mon > 10 && localTime->tm_mday > 10) {
    date = ((String)(localTime->tm_year + 1900) + "-" + ((String)(localTime->tm_mon + 1)) + "-" + ((String)localTime->tm_mday) + "T");
    return date;
  }
}

String getCurrentTime() {
  struct tm* localTime = getLocalDateTime();
  String zero = "0";
  String tim3;

  if(localTime->tm_hour < 10 && localTime->tm_min < 10 && localTime->tm_sec < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
      return tim3;
  }
  if(localTime->tm_hour < 10 && localTime->tm_min < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
      return tim3;
  }
  if(localTime->tm_hour < 10 && localTime->tm_sec < 10) {
      tim3 = zero +"" +((String)localTime->tm_hour) + ":" +((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
      return tim3;
  }
  if(localTime->tm_min < 10 && localTime->tm_sec < 10) {
     tim3 = ((String)localTime->tm_hour) + ":" + zero + "" + ((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
     return tim3;
  }
  if(localTime->tm_sec < 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + zero + "" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  } 
  if (localTime->tm_min < 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + zero + "" +((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  } 
  if (localTime->tm_hour < 10) {
    tim3 = zero + "" + ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  } 
  if(localTime->tm_sec > 10 && localTime->tm_min > 10 && localTime->tm_hour > 10) {
    tim3 = ((String)localTime->tm_hour) + ":" + ((String)localTime->tm_min) + ":" + ((String)localTime->tm_sec) + ".960Z";
    return tim3;
  }

  Serial.print("time : ");Serial.print(tim3);
  Serial.println();
}

struct tm* getLocalDateTime() {
  time_t currentTime;
  struct tm *localTime;
  time( &currentTime );
  currentTime -= 2*30*60;
  localTime= localtime(&currentTime);

  return localTime;
}
