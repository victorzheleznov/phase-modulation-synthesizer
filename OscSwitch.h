#ifndef OSC_SWITCH_H
#define OSC_SWITCH_H

#include <memory>        // for unique_ptr
#include <cmath>         // for round()
#include "Oscillators.h" // for using Phasor class and it's subclasses

/// Oscillator class which can change waveshape using setWaveshape() method.
class OscSwitch
{
public:
    float process()
    {
        float oscSample = osc->process();
        phase = osc->getPhase();
        return oscSample;
    }

    /// set oscillator waveshape
    /// @param int, waveshape id (0 - sine, 1 - triangle, 2 - saw, 3 - square)
    void setWaveshape (int _waveshapeId)
    {
        // detect if oscillator is already initialized and we're changing it's waveshape
        bool rewrite = false;
        if (osc != nullptr)
            rewrite = true;
        
        // set waveshape
        switch (_waveshapeId)
        {
        case 0:
            osc.reset (new SinOsc());
            break;
        case 1:
            osc.reset (new TriOsc());
            break;
        case 2:
            osc.reset (new SawOsc());
            break;
        case 3:
            osc.reset (new SqrOsc());
            break;
        default:
            osc.reset (new Phasor());
        }

        // set oscillator parameters if waveshape was overwritten
        if (rewrite == true)
        {
            osc->setSampleRate (sampleRate);
            osc->setFrequency (frequency);
            osc->setAmplitude (amplitude);
            osc->setPhaseOffset (phaseOffset);
            osc->setDC (dc);
            osc->setPower (power);
            osc->setPhase (phase);
        }
    }

    /// set sample rate
    /// @param float, sample rate in Hz
    void setSampleRate (float _sampleRate)
    {
        jassert (_sampleRate > 0.0f); // check sample rate value
        sampleRate = _sampleRate;
        osc->setSampleRate (_sampleRate);
    }

    /// set frequency
    /// @param float, frequency in Hz
    void setFrequency (float _frequency)
    {
        frequency = _frequency;
        osc->setFrequency (_frequency);
    }

    /// set phase offset (useful for phase modulation)
    /// @param float, phase offset
    void setPhaseOffset (float _phaseOffset)
    {
        phaseOffset = _phaseOffset;
        osc->setPhaseOffset (_phaseOffset);
    }

    /// set amplitude
    /// @param float, amplitude
    void setAmplitude (float _amplitude)
    {
        amplitude = _amplitude;
        osc->setAmplitude (_amplitude);
    }

    /// set amplitude offset (useful for amplitude modulation and LFO's)
    /// @param float, amplitude offset
    void setAmplitudeOffset (float _amplitudeOffset)
    {
        amplitudeOffset = _amplitudeOffset;
        osc->setAmplitudeOffset (_amplitudeOffset);
    }

    /// set direct current (useful for LFO's)
    /// @param float, phase offset
    void setDC (float _dc)
    {
        dc = _dc;
        osc->setDC (_dc);
    }

    /// set power value for oscillator output (useful for amplitude modulation and LFO's)
    /// @param float, power
    void setPower (float _power)
    {
        _power = std::round (_power); // fractional powers are excluded
        jassert (_power >= 1.0f);     // negative values excluded so we don't have division by zero; for zero value we have no oscillation
        power = _power;
        osc->setPower (_power);
    }

    float getPhase()
    {
        return phase;
    }

    void setPhase (float _phase)
    {
        osc->setPhase (_phase);
    }
private:
    std::unique_ptr<Phasor> osc = nullptr; // carrier oscillator

    // oscillator parameters (are stored so we can change oscillator waveshape in the process)
    float sampleRate = 0.0f;      // sample rate [Hz]
    float frequency = 0.0f;       // frequency [Hz]
    float phaseOffset = 0.0f;     // phase offset
    float amplitude = 1.0f;       // amplitude
    float amplitudeOffset = 0.0f; // amplitude offset
    float dc = 0.0f;              // direct current
    float power = 1.0f;           // power
    float phase = 0.0f;
};

#endif // OSC_SWITCH_H