#include <LED.h>
#include <ESP8266WiFi.h>
#include "config.h"

LED connectLed = LED(D4);

void connect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    connectLed.on();
    Serial.print(".");
    delay(300);
    connectLed.off();
    delay(300);
  }

  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.print("NodeMCU IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void write(int value)
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String uri = "/write?db=" + db;
  
  String data =   "moisture,host=" + sensorHostname + " value=" + value + "\n";
  
  String postRequest = 
    "POST " + uri + " HTTP/1.0\r\n" +
    "Host: " + host + ":" + httpPort + "\r\n" +
    "Content-Length: " + data.length() + "\r\n" +
    "Cache-Control: no-cache\r\n" +
    "Content-Type: application/x-www-form-urlencoded\r\n" +
    "\r\n" + 
    data;

  client.print(postRequest);
               
  delay(250);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

