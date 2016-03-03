//nodeMCU v1.0 (black) with Arduino IDE
//stream temperature data DS18B20 with 1wire on ESP8266 ESP12-E (nodeMCU v1.0)
//http://shin-ajaran.blogspot.co.uk/2015/09/stream-iot-sensor-data-esp8266-nodemcu.html
//nodemcu pinout https://github.com/esp8266/Arduino/issues/584


#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#define ReportInterval 60 //in sec | Thingspeak pub is 15sec but 60 second interval is fine
#define ONE_WIRE_BUS D4  // DS18B20 on arduino pin2 corresponds to D4 on physical board
#define AlarmLED D6  // AlarmLED lights when temps go out of nominal range.
#define NormLED D8  //NormLed is light when temps are nominal.
#define FanRelay D7 //Digial pin for relay control

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float prevTemp = 0;
const char* server = "api.thingspeak.com";
String apiKey ="your API key here";
const char* MY_SSID = "your SSID here"; 
const char* MY_PWD = "your SSID password here";

//Lower bound for nominal temp
const float LowTemp = 19;

//upper bound for nominal temp 
const float HighTemp = 31;

int sent = 0;
void setup() {
  Serial.begin(115200);
  connectWifi();
}

void loop() {
  float temp;
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(temp);
  
  if (temp >= HighTemp)
  {
	  digitalWrite(FanRelay, HIGH);
  }
  else
  {
	  digitalWrite(FanRelay, LOW);
  }

  if (temp <= LowTemp || temp >= HighTemp)
  {
    digitalWrite(AlarmLED, HIGH);
    digitalWrite(NormLED, LOW);
	  digitalWrite(FanRelay, LOW);
  }
  else
  {
    digitalWrite(AlarmLED, LOW);
    digitalWrite(NormLED, HIGH);
  }
  
  sendTeperatureTS(temp);
  int count = ReportInterval;
  while(count--)
  delay(1000);
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect


void sendTeperatureTS(float temp)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send

