#include <ESP8266WiFi.h>
#include <Wire.h>
#include "config.h"

int ledPin = 3; // GPIO0
int sdaPin = 4;
int sclPin = 5;

int BH1750_address = 0x23; // i2c Addresse
byte buff[2];

WiFiServer server(80);
 
void setup() {

  Serial.begin(115200);
  delay(10);

  // I2C SDA 4, SCL 5
  Wire.begin(sdaPin,sclPin);

  // GY-30 Modul initialisieren
  BH1750_Init(BH1750_address);

  // On-Board-LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }


  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);


  float lx = BH1750_ReadLX(BH1750_address);
  if(lx < 0) {
    Serial.print("> 65535");
  }
  else {
    Serial.print((int)lx,DEC); 
  }
  Serial.println(" lx"); 
    
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
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
 
