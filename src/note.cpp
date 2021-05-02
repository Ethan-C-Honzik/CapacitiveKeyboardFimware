#include <frequencyToNote.h>
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

class Note
{
public:
    uint8_t note = 0;
    bool noteActive = false;

    //the live velocity value. will be different from inital note on.
    uint8_t velocityVal = 0;

    //used to track how many cycles the note has been on for.
    uint8_t OnCycles = 0;

    void noteOn(uint8_t channel, uint8_t velocity)
    {
        OnCycles++;
        velocityVal = velocity;
        if (!noteActive && OnCycles > (uint8_t)3)
        {
            noteActive = true;
            midiEventPacket_t noteOn = {0x09, 0x90 | channel, note, velocity};
            MidiUSB.sendMIDI(noteOn);
        }
    }

    void noteOff(uint8_t channel, uint8_t velocity)
    {
        OnCycles = 0;
        velocityVal = velocity;
        if (noteActive)
        {
            noteActive = false;
            midiEventPacket_t noteOff = {0x08, 0x80 | channel, note, velocity};
            MidiUSB.sendMIDI(noteOff);
        }
    }
};
