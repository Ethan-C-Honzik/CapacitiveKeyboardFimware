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

    const uint16_t CALIBRATION_STEPS = 1000;
    uint16_t calibrationSamples = 0;
    long baseVal = 0;

    // the live velocity value. will be different from inital note on.
    uint8_t velocityVal = 0;

    // used to track how many cycles the note has been on for.
    uint8_t OnCycles = 0;

    void calibrate(int input)
    {
        if (calibrationSamples < CALIBRATION_STEPS)
        {
            baseVal += input;
            calibrationSamples += 1;
        }else if(calibrationSamples == CALIBRATION_STEPS){
            baseVal = (int16_t)((baseVal / CALIBRATION_STEPS) * 1.35);
            calibrationSamples += 1;
        }
    }

    void noteOn(uint8_t channel, uint8_t velocity)
    {
        if (calibrationSamples < CALIBRATION_STEPS)
            return;
        OnCycles++;
        velocityVal = velocity;
        if (!noteActive && OnCycles > (uint8_t)2)
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
