#ifndef PM_SYNTH_H
#define PM_SYNTH_H


#include <JuceHeader.h>
#include "Operator.h"
#include "Filter.h"
#include "Parameters.h"

// ===========================
// ===========================
// SOUND
class PMSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote      (int) override { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override { return true; }
};




// =================================
// =================================
// Synthesiser Voice - your synth code goes in here

/*!
 @class PMSynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple PMSynthVoice objects will be created by the Synthesiser so that it can be played polyphicially

 @namespace none
 @updated 2019-06-18
 */
class PMSynthVoice : public juce::SynthesiserVoice
{
public:
    PMSynthVoice() {}

    void setParamPtr(Parameters* _param)
    {
        param = _param;
    }

    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        float freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        // prepare operators
        for (int i = 0; i < param->numOperators; i++)
        {
            ops[i].setOscWaveshape (*param->opWaveshapeParam[i]);
            ops[i].setSampleRate (getSampleRate());
            ops[i].setOscFrequency (freq * (*param->opCoarseParam[i] + *param->opFineParam[i]/1000.0f));
            ops[i].setOscAmplitude (*param->opLevelParam[i]);
            ops[i].setEnvParameters (*param->opAttackParam[i], *param->opDecayParam[i], *param->opSustainParam[i], *param->opReleaseParam[i]);
            ops[i].noteOn();
        }
        // prepare filter
        filter.startNote (param, getSampleRate());
        playing = true;
    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        for (int i = 0; i < param->numOperators; i++)
            ops[i].noteOff();
        filter.stopNote();
    }

    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here

     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                // process PM algorithm
                float algorithmOut = 0.0f;
                bool isOutput[4] = {false};
                switch (param->algorithm)
                {
                case 1: // D -> C -> B -> A
                    float opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    float opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset (opSampleC);
                    float opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset (opSampleB);
                    algorithmOut = ops[0].process();
                    isOutput[0] = true;
                }

                // process filter
                float filterOut = filter.process (algorithmOut);

                // for each channel, write the currentSample float to the output
                float outSample = filterOut;
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, outSample);
                }

                bool isActive = false;
                for (int i = 0; i < param->numOperators; i++)
                    isActive = isActive || ops[i].isEnvActive();
                if (isActive == false)
                {
                    clearCurrentNote();
                    playing = false;
                    for (int i = 0; i < param->numOperators; i++)
                        ops[i].resetEnv();
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of PMSynthSound
     */
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<PMSynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;

    Operator ops[4];
    Filter filter;
    juce::ADSR pitchEnv;
    OscSwitch lfo1;
    OscSwitch lfo2;

    // parameters pointer
    Parameters* param;
};

#endif // !PM_SYNTH_H