#ifndef REVERB_H
#define REVERB_H

#include <JuceHeader.h> // for JUCE classes
#include "Parameters.h" // for accessing parameters set by the user interface

/// Reverb class.
/// This class is a wrapper class around juce::Reverb that adds parameter
/// mapping so the effect can be controlled from the user interface.
class Reverb
{
public:
    /// initialise parameters pointer
    /// @param Parameters*, pointer to the parameters class
    Reverb (Parameters* _param) :
        param (_param)
    {
    }

    /// initialise reverb
    /// @param float, sample rate [Hz]
    void prepareToPlay (float _sampleRate)
    {
        // reset reverb
        resetReverb();
        reverb.setSampleRate (_sampleRate);
        // initialise smoothed values
        smoothedDryWet.reset (_sampleRate, 0.1f);
        smoothedDryWet.setCurrentAndTargetValue (0.0f);
        smoothedRoomSize.reset (_sampleRate, 0.1f);
        smoothedRoomSize.setCurrentAndTargetValue (0.5f);
        smoothedWidth.reset (_sampleRate, 0.1f);
        smoothedWidth.setCurrentAndTargetValue (0.5f);
        smoothedDamping.reset (_sampleRate, 0.1f);
        smoothedDamping.setCurrentAndTargetValue (0.5f);
    }

    /// apply reverb to an audio buffer
    /// @param juce::AudioBuffer&, input audio buffer with samples
    /// @param int, number of samples in the buffer
    void processBlock (juce::AudioSampleBuffer& outputBuffer, int numSamples)
    {
        // check on/off switch
        if (*param->reverbOnParam == false)
        {
            if (isReverbReset == false)
                resetReverb();
            return;
        }
        isReverbReset = false;
        // process reverb
        assignParameters();
        int numChannels = outputBuffer.getNumChannels();
        if (numChannels == 1)
            reverb.processMono (outputBuffer.getWritePointer (0), numSamples);
        else if (numChannels == 2)
            reverb.processStereo (outputBuffer.getWritePointer (0), outputBuffer.getWritePointer (1), numSamples);
    }
private:
    // base members
    juce::Reverb reverb; // reverb
    Parameters* param;   // pointer to parameters set by the user interface
    bool isReverbReset;  // flag for reseted reverb state

    // smoothed values
    juce::SmoothedValue<float> smoothedDryWet;   // smoothed dry/wet
    juce::SmoothedValue<float> smoothedRoomSize; // smoothed room size
    juce::SmoothedValue<float> smoothedWidth;    // smoothed width
    juce::SmoothedValue<float> smoothedDamping;  // smoothed damping

    /// assign user interface parameters values to reverb
    void assignParameters()
    {
        // set target values
        smoothedDryWet.setTargetValue (*param->reverbDryWetParam);
        smoothedRoomSize.setTargetValue (*param->reverbRoomSizeParam);
        smoothedWidth.setTargetValue (*param->reverbWidthParam);
        smoothedDamping.setTargetValue (*param->reverbDampingParam);
        // set reverb parameters
        juce::Reverb::Parameters reverbParameters;
        reverbParameters.dryLevel = 1.0f - smoothedDryWet.getNextValue();
        reverbParameters.wetLevel = 1.0f - reverbParameters.dryLevel;
        reverbParameters.roomSize = smoothedRoomSize.getNextValue();
        reverbParameters.width = smoothedWidth.getNextValue();
        reverbParameters.damping = smoothedDamping.getNextValue();
        reverb.setParameters (reverbParameters);
    }
    
    /// reset reverb
    void resetReverb()
    {
        reverb.reset();
        isReverbReset = true;
    }
};

#endif // REVERB_H