#ifndef OPERATOR_H
#define OPERATOR_H

#include <JuceHeader.h>
#include "OscSwitch.h"

class Operator
{
public:
    float process()
    {
        float oscSample = osc.process();
        float envVal = env.getNextSample();
        return envVal * oscSample;
    }

    /// set sample rate for oscillator
    /// @param float, sample rate in Hz
    void setSampleRate (float _sampleRate)
    {
        osc.setSampleRate (_sampleRate);
    }

    /// set oscillator waveshape
    /// @param int, waveshape id (0 - sine, 1 - triangle, 2 - saw, 3 - square)
    void setOscWaveshape (int _carOscWaveshapeId)
    {
        osc.setWaveshape (_carOscWaveshapeId);
    }

    /// set carrier oscillator frequency
    /// @param float, frequency
    void setOscFrequency (float _frequency)
    {
        osc.setFrequency (_frequency);
    }

    /// set phase offset (useful for phase modulation)
    /// @param float, phase offset
    void setOscPhaseOffset (float _phaseOffset)
    {
        osc.setPhaseOffset (_phaseOffset);
    }

    /// set attack for envelope
    /// @param float, attack
    void setEnvParameters (float _attack, float _decay, float _sustain, float _release)
    {
        juce::ADSR::Parameters envParam(_attack, _decay, _sustain, _release);
        env.setParameters (envParam);
    }

    /// start the attack phase of the envelope
    void noteOn()
    {
        env.noteOn();
    }

    /// start the release phase of the envelope
    void noteOff()
    {
        env.noteOff();
    }
    
    /// reset the envelope to an idle state
    void resetEnv()
    {
        env.reset();
    }

    /// check if envelope is active
    /// @return bool, if the envelope is in its attack, decay, sustain or release stage
    bool isEnvActive()
    {
        return env.isActive();
    }
private:
    OscSwitch osc;
    juce::ADSR env;
};

#endif // OPERATOR_H