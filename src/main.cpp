#include <Arduino.h>
#include <CapacitiveSensor.h>
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>
#include <note.cpp>

CapacitiveSensor sensor = CapacitiveSensor(8,9);
void setupNotes(int octave);
const int NOTE_COUNT = 16;
Note notes[NOTE_COUNT];
void setup() {
  Serial.begin(9600);       
  //setup multiplexer output pins
  for (size_t i = 2; i <= 5; i++)
  {
    pinMode(i, OUTPUT);
  }    
  setupNotes(5);
}

uint8_t channel = 0;
long input; 
void loop() {
  for (channel = 0; channel < NOTE_COUNT; channel++)
  {
    digitalWrite(2, bitRead(channel, 3));
    digitalWrite(3, bitRead(channel, 2));
    digitalWrite(4, bitRead(channel, 1));
    digitalWrite(5, bitRead(channel, 0));
    input = sensor.capacitiveSensor(20);
    if(input > 200){
      notes[channel].noteOn(0, 255);
    }else{
      notes[channel].noteOff(0, 255);
    }
    Serial.print(input);
    Serial.print(" ");    
  }
  Serial.println();    
  MidiUSB.flush();
}

void setupNotes(int octave){
  int start = octave * 12 + 9;
  for (size_t i = 0; i < NOTE_COUNT; i++)
  {
    notes[i].note = NOTE_COUNT - 1 - i + start;
  }
  
}