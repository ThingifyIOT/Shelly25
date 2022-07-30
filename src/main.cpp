#include <Arduino.h>
#include <ThingifyEsp.h>
#include "Lib\DebounceButton\DebounceButton.h"
#include "temperature_sensor.h"

ThingifyEsp thing("Shelly2.5");

// pin definitions
const int Relay1Pin = 4;
const int Relay2Pin = 15;

const int Switch1Pin = 13;
const int Switch2Pin = 5;

const int ResetButtonPin = 2;
const int Ade7953IrqPin = 13;

const int TemperaturePin = A0;
const int StatusLedPin = 0;

// nodes def
Node* relay1Node, *relay2Node, *switch1Node, *switch2Node;
Node* temperatureNode;
Node* resetIndicatorNode;

// variables
bool _wasResetPressed = false;
uint64_t _resetPressStartTime = 0;
uint64_t _lastTemperatureRead = 0;

//buttons 
DebounceButton button1(Switch1Pin, INPUT);
DebounceButton button2(Switch2Pin, INPUT);

bool OnRelay1Changed(void*_, Node *node)
{
	digitalWrite(Relay1Pin, node->Value.AsBool());
  	return true;
}
bool OnRelay2Changed(void*_, Node *node)
{
	digitalWrite(Relay2Pin, node->Value.AsBool());
  	return true;
}
void setup()
{
	Serial.begin(500000);
    thing.ResetSequenceEnabled = false;
    thing.Initialize();	
	thing.AddDiagnostics();
    thing.AddStatusLed(StatusLedPin, true);

    // pin initialization
    pinMode(ResetButtonPin, INPUT);
	pinMode(Relay1Pin, OUTPUT);
	pinMode(Relay2Pin, OUTPUT);

    pinMode(TemperaturePin, INPUT);

    pinMode(Ade7953IrqPin, INPUT);

    // button intialization
    button1.Init();
    button2.Init();

	relay1Node = thing.AddBoolean("Out1")->OnChanged(OnRelay1Changed);
    relay2Node = thing.AddBoolean("Out2")->OnChanged(OnRelay2Changed);

    switch1Node = thing.AddBoolean("Switch1");
    switch2Node = thing.AddBoolean("Switch2");

    temperatureNode = thing.AddFloat("temperature", ThingifyUnit::Celsius);

	thing.Start();
}


void ReadTempratureStep()
{
    if(_lastTemperatureRead == 0 || millis() - _lastTemperatureRead > 5000)
    {
        _lastTemperatureRead = millis();
        float temperature = ReadTemperature(TemperaturePin);
        temperatureNode->Set(temperature);
    }
}
void loop()
{
    if(button1.Loop())
    {
        relay1Node->Set(!relay1Node->Value.AsBool());
    }
    if(button2.Loop())
    {
        relay2Node->Set(!relay2Node->Value.AsBool());
    }

    switch1Node->Set(button1.IsPressed());
    switch2Node->Set(button2.IsPressed());


    // detect button reset long press
    bool isResetPressed = !digitalRead(ResetButtonPin);

    if(!_wasResetPressed && isResetPressed)
    {
        _resetPressStartTime = millis();
    }

    if(!isResetPressed)
    {
        _resetPressStartTime = 0;
    }    

    if(_resetPressStartTime != 0 && millis() - _resetPressStartTime > 5000)
    {
        thing.ResetConfiguration();
    }
    _wasResetPressed = isResetPressed;
    // end detect button reset long press

    ReadTempratureStep();

	thing.Loop();
}
