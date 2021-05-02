#include <frequencyToNote.h>
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

class Note
{
public:
    int note = 0;
    bool noteActive = false;

    void noteOn(byte channel, byte velocity)
    {
        if (!noteActive)
        {
            noteActive = true;
            midiEventPacket_t noteOn = {0x09, 0x90 | channel, note, velocity};
            MidiUSB.sendMIDI(noteOn);
        }
    }

    void noteOff(byte channel, byte velocity)
    {
        if (noteActive)
        {
            noteActive = false;
            midiEventPacket_t noteOff = {0x08, 0x80 | channel, note, velocity};
            MidiUSB.sendMIDI(noteOff);
        }
    }

    void controlChange(byte channel, byte value)
    {
        if (noteActive)
        {
            midiEventPacket_t event = {0x0B, 0xB0 | channel, note, value};
            MidiUSB.sendMIDI(event);
        }
    }
};
