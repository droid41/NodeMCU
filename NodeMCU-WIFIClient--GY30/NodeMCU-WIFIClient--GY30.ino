#include <ESP8266WiFi.h>
#include <Wire.h>
#include "config.h"
#include "SparkFunHTU21D.h"

//https://github.com/claws/BH1750
#include <BH1750.h>

/**
 * Connects to WLAN (see config.h)and periodically reads BH1750/HTU21D sensor values and transmits it to api.thethings.io.
 *  
 * BH1750 / HTU21D are connected via I2C on SDA GPIO4 (D2) / SCL GPIO5 (D1).
 * Status LEDs: green GPIO14 (D5), red GPIO12 (D6).
 */

const int WAIT_TIME = 5000;

int sdaPin = 4;
int sclPin = 5;

int ledGreenPin = 14;
int ledRedPin = 12;

char host[] = "api.thethings.io";
int httpPort = 80;

BH1750 lightMeter;
HTU21D myHumidity;
 
void setup() {

  Serial.begin(115200);
  delay(10);

  Serial.println("Starting I2C");

  // I2C SDA 4, SCL 5
  Wire.begin(sdaPin,sclPin);

  Serial.println("Starting BH1750");
  lightMeter.begin();

  Serial.println("Starting HTU21D");

  // HTU21D init
  myHumidity.begin();

  configLeds();
 
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  redOn();
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    redOff();
    Serial.print(".");
    delay(100);
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

  delay(1000);
  greenOff();
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
  
  uint16_t lux = lightMeter.readLightLevel();
  Serial.print(lux); 
  Serial.println(" lx"); 

  Serial.print("connecting to ");
  Serial.println(host);

  greenOn();

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  char str[50];
  sprintf(str, "%d", (int) lux);
  String lxString = str;
  
  String uri = "/v2/things/" + apiKey;
  String data = "{ \"values\": [ { \"key\": \"lxval\", \"value\": " + lxString + " },"
    + "{ \"key\": \"humval\", \"value\": " + humd + " },"
    + "{ \"key\": \"tempval\", \"value\": " + temp + " }"
    + " ] }";

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

  greenOff();

  // 5 Sek. for test
  delay(WAIT_TIME);
}

void configLeds()
{
  Serial.println("Starting LEDs");

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


