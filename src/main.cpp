#include <Arduino.h>
#include <CapacitiveSensor.h>
#include <note.cpp>

CapacitiveSensor sensor = CapacitiveSensor(8, 9);
void controlChange(uint8_t channel, uint8_t controlVal, uint8_t value);
void SelectChannel(uint8_t &channel);

void setupNotes(int octave);
const int NOTE_COUNT = 16;
Note notes[NOTE_COUNT];
void setup()
{
  Serial.begin(115200);
  //setup multiplexer output pins
  for (size_t i = 2; i <= 6; i++)
  {
    pinMode(i, OUTPUT);
  }
  setupNotes(4);
}

uint8_t channel = 0;
//a control change of the greatest velocity is sent.
int16_t greatestVel = 0;
int input;

void loop()
{
  greatestVel = -1;
  for (channel = 0; channel < NOTE_COUNT; channel++)
  {
    SelectChannel(channel);
    input = sensor.capacitiveSensor(3);
    if (input > 40)
    {
      notes[channel].noteOn(0, constrain(input - 40, 0, 255));
      //track greatest velocity
      if (notes[channel].velocityVal > greatestVel)
      {
        greatestVel = notes[channel].velocityVal;
      }
    }
    else
    {
      notes[channel].noteOff(0, 0);
    }

    Serial.print(input);
    Serial.print(" ");
  }
  if (greatestVel > 0)
  {
    controlChange(0, 0, (uint8_t)(greatestVel * 0.3921));
  }
  Serial.println();
  MidiUSB.flush();
}

void SelectChannel(uint8_t &channel)
{
  digitalWrite(2, bitRead(channel, 3));
  digitalWrite(3, bitRead(channel, 2));
  digitalWrite(4, bitRead(channel, 1));
  digitalWrite(5, bitRead(channel, 0));
  digitalWrite(6, bitRead(channel, 4));
}

void setupNotes(int octave)
{
  int start = octave * 12 + 9;
  for (size_t i = 0; i < NOTE_COUNT; i++)
  {
    notes[i].note = NOTE_COUNT - 1 - i + start;
  }
}

void controlChange(uint8_t channel, uint8_t controlVal, uint8_t value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, controlVal, value};
  MidiUSB.sendMIDI(event);
}