/**
 * Automatic plant watering.
 * 
 * 1. periodically reading a soil moisture sensor
 * 2. 
 * 
 * 
 * 
 * 
 */
//http://playground.arduino.cc/uploads/Code/FSM.zip
#include <FiniteStateMachine.h>

//http://playground.arduino.cc/uploads/Code/Button.zip
#include <Button.h>

//http://playground.arduino.cc/uploads/Code/LED.zip
#include <LED.h>

const byte NUMBER_OF_STATES = 4; //how many states are we cycling through?

//utility functions
LED led = LED(D4);                 //initialize the LED
void ledOn(){ led.on(); }
void ledOff(){ led.off(); }
void ledFadeIn(){ led.fadeIn(500); }
void ledFadeOut(){ led.fadeOut(500); }
//end utility functions
 
//initialize states
State On = State(ledOn);
State Off = State(ledOff);
State FadeIn = State(ledFadeIn);
State FadeOut = State(ledFadeOut);
 
FSM ledStateMachine = FSM(On);     //initialize state machine, start in state: On
 
Button button = Button(D6,PULLUP); //initialize the button (wire between pin 12 and ground)

byte buttonPresses = 0;            //counter variable, holds number of button presses
 
void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(200);
  
  pinMode(A0, INPUT);

  delay(200);

  Serial.println("Soil moisture sensor");
}

int currentA0 = 0;
int oldA0 = 0;
 
void loop()
{
  if (button.uniquePress())
  {
    Serial.println("Button press");
    
    //increment buttonPresses and constrain it to [ 0, 1, 2, 3 ]
    buttonPresses = ++buttonPresses % NUMBER_OF_STATES;
    switch (buttonPresses){
      case 0: ledStateMachine.transitionTo(On); break;
      case 1: ledStateMachine.transitionTo(Off); break;
      case 2: ledStateMachine.transitionTo(On); break;
      case 3: ledStateMachine.transitionTo(Off); break;
    }

    currentA0 = analogRead( A0 );
    Serial.print("A=");
    Serial.println(currentA0);
  }
  ledStateMachine.update();
}
