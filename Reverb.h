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
        reverb.reset();
        reverb.setSampleRate (_sampleRate);
    }

    void processBlock (juce::AudioSampleBuffer& outputBuffer, int numSamples)
    {
        assignParameters();
        int numChannels = outputBuffer.getNumChannels();
        if (numChannels == 1)
            reverb.processMono (outputBuffer.getWritePointer (0), numSamples);
        else if (numChannels == 2)
            reverb.processStereo (outputBuffer.getWritePointer (0), outputBuffer.getWritePointer (1), numSamples);
    }

    void assignParameters()
    {
        juce::Reverb::Parameters reverbParameters;
        reverbParameters.dryLevel = 1.0f - *param->reverbDryWetParam;
        reverbParameters.wetLevel = 1.0f - reverbParameters.dryLevel;
        reverbParameters.roomSize = *param->reverbRoomSizeParam;
        reverbParameters.width = *param->reverbWidthParam;
        reverbParameters.damping = *param->reverbDampingParam;
        reverb.setParameters (reverbParameters);
    }
private:
    juce::Reverb reverb;
    Parameters* param;
};

#endif // REVERB_H