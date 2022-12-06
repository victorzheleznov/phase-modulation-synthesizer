#ifndef PM_SYNTH_H
#define PM_SYNTH_H


#include <JuceHeader.h>
#include "Operator.h"
#include "Filter.h"
#include "LFO.h"
#include "Delay.h"
#include "Reverb.h"
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
    PMSynthVoice(Parameters* _param) :
        param (_param),
        filter (_param->apvts.getParameterRange("filterFrequency"), _param->apvts.getParameterRange("filterResonance")),
        lfo {_param->apvts.getParameterRange("lfo1Rate"), _param->apvts.getParameterRange("lfo2Rate")}
    {
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
            ops[i].startNote (param, i, freq, getSampleRate());
        }
        // prepare filter
        filter.startNote (param, getSampleRate());
        // prepare LFOs
        for (int i = 0; i < param->numLFOs; i++)
        {
            lfo[i].startNote (param, i, getSampleRate());
        }
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
            ops[i].stopNote();
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
                // apply LFOs
                for (int i = 0; i < param->numLFOs; i++)
                {
                    int lfoDestination = *param->lfoDestinationParam[i];
                    float lfoSample = lfo[i].process();
                    // operators level modulation
                    if (lfo[i].isAppliedToOpLevel (lfoDestination, param->numOperators))
                        ops[lfoDestination].setOscAmplitudeOffset (lfoSample);
                    // operators phase modulation
                    if (lfo[i].isAppliedToOpsPhase (lfoDestination, param->numOperators))
                    {
                        for (int j = 0; j < param->numOperators; j++)
                            ops[j].setOscPhaseOffset (lfoSample);
                    }
                    // filter frequency modulation
                    if (lfo[i].isAppliedToFilterFreq (lfoDestination, param->numOperators))
                        filter.setFrequencyOffset (lfoSample);
                    // filter resonance modulation
                    if (lfo[i].isAppliedToFilterRes (lfoDestination, param->numOperators))
                        filter.setResonanceOffset (lfoSample);
                    // previous LFO rate modulation
                    if (lfo[i].isAppliedToLFORate (lfoDestination, param->numOperators, param->numLFOs))
                        lfo[i-1].setLFOFrequencyOffset (lfoSample);
                    // previous LFO amount modulation
                    if (lfo[i].isAppliedToLFOAmount (lfoDestination, param->numOperators, param->numLFOs))
                        lfo[i-1].setLFOAmountOffset (lfoSample);
                }
                
                // process PM algorithm
                float algorithmOut = 0.0f;
                bool isOutput[4] = {false};
                float opSampleA, opSampleB, opSampleC, opSampleD;
                switch ((int) *param->algorithm)
                {
                case 1: // D -> C -> B -> A
                    // specify output operators
                    isOutput[0] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset (opSampleC);
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset (opSampleB);
                    algorithmOut = ops[0].process();
                    break;
                case 2:
                    // specify output operators
                    isOutput[0] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset ((opSampleC + opSampleD) / 2);
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset (opSampleB);
                    algorithmOut = ops[0].process();
                    break;
                case 3:
                    // specify output operators
                    isOutput[0] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset (opSampleC);
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset ((opSampleB + opSampleD) / 2);
                    algorithmOut = ops[0].process();
                    break;
                case 4:
                    // specify output operators
                    isOutput[0] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset (opSampleD);
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset ((opSampleB + opSampleC) / 2);
                    algorithmOut = ops[0].process();
                    break;
                case 5:
                    // specify output operators
                    isOutput[0] = true;
                    isOutput[1] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset (opSampleC);
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset (opSampleC);
                    opSampleA = ops[0].process();
                    algorithmOut = (opSampleA + opSampleB) / 2;
                    break;
                case 6:
                    // specify output operators
                    isOutput[0] = true;
                    isOutput[1] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset (opSampleC);
                    opSampleB = ops[1].process();
                    opSampleA = ops[0].process();
                    algorithmOut = (opSampleA + opSampleB) / 2;
                    break;
                case 7:
                    // specify output operators
                    isOutput[0] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    opSampleC = ops[2].process();
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset ((opSampleB + opSampleC + opSampleD) / 3);
                    algorithmOut = ops[0].process();
                    break;
                case 8:
                    // specify output operators
                    isOutput[0] = true;
                    isOutput[2] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    opSampleC = ops[2].process();
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset (opSampleB);
                    opSampleA = ops[0].process();
                    algorithmOut = 0.5 * (opSampleA + opSampleC);
                    break;
                case 9:
                    // specify output operators
                    isOutput[0] = true;
                    isOutput[1] = true;
                    isOutput[2] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    opSampleC = ops[2].process();
                    ops[1].setOscPhaseOffset (opSampleD);
                    opSampleB = ops[1].process();
                    ops[0].setOscPhaseOffset (opSampleD);
                    opSampleA = ops[0].process();
                    algorithmOut = (opSampleA + opSampleB + opSampleC) / 3;
                    break;
                case 10:
                    // specify output operators
                    isOutput[0] = true;
                    isOutput[1] = true;
                    isOutput[2] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    ops[2].setOscPhaseOffset (opSampleD);
                    opSampleC = ops[2].process();
                    opSampleB = ops[1].process();
                    opSampleA = ops[0].process();
                    algorithmOut = (opSampleA + opSampleB + opSampleC) / 3;
                    break;
                case 11:
                    // specify output operators
                    isOutput[0] = true;
                    isOutput[1] = true;
                    isOutput[2] = true;
                    isOutput[3] = true;
                    // process algorithm
                    opSampleD = ops[3].process();
                    opSampleC = ops[2].process();
                    opSampleB = ops[1].process();
                    opSampleA = ops[0].process();
                    algorithmOut = (opSampleA + opSampleB + opSampleC + opSampleD) / 4;
                    break;
                }

                // process filter
                float filterOut = filter.process (algorithmOut);

                // for each channel, write the currentSample float to the output
                float outSample = filterOut;
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, 0.2 * outSample);
                }

                bool isActive = false;
                for (int i = 0; i < param->numOperators; i++)
                    isActive = isActive || ops[i].isEnvActive();
                if (isActive == false)
                {
                    clearCurrentNote();
                    playing = false;
                    //for (int i = 0; i < param->numOperators; i++)
                    //    ops[i].resetEnv();
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
    LFO lfo[2];

    // parameters pointer
    Parameters* param;
};

#endif // !PM_SYNTH_H