#include <Arduino.h>
#include <ThingifyEsp.h>

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
bool previousSwitch1State = false;
bool previousSwitch2State = false;
bool _wasResetPressed = false;
uint64_t _resetPressStartTime = 0;
uint64_t _lastTemperatureRead = 0;


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

	pinMode(Switch1Pin, INPUT);
    pinMode(Switch2Pin, INPUT);
    pinMode(ResetButtonPin, INPUT);

	pinMode(Relay1Pin, OUTPUT);
	pinMode(Relay2Pin, OUTPUT);

    pinMode(TemperaturePin, INPUT);

    pinMode(Ade7953IrqPin, INPUT);


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
	bool switch1State = digitalRead(Switch1Pin);
	if(switch1State != previousSwitch1State)
	{
		relay1Node->Set(!relay1Node->Value.AsBool());
	}
	previousSwitch1State = switch1State;

    bool switch2State = digitalRead(Switch2Pin);
	if(switch2State != previousSwitch2State)
	{
		relay2Node->Set(!relay2Node->Value.AsBool());
	}
	previousSwitch1State = switch1State;

    switch1Node->Set(switch1State);
    switch2Node->Set(switch2State);


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
