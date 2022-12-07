#ifndef PM_SYNTH_H
#define PM_SYNTH_H


#include <JuceHeader.h>
#include "Operator.h"
#include "Algorithm.h"
#include "Filter.h"
#include "LFO.h"
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
            ops[i].startNote (param, i, freq, velocity, getSampleRate());
        }
        // prepare algorithm
        algorithm.startNote (param);
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
                    // check on/off switch
                    if (*param->lfoOnParam[i] == false)
                        continue;
                    // get LFO sample
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
                bool isOutput[4] = {false};
                float algorithmOut = algorithm.process(ops, isOutput);

                // process filter
                float filterOut;
                if (*param->filterOnParam == true)
                    filterOut = filter.process (algorithmOut);
                else
                    filterOut = algorithmOut;

                // for each channel, write the currentSample float to the output
                float outSample = filterOut;
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, outSample);
                }

                // check envelope end for output operators
                bool isActive = false;
                for (int i = 0; i < param->numOperators; i++)
                {   
                    if (isOutput[i] == true)
                        isActive = isActive || ops[i].isEnvActive();
                }
                if (isActive == false)
                {
                    clearCurrentNote();
                    playing = false;
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

    // base members
    Operator ops[4];
    Algorithm algorithm;
    Filter filter;
    LFO lfo[2];

    // parameters pointer
    Parameters* param;
};

#endif // !PM_SYNTH_H