#ifndef FILTER_MOD_H
#define FILTER_MOD_H

#include <JuceHeader.h> // for defining juce classes variables
#include "Parameters.h"

/// Filter class with two build-in LFOs for cutoff and resonance modulation.
/// Filter type can be set by passing a pointer to a coefficients function using
/// setFilterCoefficientsFunction() class method thus the class itself isn't
/// dependent on a particular filter type.
class Filter
{
public:
    /// constructor that resets filter instance
    Filter(juce::NormalisableRange<float> frequencyRange, juce::NormalisableRange<float> resonanceRange)
        : minFrequency(frequencyRange.start), maxFrequency(frequencyRange.end), minResonance(resonanceRange.start), maxResonance(resonanceRange.end)
    {
        filter.reset();
        frequencyMaxOffset = 0.5 * (maxFrequency - minFrequency); // max frequency offset for envelope and LFO
        resonanceMaxOffset = 0.5 * (maxResonance - minResonance); // max resonance offset for LFO
    }

    /// process input sample
    /// @param float, input sample
    /// @return float, filter output
    float process (float _inSample)
    {
        jassert (sampleRate > 0.0f); // check if sample rate is set (the default value on initialization is 0)
        float envVal = env.getNextSample(); // SHOULD BE LOGARITHMIC CHANGE!!!!!!!!!!!!!!!!!!!!!!!!!
        // calculate frequency with modulations
        float freq = frequency + envAmount * envVal * frequencyMaxOffset + frequencyOffset;
        // check bounds
        if (freq > maxFrequency)
            freq = maxFrequency;
        if (freq < minFrequency)
            freq = minFrequency;
        // calculate resonance with modulations
        float res = resonance + resonanceOffset;
        // check resonance bounds
        if (res < minResonance)
            res = minResonance;
        if (res > maxResonance)
            res = maxResonance;
        filter.setCoefficients (makeFilterCoefficients (sampleRate, freq, res));
        // reset modulations
        resetModulations();
        return filter.processSingleSampleRaw (_inSample);
    }

    /// set sample rate
    /// @param float, sample rate
    void setSampleRate (float _sampleRate)
    {
        jassert (_sampleRate > 0.0f); // check sample rate value
        sampleRate = _sampleRate;
    }

    /// set filter frequency
    /// @param float, frequency
    void setFrequency (float _frequency)
    {
        frequency = _frequency;
    }

    /// set filter resonance
    /// @param float, frequency
    void setResonance (float _resonance)
    {
        resonance = _resonance;
    }

    /// set filter type
    /// @param int, filter type (0 - low pass, 1 - high pass, 2 - band pass, 3 - notch
    void setType (int _filterType)
    {
        switch (_filterType)
        {
        case 0:
            setFilterCoefficientsFunction (&(juce::IIRCoefficients::makeLowPass));
            break;
        case 1:
            setFilterCoefficientsFunction (&(juce::IIRCoefficients::makeHighPass));
            break;
        case 2:
            setFilterCoefficientsFunction (&(juce::IIRCoefficients::makeBandPass));
            break;
        case 3:
            setFilterCoefficientsFunction (&(juce::IIRCoefficients::makeNotchFilter));
            break;
        }
    }

    /// set attack for envelope
    /// @param float, attack
    void setEnvParameters (float _attack, float _decay, float _sustain, float _release)
    {
        juce::ADSR::Parameters envParam(_attack, _decay, _sustain, _release);
        env.setParameters (envParam);
    }

    /// set envelope amount
    /// @param float, envelope amound (-100% to 100%)
    void setEnvAmount (float _envAmount)
    {
        envAmount = _envAmount;
    }

    /// start the attack phase of the envelope and update parameters
    void startNote (Parameters* param, float _sampleRate)
    {
        filter.reset();
        env.reset();

        (*this).setSampleRate (_sampleRate);
        (*this).setType (*param->filterTypeParam);
        (*this).setFrequency (*param->filterFrequencyParam);
        (*this).setResonance (*param->filterResonanceParam);
        (*this).setEnvParameters (*param->filterAttackParam, *param->filterDecayParam, *param->filterSustainParam, *param->filterReleaseParam);
        (*this).setEnvAmount (*param->filterEnvAmountParam);

        env.noteOn();
    }

    /// start the release phase of the envelope
    void stopNote()
    {
        env.noteOff();
    }

    /// set frequency offset
    /// @param float, frequency offset amount (from -1 to 1)
    void setFrequencyOffset (float _frequencyOffsetAmount)
    {
        frequencyOffset += _frequencyOffsetAmount * frequencyMaxOffset;
    }

    /// set resonance offset
    /// @param float, resonance offset amount (from -1 to 1)
    void setResonanceOffset (float _resonanceOffsetAmount)
    {
        resonanceOffset += _resonanceOffsetAmount * resonanceMaxOffset;
    }
private:
    float sampleRate = 0.0f;                                                                         // sample rate [Hz]
    // base members
    juce::IIRFilter filter;                                                                          // filter instance
    juce::IIRCoefficients (*makeFilterCoefficients) (double sampleRate, double frequency, double Q); // pointer to a function with calculates filter coefficiens using specified sample rate, cutoff frequency and resonance
    juce::ADSR env;                                                                                  // envelope
    // filter parameters
    float frequency;
    float resonance;
    float envAmount = 0.0f;
    // modulation parameters
    float frequencyOffset = 0.0f;
    float resonanceOffset = 0.0f;
    // parameters bounds
    const float minFrequency;
    const float maxFrequency;
    const float minResonance;
    const float maxResonance;
    float frequencyMaxOffset;
    float resonanceMaxOffset;

    /// set filter coefficients function
    /// @param juce::IIRCoefficients (*_func) (double, double, double), pointer to a function,
    ///        which calculates filter coefficients by using sample rate, cutoff and resonance values
    void setFilterCoefficientsFunction (juce::IIRCoefficients (*_func) (double, double, double))
    {
        makeFilterCoefficients = _func;
    }

    /// reset external filter modulations (frequency and resonance modulations)
    void resetModulations()
    {
        frequencyOffset = 0.0f;
        resonanceOffset = 0.0f;
    }
};

#endif // FILTER_MOD_H