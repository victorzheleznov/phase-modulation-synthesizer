#ifndef DELAY_H
#define DELAY_H

#include <JuceHeader.h>
#include <cmath>  // for rounding functions
#include <memory> // for unique_ptr
#include "Parameters.h"

/// Delay class.
/// A class instance stores samples into a buffer of specified
/// size and outputs them with a specified delay time.
class Delay
{
public:
    Delay (Parameters* _param) :
        param(_param),
        minDelayTime(_param->apvts.getParameterRange("delayTimeLeft").start), maxDelayTime(_param->apvts.getParameterRange("delayTimeLeft").end)
    {
    }

    void prepareToPlay (float _sampleRate)
    {
        (*this).setSampleRate (_sampleRate);
        sizeInSamples = int(std::ceil (maxDelayTime * sampleRate));
        allocateBuffers();
        smoothedDryWet.reset (_sampleRate, 0.1f);
        smoothedDryWet.setCurrentAndTargetValue (0.0f);
        smoothedFeedback.reset (_sampleRate, 0.1f);
        smoothedFeedback.setCurrentAndTargetValue (0.0f);
        for (int i = 0; i < 2; i++)
        {
            smoothedDelayTime[i].reset (_sampleRate, 0.1f);
            smoothedDelayTime[i].setCurrentAndTargetValue (minDelayTime);
        }
    }

    void processBlock (juce::AudioSampleBuffer& outputBuffer, int numSamples)
    {
        // check on/off switch
        if (*param->delayOnParam == false)
        {
            if (areBuffersClear == false)
                clearBuffers();
            return;
        }
        areBuffersClear = false;
        // process delay
        int numChannels = outputBuffer.getNumChannels();
        if (numChannels == 1)
            processMono (outputBuffer.getWritePointer (0), numSamples);
        else if (numChannels == 2)
            processStereo (outputBuffer.getWritePointer (0), outputBuffer.getWritePointer (1), numSamples);
    }

private:
    float sampleRate = 0.0f;                 // sample rate [Hz]
    int sizeInSamples = 0;                   // size [samples]
    int writeIndex = 0;                      // write location in a buffer
    std::unique_ptr<float[]> buffer[2] = {nullptr}; // unique_ptr that manages a dynamically-allocated data array (data is deleted automatically when goes out of scope)
    bool areBuffersClear = false;

    Parameters* param;
    const float minDelayTime;
    const float maxDelayTime;

    // smoothed values
    juce::SmoothedValue<float> smoothedDryWet;
    juce::SmoothedValue<float> smoothedDelayTime[2];
    juce::SmoothedValue<float> smoothedFeedback;

    /// set sample rate
    /// @param float, sample rate
    void setSampleRate (float _sampleRate)
    {
        jassert (_sampleRate > 0.0f); // check sample rate value
        sampleRate = _sampleRate;
    }

    void allocateBuffers()
    {
        for (int i = 0; i < 2; i++)
            buffer[i].reset (new float[sizeInSamples]);
        clearBuffers();
    }

    void clearBuffers()
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < sizeInSamples; j++)
                buffer[i][j] = 0.0f;
        }
        areBuffersClear = true;
    }

    /// process delay line
    /// @param float, input sample
    /// @return float, delayed output
    float processSample (float _inSample, int channelIdx)
    {
        // set target values for delay time
        if (*param->delayTimeLinkParam == true)
            smoothedDelayTime[channelIdx].setTargetValue (*param->delayTimeParam[0]);
        else
            smoothedDelayTime[channelIdx].setTargetValue (*param->delayTimeParam[channelIdx]);
        // check changing delay time
        if (juce::isWithin (smoothedDelayTime[0].getCurrentValue(), smoothedDelayTime[0].getTargetValue(), 1e-6f) == false ||
            juce::isWithin (smoothedDelayTime[1].getCurrentValue(), smoothedDelayTime[1].getTargetValue(), 1e-6f) == false)
        {
            // start fade out when delay time change is detected
            smoothedDryWet.setTargetValue (0.0f);
        }
        else
        {
            // start fade in and process dry/wet normally when delay time is fixed
            smoothedDryWet.setTargetValue (*param->delayDryWetParam);
        }
        // calculate current delay time
        float delayTime;
        if (smoothedDryWet.getCurrentValue() == 0.0f)
            // change delay time only when the effect isn't active
            delayTime = smoothedDelayTime[channelIdx].getNextValue();
        else
            // keep delay time fixed while fade out
            delayTime = smoothedDelayTime[channelIdx].getCurrentValue();
        // get feedback and dry/wet values
        smoothedFeedback.setTargetValue (*param->delayFeedbackParam);
        float feedback = smoothedFeedback.getNextValue();
        float dryWet = smoothedDryWet.getNextValue();
        // process delay line
        float readTimeInSamples = float(writeIndex) - delayTime * sampleRate;
        while (readTimeInSamples < 0.0f)
            readTimeInSamples += float(sizeInSamples);
        float outSample = linearInterpolation (readTimeInSamples, channelIdx);
        buffer[channelIdx][writeIndex] = _inSample + feedback * outSample;
        writeIndex = (writeIndex + 1) % sizeInSamples;
        return (1.0f - dryWet) * _inSample + dryWet * outSample;
    }

    /// perform linear interpolation if the delay time in samples isn't integer
    float linearInterpolation (float readTimeInSamples, int channelIdx)
    {
        int indexA = int(std::floor (readTimeInSamples));
        int indexB = (indexA + 1) % sizeInSamples;
        float weight = readTimeInSamples - indexA;
        return (1.0f - weight) * buffer[channelIdx][indexA] + weight * buffer[channelIdx][indexB];
    }

    void processMono (float* samples, int numSamples)
    {
        for (int j = 0; j < numSamples; j++)
            samples[j] = processSample(samples[j], 0);
    }

    void processStereo (float* leftSamples, float* rightSamples, int numSamples)
    {
        for (int j = 0; j < numSamples; j++)
        {
            leftSamples[j] = processSample(leftSamples[j], 0);
            rightSamples[j] = processSample(rightSamples[j], 1);
        }
    }
};

#endif // DELAY_H