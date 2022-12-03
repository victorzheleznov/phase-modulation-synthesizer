#ifndef OSCILLATORS_H
#define OSCILLATORS_H

#include <cmath> // for sin(), powf()
#include <JuceHeader.h> // for jassert()

/// Base phasor class.
/// A class is used as a base for building different oscillator forms.
/// A class method process() processes phase value to output oscillator sample
/// at that time. This method accounts for phase offset, amplitude, direct current
/// and power which are specified using setter functions.
class Phasor
{
public:
    /// update the phase and output the next sample from the oscillator
    /// @return float, oscillator sample (with applied phase offset, amplitude,
    ///         direct current and power which are specified using setter functions)
    float process()
    {
        phase += phaseDelta;
        
        if (phase > 1.0f)
            phase -= 1.0f;
        
        return (amplitude + amplitudeOffset) * powf (output (phase + phaseOffset), power) + dc;
    }
    
    /// placeholder function to specify output of an oscillator
    /// @param float, phase
    /// @return float, raw oscillator sample
    virtual float output (float p)
    {
        return p;
    }
    
    /// set sample rate
    /// @param float, sample rate in Hz
    void setSampleRate (float _sampleRate)
    {
        jassert (_sampleRate > 0.0f); // check sample rate value
        sampleRate = _sampleRate;
    }
    
    /// set oscillator frequency
    /// @param float, frequency value in Hz
    void setFrequency (float _frequency)
    {
        frequency = _frequency;
        jassert (sampleRate > 0.0f); // check if sample rate is set (the default value on initialization is 0)
        phaseDelta = frequency / sampleRate;
    }
    
    /// set phase offset (useful for phase modulation)
    /// @param float, phase offset
    void setPhaseOffset (float _phaseOffset)
    {
        phaseOffset = _phaseOffset;
    }

    /// set amplitude (useful for amplitude modulation and LFOs)
    /// @param float, amplitude
    void setAmplitude (float _amplitude)
    {
        amplitude = _amplitude;
    }

    /// set amplitude offset (useful for LFOs)
    /// @param float, amplitude offset
    void setAmplitudeOffset (float _amplitudeOffset)
    {
        amplitudeOffset = _amplitudeOffset;
    }

    /// set direct current (useful for LFOs)
    /// @param float, phase offset
    void setDC (float _dc)
    {
        dc = _dc;
    }

    /// set power value for oscillator output (useful for amplitude modulation and LFOs)
    /// @param float, power (assumed to be positive)
    void setPower (float _power)
    {
        _power = std::round (_power); // fractional powers are excluded
        jassert (_power >= 1.0f);     // negative values excluded so we don't have division by zero; for zero value we have no oscillation
        power = _power;
    }
    
private:
    // base parameters
    float frequency = 0.0f;   // frequency [Hz]
    float sampleRate = 0.0f;  // sample rate [Hz]
    float phase = 0.0f;       // phase
    float phaseDelta = 0.0f;  // phase delta
    float amplitude = 1.0f;   // amplitude
    // modulation parameters
    float phaseOffset = 0.0f;     // phase offset
    float amplitudeOffset = 0.0f; // amplitude offset
    float dc = 0.0f;              // direct current
    float power = 1.0f;           // power
};

/// Triangle oscillator which has zero amplitude points at phase = 0, 0.5, 1.
class TriOsc : public Phasor
{
public:
    /// get triangle oscillator output
    /// @param float, phase
    /// @return float, triangle oscillator output in range [-1,1]
    float output(float p) override
    {
        // the following function is used: 1 - 4*abs(1/2 - frac(1/2*p+1/4))
        // the reason for this form is that the waveshape has zero amplitude points
        // at p = 0, 1/2, 1 (the same as in sine oscillator)
        float frac = (0.5f * p + 0.25f - (int)(0.5f * p + 0.25f));
        return 1.0f - 4.0f * fabsf(0.5f - frac);
    }
};

/// Sine oscillator
class SinOsc : public Phasor
{
public:
    /// get sine oscillator output
    /// @param float, phase
    /// @return float, sine oscillator output in range [-1,1]
    float output(float p) override
    {
        return sin(p * 2 * 3.1415926535897932384626433832795f);
    }
};

/// Square oscillator
class SqrOsc : public Phasor
{
public:
    /// get square oscillator output
    /// @param float, phase
    /// @return float, square oscillator output in range [-1,1]
    float output(float p) override
    {
        float outVal = 1.0f;
        if (p > pulseWidth)
            outVal = -1.0f;
        return outVal;
    }

    /// set square wave pulse width
    /// @param float, pulse width in range [0,1]
    void setPulseWidth(float _pulseWidth)
    {
        pulseWidth = _pulseWidth;
    }
private:
    float pulseWidth = 0.5f;
};

/// Saw oscillator
class SawOsc : public Phasor
{
public:
    /// get saw oscillator output
    /// @param float, phase
    /// @return float, saw oscillator output in range [-1,1]
    float output(float p) override
    {
        return 2.0f * p - 1.0f;
    }
};

#endif // OSCILLATORS_H