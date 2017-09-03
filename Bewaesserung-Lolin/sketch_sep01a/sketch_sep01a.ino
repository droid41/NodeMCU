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

Button button = Button(D6,PULLUP); //initialize the button (wire between pin 12 and ground)

//utility functions
LED led = LED(D4);                 //initialize the LED
void ledOn(){ led.on(); }
void ledOff(){ led.off(); }

//initialize states
State Metering = State(metering, meteringUpdate, NULL);
State Watering = State(watering, wateringUpdate, wateringExit);
State Sleeping = State(sleeping, sleepingUpdate, NULL);

FSM stateMachine = FSM(Metering);     //initialize state machine, start in state: On

int wateringEndMillis = 0;
int sleepingEndMillis = 0;

void watering()
{
  Serial.println("Watering, starting pump, setting time to 10 seconds");
  ledOn();
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
  ledOff();
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
  if (button.uniquePress())
  {
    stateMachine.transitionTo(Watering);
  }
}

//end utility functions
 
void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(200);
  
  pinMode(A0, INPUT);
  pinMode(D5, PULLDOWN);

  delay(200);

  Serial.println("Soil moisture sensor");
}

int currentA0 = 0;
int oldA0 = 0;
 
void loop()
{

/*
    stateMachine.transitionTo(Watering);
    stateMachine.transitionTo(Sleeping);

    currentA0 = analogRead( A0 );
    Serial.print("A=");
    Serial.println(currentA0);
*/
  stateMachine.update();
  delay(10);
}
