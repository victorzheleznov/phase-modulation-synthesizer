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
    Filter()
    {
        filter.reset();
    }

    /// process input sample
    /// @param float, input sample
    /// @return float, filter output
    float process (float _inSample)
    {
        jassert (sampleRate > 0.0f); // check if sample rate is set (the default value on initialization is 0)
        float envVal = env.getNextSample(); // SHOULD BE LOGARITHMIC CHANGE!!!!!!!!!!!!!!!!!!!!!!!!!
        filter.setCoefficients (makeFilterCoefficients (sampleRate, frequency + envAmount * envVal * frequencyShift, resonance));
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

    /// start the attack phase of the envelope
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

        if (envAmount >= 0.0f)
            frequencyShift = 0.5 * sampleRate - frequency;
        else
            frequencyShift = frequency - lowestFrequency;
    }

    /// start the release phase of the envelope
    void stopNote()
    {
        env.noteOff();
    }

private:
    float sampleRate = 0.0f;                                                                         // sample rate [Hz]
    juce::IIRFilter filter;                                                                          // filter instance
    juce::IIRCoefficients (*makeFilterCoefficients) (double sampleRate, double frequency, double Q); // pointer to a function with calculates filter coefficiens using specified sample rate, cutoff frequency and resonance
    float frequency;
    float resonance;
    juce::ADSR env;
    int frequencyShift = 9;
    float lowestFrequency = 0.1f;
    float envAmount;

    /// set filter coefficients function
    /// @param juce::IIRCoefficients (*_func) (double, double, double), pointer to a function,
    ///        which calculates filter coefficients by using sample rate, cutoff and resonance values
    void setFilterCoefficientsFunction (juce::IIRCoefficients (*_func) (double, double, double))
    {
        makeFilterCoefficients = _func;
    }
};

#endif // FILTER_MOD_H