#include <ESP8266WiFi.h>
#include <Wire.h>
#include "config.h"
#include "SparkFunHTU21D.h"

int sdaPin = 4;
int sclPin = 5;

int ledGreenPin = 14;
int ledRedPin = 12;

char host[] = "api.thethings.io";
int httpPort = 80;

int BH1750_address = 0x23; // i2c Addresse
byte buff[2];

//Create an instance of the object
HTU21D myHumidity;
 
void setup() {

  Serial.begin(115200);
  delay(10);

  Serial.print("Starting I2C");

  // I2C SDA 4, SCL 5
  Wire.begin(sdaPin,sclPin);

  Serial.print("Starting GY-30");

  // GY-30 Modul initialisieren
  BH1750_Init(BH1750_address);

  Serial.print("Starting HTU21D");

  // HTU21D init
  myHumidity.begin();

  configLeds();
 
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    redOn();
  }
  
  Serial.println("");
  Serial.println("WiFi connected");

  redOff();
  greenOn();
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {

  float humd = myHumidity.readHumidity();
  float temp = myHumidity.readTemperature();

  Serial.print("Time:");
  Serial.print(millis());
  Serial.print(" Temperature:");
  Serial.print(temp, 1);
  Serial.print("C");
  Serial.print(" Humidity:");
  Serial.print(humd, 1);
  Serial.print("%");

  Serial.println();
  
  float lx = BH1750_ReadLX(BH1750_address);
  if(lx < 0) {
    Serial.print("> 65535");
  }
  else {
    Serial.print((int)lx,DEC); 
  }
  Serial.println(" lx"); 

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  char str[50];
  sprintf(str, "%d", (int)lx);
  
  String lxString = str;
  String uri = "/v2/things/" + apiKey;
  String data = "{ \"values\": [ { \"key\": \"lxval\", \"value\": " + lxString + " } ] }";

  String postRequest = 
    "POST " + uri + " HTTP/1.0\r\n" +
    "Host: " + host + "\r\n" +
    "Accept: application/json\r\n" +
    "Content-Length: " + data.length() + "\r\n" +
    "Content-Type: application/json\r\n" +
    "\r\n" + data;

  client.print(postRequest);
               
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");


  delay(60000);
}

void BH1750_Init(int address){
  
  Wire.beginTransmission(address);
  Wire.write(0x10); // 1 [lux] aufloesung
  Wire.endTransmission();
}

byte BH1750_Read(int address){
  
  byte i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read(); 
    i++;
  }
  Wire.endTransmission();  
  return i;
}

float BH1750_ReadLX(int address)
{
  float valf=0;
  if(BH1750_Read(address)==2) {
    valf=((buff[0]<<8)|buff[1])/1.2;
    return valf;
  }

  return 0;
}

void configLeds()
{
  Serial.print("Starting LEDs");

  pinMode(ledGreenPin, OUTPUT);
  greenOff();

  pinMode(ledRedPin, OUTPUT);
  redOff();
}

void redOn()
{
  digitalWrite(ledRedPin, HIGH);
}

void redOff()
{
  digitalWrite(ledRedPin, LOW);
}

void greenOn()
{
  digitalWrite(ledGreenPin, HIGH);
}

void greenOff()
{
  digitalWrite(ledGreenPin, LOW);
}


