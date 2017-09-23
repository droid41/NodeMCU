#include <ESP8266WiFi.h>
#include <Wire.h>
#include "config.h"

//https://github.com/claws/BH1750
#include <BH1750.h>

/**
 * Connects to WLAN (see config.h) and provides a minimalistic webserver to switch led on/off and read BH1750 sensor value.
 *  
 * BH1750 is connected via I2C on SDA GPIO4 (D2) / SCL GPIO5 (D1).
 * LED is connected on GPIO0 (D3).
 * 
 * Connect info is printed via Serial.
 */

int ledPin = 3; // GPIO0
int sdaPin = 4;
int sclPin = 5;

BH1750 lightMeter;

WiFiServer server(80);
 
void setup() {

  Serial.begin(115200);
  delay(10);

  // I2C SDA 4, SCL 5
  Wire.begin(sdaPin,sclPin);

  Serial.println("Starting BH1750");
  lightMeter.begin();

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
 
  uint16_t lux = lightMeter.readLightLevel();
  Serial.print(lux); 
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

  client.print("LightMeter lx ");
  client.println(lux);
  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

