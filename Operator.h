#ifndef OPERATOR_H
#define OPERATOR_H

#include <JuceHeader.h>
#include "OscSwitch.h"
#include "Parameters.h"

class Operator
{
public:
    float process()
    {
        float oscSample = osc.process();
        float envVal = env.getNextSample();
        resetModulations();
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
    void setOscWaveshape (int _oscWaveshapeId)
    {
        osc.setWaveshape (_oscWaveshapeId);
    }

    /// set oscillator frequency
    /// @param float, frequency
    void setOscFrequency (float _frequency)
    {
        osc.setFrequency (_frequency);
    }

    /// set phase offset (useful for phase modulation)
    /// @param float, phase offset
    void setOscPhaseOffset (float _phaseOffset)
    {
        phaseOffset += _phaseOffset;
        osc.setPhaseOffset (phaseOffset);
    }

    /// set oscillator amplitude
    /// @param float, amplitude
    void setOscAmplitude (float _amplitude)
    {
        osc.setAmplitude (_amplitude);
    }

    /// set oscillator amplitude offset (useful for amplitude modulation)
    /// @param float, amplitude
    void setOscAmplitudeOffset (float _amplitudeOffset)
    {
        amplitudeOffset += _amplitudeOffset;
        osc.setAmplitudeOffset (amplitudeOffset);
    }

    /// set attack for envelope
    /// @param float, attack
    void setEnvParameters (float _attack, float _decay, float _sustain, float _release)
    {
        juce::ADSR::Parameters envParam(_attack, _decay, _sustain, _release);
        env.setParameters (envParam);
    }

    /// start the attack phase of the envelope
    void startNote (Parameters* _param, int _idx, float _freq, float _sampleRate)
    {
        env.reset();
        (*this).setOscWaveshape (*_param->opWaveshapeParam[_idx]);
        (*this).setSampleRate (_sampleRate);
        (*this).setOscFrequency (_freq * (*_param->opCoarseParam[_idx] + *_param->opFineParam[_idx] / 1000.0f));
        (*this).setOscAmplitude (*_param->opLevelParam[_idx]);
        (*this).setEnvParameters (*_param->opAttackParam[_idx], *_param->opDecayParam[_idx], *_param->opSustainParam[_idx], *_param->opReleaseParam[_idx]);
        env.noteOn();
    }

    /// start the release phase of the envelope
    void stopNote()
    {
        env.noteOff();
    }

    /// check if envelope is active
    /// @return bool, if the envelope is in its attack, decay, sustain or release stage
    bool isEnvActive()
    {
        return env.isActive();
    }
private:
    // base members
    OscSwitch osc;
    juce::ADSR env;
    // modulation variables
    float amplitudeOffset = 0.0f;
    float phaseOffset = 0.0f;
    
    /// reset external modulations for operator (amplitude and phase modulation)
    void resetModulations()
    {
        amplitudeOffset = 0.0f;
        osc.setAmplitudeOffset (0.0f);
        phaseOffset = 0.0f;
        osc.setPhaseOffset (0.0f);
    }
};

#endif // OPERATOR_H