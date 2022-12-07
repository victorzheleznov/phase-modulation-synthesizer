#ifndef LFO_H
#define LFO_H

#include "OscSwitch.h"

// wrapped class around OscSwith
class LFO
{
public:
    /// constructor that resets filter instance
    LFO(juce::NormalisableRange<float> frequencyRange)
        : minFrequency(frequencyRange.start), maxFrequency(frequencyRange.end)
    {
        frequencyMaxOffset = 0.5 * (maxFrequency - minFrequency); // max frequency offset for an external LFO
    }

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

    void setAmount (float _amount)
    {
        amount = _amount;
    }

    void setAmountOffset (float _amountOffset)
    {
        amountOffset += _amountOffset;
    }

    /// start the attack phase of the envelope
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

    /// start the release phase of the envelope
    void stopNote()
    {}

    ///
    bool isAppliedToOpLevel (int lfoDestination, int numOperators)
    {
        if (lfoDestination < numOperators)
            return true;
        else
            return false;
    }

    bool isAppliedToOpsPhase (int lfoDestination, int numOperators)
    {
        if (lfoDestination == numOperators)
            return true;
        else
            return false;
    }

    bool isAppliedToFilterFreq (int lfoDestination, int numOperators)
    {
        if (lfoDestination == numOperators + 1)
            return true;
        else
            return false;
    }

    bool isAppliedToFilterRes (int lfoDestination, int numOperators)
    {
        if (lfoDestination == numOperators + 2)
            return true;
        else
            return false;
    }

    bool isAppliedToLFORate (int lfoDestination, int numOperators, int numLFOs)
    {
        int idxStart = numOperators + 3;
        if (lfoDestination >= idxStart && lfoDestination < idxStart + 2 * (numLFOs-1) && (lfoDestination - idxStart) % 2 == 0)
            return true;
        else
            return false;
    }

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
    float phase = 0.0f; // is storesd so there is an option to not retrigger LFO with a new note
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