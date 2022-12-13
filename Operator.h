#ifndef OPERATOR_H
#define OPERATOR_H

#include <JuceHeader.h> // for juce::ADSR
#include "OscSwitch.h"  // for oscillator with variable waveshape
#include "Parameters.h" // for accessing parameters set by the user interface

/// Operator class.
/// A class instance consists of an oscillator with
/// variable waveshape, an amplitude envelope and
/// a pitch envelope.
class Operator
{
public:
    /// process operator with amplitude and pitch envelopes
    /// @return float, output sample
    float process()
    {
        float envVal = env.getNextSample();
        float pitchEnvVal = pitchEnv.getNextSample();
        float freq = frequency * (1.0f + pitchEnvVal * (powf(2.0f, pitchEnvInitialLevel/12.0f) - 1.0f));
        osc.setFrequency (freq);
        float oscSample = osc.process();
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
        frequency = _frequency;
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

    /// set amplitude envelope parameters
    /// @param float, attack
    /// @param float, decay
    /// @param float, sustain
    /// @param float, release
    void setEnvParameters (float _attack, float _decay, float _sustain, float _release)
    {
        juce::ADSR::Parameters envParam(_attack, _decay, _sustain, _release);
        env.setParameters (envParam);
    }

    /// set pitch envelope parameters
    /// @param float, initial level (in semitones)
    /// @param float, decay time
    void setPitchEnvParameters (float _initialLevel, float _decay)
    {
        juce::ADSR::Parameters pitchEnvParam(0.0f, _decay, 0.0f, 0.0f);
        pitchEnv.setParameters (pitchEnvParam);
        pitchEnvInitialLevel = _initialLevel;
    }

    /// start the attack phase of amplitude and picth envelopes and update operator's parameters
    /// @param Parameters*, pointer to parameters set by the user interface
    /// @param int, operator index
    /// @param float, midi note frequency
    /// @param float, midi note velocity
    /// @param float, sample rate [Hz]
    void startNote (Parameters* _param, int _idx, float _freq, float _velocity, float _sampleRate)
    {
        env.reset();
        pitchEnv.reset();
        (*this).setOscWaveshape (*_param->opWaveshapeParam[_idx]);
        (*this).setSampleRate (_sampleRate);
        (*this).setOscFrequency (_freq * (*_param->opCoarseParam[_idx] + *_param->opFineParam[_idx] / 1000.0f));
        (*this).setOscAmplitude (*_param->opLevelParam[_idx] * _velocity);
        (*this).setEnvParameters (*_param->opAttackParam[_idx], *_param->opDecayParam[_idx], *_param->opSustainParam[_idx], *_param->opReleaseParam[_idx]);
        (*this).setPitchEnvParameters (*_param->pitchEnvInitialLevelParam, *_param->pitchEnvDecayParam);
        env.noteOn();
        if (*_param->pitchEnvOnParam == true)
            pitchEnv.noteOn();
    }

    /// start the release phase of amplitude and picth envelopes
    void stopNote()
    {
        env.noteOff();
        pitchEnv.noteOff();
    }

    /// check if amplitude envelope is active
    /// @return bool, if the envelope is in its attack, decay, sustain or release stage
    bool isEnvActive()
    {
        return env.isActive();
    }
private:
    // base members
    OscSwitch osc;                // oscillator with variable waveshape
    juce::ADSR env;               // amplitude envelope
    juce::ADSR pitchEnv;          // pitch envelope
    float frequency;              // oscillator frequency [Hz]
    int pitchEnvInitialLevel = 0; // initial level for pitch envelope [semitones]
    // modulation variables
    float amplitudeOffset = 0.0f; // amplitude offset set by an external source
    float phaseOffset = 0.0f;     // phase offset set by an external source
    
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