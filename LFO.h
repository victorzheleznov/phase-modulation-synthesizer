#ifndef LFO_H
#define LFO_H

#include <JuceHeader.h> // for juce::NormalisableRange and juce::SmoothedValue
#include "OscSwitch.h"  // base oscillator class
#include "Parameters.h" // for accessing parameters set by the user interface

/// LFO class wrapped around OscSwitch oscillator class.
/// LFO class stores information about possible routings
/// in the user interface. Class contains a set of methods 
/// for checking if an LFO instance is applied to a particular 
/// variable. LFO class needs to be revisited if there were
/// changes to the routing parameters in the user interface.
class LFO
{
public:
    /// constructor that initialises LFO frequency range
    /// @param juce::NormalisableRange<float>, frequency range
    LFO(juce::NormalisableRange<float> frequencyRange)
        : minFrequency(frequencyRange.start), maxFrequency(frequencyRange.end)
    {
        frequencyMaxOffset = 0.5 * (maxFrequency - minFrequency); // max frequency offset for an external LFO
    }

    /// process LFO
    /// @param float, output sample
    float process()
    {
        // LFO amount
        float am = amount + amountOffset;
        if (am > 1.0f)
            am = 1.0f;
        if (am < -1.0f)
            am = -1.0f;
        // LFO frequency
        float freq = frequency + frequencyOffset;
        if (freq > maxFrequency)
            freq = maxFrequency;
        if (freq < minFrequency)
            freq = minFrequency;
        // calculate smoothed value
        lfo.setFrequency (freq);
        float lfoSample = am * lfo.process();
        smoothedLFOValue.setTargetValue (lfoSample);
        float smoothedLFOSample = smoothedLFOValue.getNextValue();
        phase = lfo.getPhase();
        resetModulations();
        return smoothedLFOSample;
    }

    /// set sample rate for LFO
    /// @param float, sample rate in Hz
    void setSampleRate (float _sampleRate)
    {
        lfo.setSampleRate (_sampleRate);
    }

    /// set LFO waveshape
    /// @param int, waveshape id (0 - sine, 1 - triangle, 2 - saw, 3 - square)
    void setWaveshape (int _lfoWaveshapeId)
    {
        lfo.setWaveshape (_lfoWaveshapeId);
    }

    /// set LFO frequency
    /// @param float, frequency
    void setFrequency (float _frequency)
    {
        frequency = _frequency;
        lfo.setFrequency (_frequency);
    }

    /// set LFO frequency offset (useful for frequency modulation)
    /// @param float, frequency offset amount (from -1 to 1)
    void setFrequencyOffset (float _frequencyOffsetAmount)
    {
        frequencyOffset += _frequencyOffsetAmount * frequencyMaxOffset;
    }

    /// set LFO amplitude
    /// @param float, amplitude
    void setAmplitude (float _amplitude)
    {
        lfo.setAmplitude (_amplitude);
    }

    /// set LFO amount
    /// @param float, LFO amount (from -1 to 1)
    void setAmount (float _amount)
    {
        amount = _amount;
    }

    /// set LFO amount offset (useful for external modulations)
    /// @param float, offset for LFO amount
    void setAmountOffset (float _amountOffset)
    {
        amountOffset += _amountOffset;
    }

    /// update LFO parameters
    /// @param Parameters*, pointer to parameters set by the user interface
    /// @param int, LFO index
    /// @param float, samples rate [Hz]
    void startNote (Parameters* _param, int _idx, float _sampleRate)
    {
        (*this).setWaveshape (*_param->lfoWaveshapeParam[_idx]);
        (*this).setSampleRate (_sampleRate);
        (*this).setFrequency (*_param->lfoRateParam[_idx]);
        (*this).setAmount (*_param->lfoAmountParam[_idx]);
        if (*_param->lfoRetriggerParam[_idx] == true)
        {
            phase = 0.0f;
            lfo.setPhase (0.0f);
        }
        smoothedLFOValue.reset (_sampleRate, 1e-2f);
        smoothedLFOValue.setCurrentAndTargetValue (0.0f);
    }

    /// check if LFO is applied to the operator level
    /// @param int, LFO destination
    /// @param int, number of operators in the synth
    /// @return bool, true if LFO is applied
    bool isAppliedToOpLevel (int lfoDestination, int numOperators)
    {
        if (lfoDestination < numOperators)
            return true;
        else
            return false;
    }

    /// check if LFO is applied to operators phases
    /// @param int, LFO destination
    /// @param int, number of operators in the synth
    /// @return bool, true if LFO is applied
    bool isAppliedToOpsPhase (int lfoDestination, int numOperators)
    {
        if (lfoDestination == numOperators)
            return true;
        else
            return false;
    }

    /// check if LFO is applied to filter cutoff frequency
    /// @param int, LFO destination
    /// @param int, number of operators in the synth
    /// @return bool, true if LFO is applied
    bool isAppliedToFilterFreq (int lfoDestination, int numOperators)
    {
        if (lfoDestination == numOperators + 1)
            return true;
        else
            return false;
    }

    /// check if LFO is applied to filter resonance
    /// @param int, LFO destination
    /// @param int, number of operators in the synth
    /// @return bool, true if LFO is applied
    bool isAppliedToFilterRes (int lfoDestination, int numOperators)
    {
        if (lfoDestination == numOperators + 2)
            return true;
        else
            return false;
    }

    /// check if LFO is applied to another LFO's rate
    /// @param int, LFO destination
    /// @param int, number of operators in the synth
    /// @param int, number of LFOs in the synth
    /// @return bool, true if LFO is applied
    bool isAppliedToLFORate (int lfoDestination, int numOperators, int numLFOs)
    {
        int idxStart = numOperators + 3;
        if (lfoDestination >= idxStart && lfoDestination < idxStart + 2 * (numLFOs-1) && (lfoDestination - idxStart) % 2 == 0)
            return true;
        else
            return false;
    }

    /// check if LFO is applied to another LFO's amount
    /// @param int, LFO destination
    /// @param int, number of operators in the synth
    /// @param int, number of LFOs in the synth
    /// @return bool, true if LFO is applied
    bool isAppliedToLFOAmount (int lfoDestination, int numOperators, int numLFOs)
    {
        int idxStart = numOperators + 3;
        if (lfoDestination >= idxStart && lfoDestination < idxStart + 2 * (numLFOs - 1) && (lfoDestination - idxStart) % 2 == 1)
            return true;
        else
            return false;
    }

private:
    // base members
    OscSwitch lfo;
    juce::SmoothedValue<float> smoothedLFOValue;
    // LFO parameters
    float amount;
    float frequency;
    float phase = 0.0f; // is stored so there is an option to not retrigger LFO with a new note
    // modulation parameters
    float amountOffset = 0.0f;
    float frequencyOffset = 0.0f;
    float frequencyMaxOffset;
    // bounds
    float minFrequency;
    float maxFrequency;

    /// reset external LFO modulations (frequency and amount modulations)
    void resetModulations()
    {
        frequencyOffset = 0.0f;
        amountOffset = 0.0f;
    }
};

#endif // LFO_H