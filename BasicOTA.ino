#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "MQ135.h"
#include <BlynkSimpleEsp8266.h>
char auth[] = "t5GWG_hZKygPN8tCL9zegjsVPv5tuA3l";
String apiKey = "MBQNXE14H4FDMN9Q";  ; //  Enter your Write API key here
const char *ssid =  "vivo 1908"  ;   // Enter your WiFi Name,,,,,7u'
const char *password =  "elsajerlyn"; // Enter your WiFi Password
const char* server = "api.thingspeak.com";
const int sensorPin= 4;
int air_quality;
#define DHTPIN 0 

DHT dht(DHTPIN, DHT11);
WiFiClient client;
// Replace with your network credentials

const int ESP_BUILTIN_LED = 2;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
    Blynk.begin(auth, ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    
    delay(5000);
    ESP.restart();
    
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(ESP_BUILTIN_LED, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();
  //float h = dht.readHumidity();
  float t = dht.readTemperature();
  MQ135 gasSensor = MQ135(A0);
     air_quality = gasSensor.getPPM();
     float RL = 10;
     float R0 = 76.63;
     float Rs,ratio,CO2_PPM;
     dht.begin();
                      if (client.connect(server,80))   
                      {  
                             String postStr = apiKey;
                            
                             postStr +="&field1=";
                             postStr += String(air_quality);
                              postStr +="&field2=";
                              postStr += String(t);
                             // postStr +="&field3=";
                             //postStr += String(h);
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             Serial.print("Air Quality: "); 
                             Serial.print(air_quality);                          
                             client.print(postStr);
                             Rs=( ( 5.0 * RL ) - ( RL * air_quality ) );
                             ratio=Rs/R0; 
                             ratio=ratio*0.3611;
                             CO2_PPM = 14.15*2.868-ratio;
                             Serial.print("CO2:");
                             Serial.print(CO2_PPM);
                             Rs=( ( 5.0 * RL ) - ( RL * air_quality ) );
                             ratio=Rs/R0; 
                             ratio=ratio*0.015 ;
                             CO = 14.15*2.868-ratio;
                             Serial.print("CO:");
                             Serial.print(CO);
                             if(air_quality<15)
                             {
                              Serial.print("Air quality is good");
                             }
                              else if(air_quality<50)
    {
      Serial.print("Health alert");
    }
     else if (air_quality<150)
    {
      Serial.print("Not suitable environment");
    }
  else{
    Serial.print("Dangerous condition");
  }
                              Serial.print("Temp: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                            // Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
              Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
          Serial.println("Waiting...");
            Blynk.run();                                        //Run the Blynk                                                                                                 //Read temperature and put it in t variable                        
  float q = analogRead(A0);
   Blynk.virtualWrite(V0, t);                          //Send t value to blynk in V5 virtual pin                                 
 // Blynk.virtualWrite(V6, h);   
    Blynk.virtualWrite(V5, q);  
     if(q<15)
    {
      Blynk.notify("Air quality is good");
    }
    else if(q<50)
    {
      Blynk.notify("Health alert");
    }
     else if (51<q && q<150)
    {
      Blynk.notify("Not suitable environment");
    }
  else{
    Blynk.notify("Dangerous condition");
  }
  if (t>40) {
    Blynk.notify("The Temperature is too high");      //Blynk will send notify if t>40
  }
  delay(1000);
}
