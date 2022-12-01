#ifndef FILTER_MOD_H
#define FILTER_MOD_H

#include <JuceHeader.h> // for defining juce classes variables
#include "OscSwitch.h"  // oscillator class for LFOs

/// Filter class with two build-in LFOs for cutoff and resonance modulation.
/// Filter type can be set by passing a pointer to a coefficients function using
/// setFilterCoefficientsFunction() class method thus the class itself isn't
/// dependent on a particular filter type.
class FilterMod
{
public:
    // constructor that resets filter instance
    FilterMod()
    {
        filter.reset();
    }

    /// set sample rate
    /// @param float, sample rate
    void setSampleRate (float _sampleRate)
    {
        jassert (_sampleRate > 0.0f); // check sample rate value
        sampleRate = _sampleRate;
        lfoCutoff.setSampleRate (_sampleRate);
        lfoRes.setSampleRate (_sampleRate);
    }

    /// set cutoff LFO waveshape
    /// @param int, waveshape id (0 - sine, 1 - triangle, 2 - saw, 3 - square)
    void setCutoffLFOWaveshape (int _waveshapeId)
    {
        lfoCutoff.setWaveshape (_waveshapeId);
    }

    /// set cutoff LFO frequency
    /// @param float, frequency value in Hz
    void setCutoffLFOFrequency (float _frequency)
    {
        lfoCutoff.setFrequency (_frequency);
    }

    /// set cutoff LFO amplitude
    /// @param float, amplitude
    void setCutoffLFOAmplitude (float _amplitude)
    {
        lfoCutoff.setAmplitude (_amplitude);
    }

    /// set cutoff value
    /// @param float, cutoff value in Hz
    void setCutoff (float _cutoff)
    {
        lfoCutoff.setDC (_cutoff);
    }

    /// set cutoff LFO power
    /// @param, power
    void setCutoffLFOPower (float _power)
    {
        lfoCutoff.setPower (_power);
    }

    /// set resonance LFO waveshape
    /// @param int, waveshape id (0 - sine, 1 - triangle, 2 - saw, 3 - square)
    void setResLFOWaveshape (int _waveshapeId)
    {
        lfoRes.setWaveshape (_waveshapeId);
    }

    /// set resonance LFO frequency
    /// @param float, frequency
    void setResLFOFrequency (float _frequency)
    {
        lfoRes.setFrequency (_frequency);
    }

    /// set resonance LFO amplitude
    /// @param float, amplitude
    void setResLFOAmplitude (float _amplitude)
    {
        lfoRes.setAmplitude (_amplitude);
    }

    /// set resonance LFO power
    /// @param float, power
    void setResLFOPower (float _power)
    {
        lfoRes.setPower (_power);
    }

    /// set resonance value
    /// @param float, resonance
    void setRes (float _res)
    {
        lfoRes.setDC (_res);
    }

    /// set filter coefficients function
    /// @param juce::IIRCoefficients (*_func) (double, double, double), pointer to a function,
    ///        which calculates filter coefficients by using sample rate, cutoff and resonance values
    void setFilterCoefficientsFunction (juce::IIRCoefficients (*_func) (double, double, double))
    {
        makeFilterCoefficients = _func;
    }

    /// process input sample
    /// @param float, input sample
    /// @return float, filter output
    float process (float _inSample)
    {
        jassert (sampleRate > 0.0f); // check if sample rate is set (the default value on initialization is 0)
        float lfoCutoffOut = lfoCutoff.process();
        float lfoResOut = lfoRes.process();
        filter.setCoefficients (makeFilterCoefficients (sampleRate, lfoCutoffOut, lfoResOut));
        return filter.processSingleSampleRaw (_inSample);
    }

private:
    float sampleRate = 0.0f;                                                                         // sample rate [Hz]
    juce::IIRFilter filter;                                                                          // filter instance
    juce::IIRCoefficients (*makeFilterCoefficients) (double sampleRate, double frequency, double Q); // pointer to a function with calculates filter coefficiens using specified sample rate, cutoff frequency and resonance
    OscSwitch lfoCutoff;                                                                             // LFO for cutoff modulation
    OscSwitch lfoRes;                                                                                // LFO for resonance modulation
};

#endif // FILTER_MOD_H