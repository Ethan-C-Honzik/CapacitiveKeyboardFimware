#include <Arduino.h>
#include <CapacitiveSensor.h>
#include <note.cpp>

CapacitiveSensor sensorLow = CapacitiveSensor(10, 14);
CapacitiveSensor sensorHigh = CapacitiveSensor(8, 9);
void controlChange(const uint8_t channel, const uint8_t controlVal, const uint8_t value);
void SelectChannel(const uint8_t channel);

void setupNotes(int octave);
const int NOTE_COUNT = 24;
Note notes[NOTE_COUNT];
void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  // setup multiplexer output pins
  for (size_t i = 2; i <= 5; i++)
  {
    pinMode(i, OUTPUT);
  }
  setupNotes(4);
}

// a control change of the greatest velocity is sent.
int16_t greatestVel = 0;
long input;

void noteLogic(int input, Note &note)
{
  if (input > note.baseVal)
  {
    note.noteOn(0, constrain((input - note.baseVal) * 6, 0, 255));
    // track greatest velocity
    if (note.velocityVal > greatestVel)
    {
      greatestVel = note.velocityVal;
    }
  }
  else
  {
    note.noteOff(0, 0);
  }
  Serial.print(note.baseVal);
  Serial.print(" ");
  Serial.print(input);
  Serial.print(" ");
}

void loop()
{
  if(notes[0].calibrationSamples > notes[0].CALIBRATION_STEPS){
    digitalWrite(LED_BUILTIN, 1);
  }
  greatestVel = -1;
  for (uint8_t channel = 0; channel < 16; channel++)
  {
    SelectChannel(channel);
    if (channel < 8)
    {
      input = sensorLow.capacitiveSensor(2);
      // we have an offset of zero because these are the lower notes
      notes[channel + 16].calibrate(input);
      noteLogic(input, notes[channel + 16]);
    }
    input = sensorHigh.capacitiveSensor(2);
    notes[channel].calibrate(input);
    // we have an offset of 8 because these notes are 8 higher than the start
    noteLogic(input, notes[channel]);
  }
  if (greatestVel > 0)
  {
    controlChange(0, 0, (uint8_t)(greatestVel * 0.3921));
  }
  Serial.println();
  MidiUSB.flush();
}

void SelectChannel(const uint8_t channel)
{
  digitalWrite(2, bitRead(channel, 3));
  digitalWrite(3, bitRead(channel, 2));
  digitalWrite(4, bitRead(channel, 1));
  digitalWrite(5, bitRead(channel, 0));
}

void setupNotes(int octave)
{
  int start = octave * 12 + 9;
  for (size_t i = 0; i < NOTE_COUNT; i++)
  {
    notes[i].note = NOTE_COUNT - 1 - i + start;
  }
}

void controlChange(const uint8_t channel, const uint8_t controlVal, const uint8_t value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, controlVal, value};
  MidiUSB.sendMIDI(event);
}