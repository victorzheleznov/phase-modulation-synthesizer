#ifndef REVERB_H
#define REVERB_H

#include <JuceHeader.h>
#include "Parameters.h"

class Reverb
{
public:
    Reverb (Parameters* _param) :
        param (_param)
    {}

    void prepareToPlay (float _sampleRate)
    {
        resetReverb();
        reverb.setSampleRate (_sampleRate);
        smoothedDryWet.reset (_sampleRate, 0.1f);
        smoothedDryWet.setCurrentAndTargetValue (0.0f);
        smoothedRoomSize.reset (_sampleRate, 0.1f);
        smoothedRoomSize.setCurrentAndTargetValue (0.5f);
        smoothedWidth.reset (_sampleRate, 0.1f);
        smoothedWidth.setCurrentAndTargetValue (0.5f);
        smoothedDamping.reset (_sampleRate, 0.1f);
        smoothedDamping.setCurrentAndTargetValue (0.5f);
    }

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
    juce::Reverb reverb;
    Parameters* param;
    bool isReverbReset;

    // smoothed values
    juce::SmoothedValue<float> smoothedDryWet;
    juce::SmoothedValue<float> smoothedRoomSize;
    juce::SmoothedValue<float> smoothedWidth;
    juce::SmoothedValue<float> smoothedDamping;

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

    void resetReverb()
    {
        reverb.reset();
        isReverbReset = true;
    }
};

#endif // REVERB_H