/**
 * Automatic plant watering.
 * 
 * States
 * metering -> watering (10sec) -> sleep (20 sec) -> metering
 * 
 * 1. State "metering": periodically reading a soil moisture sensor (A0)
 * 2. transfer analog value to server
 * 3. check threshold, transition to State "watering" for 10 seconds
 * 4. switch pumps off, transition to "sleep state" for 10 seconds
 * 5. transition to "metering state"
 * 
 * TBD
 * - Switch motor instead of LED.
 * - connect to Gateway for sending analog value
 */
//http://playground.arduino.cc/uploads/Code/FSM.zip
#include <FiniteStateMachine.h>

//http://playground.arduino.cc/uploads/Code/Button.zip
#include <Button.h>

//http://playground.arduino.cc/uploads/Code/LED.zip
#include <LED.h>

int MOISTURE_DIGITAL_IN = D5;
int MOISTURE_ANALOG_INPUT = A0;
int BUTTON_INPUT = D6;
int PUMP_OUTPUT = D7;

Button button = Button(BUTTON_INPUT,PULLUP); //initialize the button (wire between pin 12 and ground)

//utility functions
LED led = LED(D4);                 //initialize the LED
void ledOn(){ led.on(); }
void ledOff(){ led.off(); }

//initialize states
State Metering = State(metering, meteringUpdate, NULL);
State Watering = State(watering, wateringUpdate, wateringExit);
State Sleeping = State(sleeping, sleepingUpdate, NULL);

//initialize state machine
FSM stateMachine = FSM(Sleeping);

int wateringEndMillis = 0;
int sleepingEndMillis = 0;

void watering()
{
  Serial.println("Watering, starting pump, setting time to 10 seconds");
  pumpOn();
  wateringEndMillis = millis() + 10 * 1000;
}

void wateringUpdate()
{
  if (millis() > wateringEndMillis)
  {
    stateMachine.transitionTo(Sleeping);
  }
}

void wateringExit()
{
  Serial.println("Watering exit, stopping pump");
  pumpOff();
  wateringEndMillis = 0;
}

void sleeping()
{
  Serial.println("Sleeping");
  sleepingEndMillis = millis() + 10 * 1000;
}

void sleepingUpdate()
{
  if (millis() > sleepingEndMillis)
  {
    stateMachine.transitionTo(Metering);
  }
}

void metering()
{
  Serial.println("Metering");
}

void meteringUpdate()
{
  if (button.uniquePress() || digitalRead(MOISTURE_DIGITAL_IN) == HIGH)
  {
    stateMachine.transitionTo(Watering);
  }
}

//end utility functions

void pumpOn()
{
  digitalWrite(PUMP_OUTPUT, HIGH);
}

void pumpOff()
{
  digitalWrite(PUMP_OUTPUT, LOW);
}


void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(200);

  // analog input moisture
  pinMode(MOISTURE_ANALOG_INPUT, INPUT);
  // digital input moisture
  pinMode(MOISTURE_DIGITAL_IN, PULLDOWN);
  
  // digital output pump
  pinMode(PUMP_OUTPUT, OUTPUT);
  digitalWrite(PUMP_OUTPUT, LOW);

  delay(200);

  Serial.println("Soil moisture sensor");

  connect();
}

void loop()
{
  stateMachine.update();
  delay(10);
  meterAnalogAndWrite();
}


int nextMeasurement = 0;
void meterAnalogAndWrite()
{
  int nowMillis = millis();
  if (nowMillis > nextMeasurement)
  {
    int currentA0 = analogRead( MOISTURE_ANALOG_INPUT );
    Serial.print("A=");
    Serial.println(currentA0);

    write(currentA0);

    // next in 60 secs
    nextMeasurement = nowMillis + 60 * 1000;
  }
}

