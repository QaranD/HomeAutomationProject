 /* Home Automation Project
  This code is developed in order to measure indoor parameters such as
  temperature, humdity, barometric pressure, sound decibel and smoke,
  and send them to www.thingspeak.com acount using DHT11, , BMP180, Arduino Microphone, MQ2 and ESP8266 modules
  ESP8266 Wifi modules hooked up on pins 5(rx) and 6(tx).

  created  11 Nov 2020
  by Qaran Dorosti
*/
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>

#define Dht_Pin 3     // DHT module(temperature and humidity) signal pin should be counected to this pin number
#define Dht_Type DHT11 // DTH11
DHT dht(Dht_Pin, Dht_Type);
int Sound_Sensor_Pin = A0; //A0 (analog pin) is used for sound sensor
int Sound_Sensor_Thresould = 270; //threshold value for sound sensor
int Smoke_Sensor_Pin = A1; //A1 (analog pin) is used for Smoke sensor (MQ2)
int Smoke_Sensor_Thresould = 400;// threshold value for smoke sensor
// Connect SCL to i2c clock - on 'Arduino Uno thats Analog 5
// Connect SDA to i2c data - on Arduino Uno thats Analog 4
Adafruit_BMP085 Barometric_Pressure;//barometric pressure sensor
const char *ssid = "your_ssid";//Your WiFi router SSID
const char *password = "your_wifi_password"; //Your WiFi password
int txPin = 6; //connect your ESP6288 rx pin to pin #6 of arduino
int rxPin = 5; //connect your ESP6288 tx pin to pin #5 of arduino
String Wifi_ssid = "name"; // "SSID-WiFi name"
String Wifi_Pass = "password"; // your ssid password
SoftwareSerial ESP_Wifi(rxPin, txPin); // serial communication pins.
String ip = "184.106.153.149"; //Thingspeak ip adresi

void setup() {

  dht.begin();
  pinMode(A0, INPUT);
  ESP_Wifi.begin(115200);// Serial communication with ESP8266.
  Serial.begin(115200);
  ESP_Wifi.println("AT");
  Serial.println("AT  sent ");
  while (!ESP_Wifi.find("OK"))  //waiting for module availability
    ESP_Wifi.println("AT");
    Serial.println("ESP8266 Not Find.");
    Serial.println("OK Command Received");
    ESP_Wifi.println("AT+CWMODE=1");  // ESP8266 module as a client.
    while (!ESP_Wifi.find("OK")) {  // waiting untile the setting is done.
      ESP_Wifi.println("AT+CWMODE=1");
      Serial.println("Setting is ....");
    }
    Serial.println("Set as client");
    Serial.println("Network connection ...");
    ESP_Wifi.println("AT+CWJAP=\"" + Wifi_ssid + "\",\"" + Wifi_Pass + "\""); // connect to defined ssid.
    while (!ESP_Wifi.find("OK"));  // wait for connection to the network.
    Serial.println("Connected");
    delay(1000);

  
}

void loop() {
  ESP_Wifi.println("AT+CIPSTART=\"TCP\",\"" + ip + "\",80"); // connecting to Thingspeak website
  if (ESP_Wifi.find("Error")) { // check the connection.
    Serial.println("AT+CIPSTART Error");
  }
  delay(2000);
  // Read humidity as percent
  float Humidity = dht.readHumidity();
  // Read temperature as Celsius
  float Temperature = dht.readTemperature();
  int Sound_Sensor_Value = 0;//sound analog read
  for (int i = 1; i <= 100; i ++)
  {
    Sound_Sensor_Value = Sound_Sensor_Value + analogRead(Sound_Sensor_Pin);
  }
  Sound_Sensor_Value = Sound_Sensor_Value / 100;
  int Smoke_Sensor_Value = 0;//smoke analog read
  for (int i = 1; i <= 100; i ++)
  {
    Smoke_Sensor_Value = Sound_Sensor_Value + analogRead(Smoke_Sensor_Pin);
  }
  Smoke_Sensor_Value = Smoke_Sensor_Value / 100;
  float Baro_Press_Val = Barometric_Pressure.readPressure() / 100.0; //mbar

  String link_Adress = "GET https://api.thingspeak.com/update?api_key=RLQNXL43HUVUSP5R";   // use your api key after "api_key=".
  link_Adress += "&field1=";
  link_Adress += String(Temperature);// The temperature variable we will send
  link_Adress += "&field2=";
  link_Adress += String(Humidity);// The moisture variable we will send
  link_Adress += "&field3=";
  link_Adress += String(Baro_Press_Val);// The moisture variable we will send
  link_Adress += "&field4=";
  link_Adress += String(Sound_Sensor_Value);// The moisture variable we will send
  link_Adress += "&field5=";
  link_Adress += String(Smoke_Sensor_Value);// The moisture variable we will send
  link_Adress += "\r\n\r\n";
  ESP_Wifi.print("AT+CIPSEND=");
  ESP_Wifi.println(link_Adress.length() + 2);
  delay(3000);
  if (ESP_Wifi.find(">")) {  // ESP8266 is ready..
    ESP_Wifi.print(link_Adress); // sending the data.
    Serial.println(link_Adress);
    Serial.println("Data sent.");
    delay(1000);
  }
  Serial.println("Connection Closed.");
  ESP_Wifi.println("AT+CIPCLOSE"); // closing the link
  delay(20000);
}
