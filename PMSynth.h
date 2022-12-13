#ifndef PM_SYNTH_H
#define PM_SYNTH_H


#include <JuceHeader.h> // for JUCE classes
#include "Operator.h"   // for operators
#include "Algorithm.h"  // for phase modulation algorithm
#include "Filter.h"     // for filter
#include "LFO.h"        // for LFOs
#include "Parameters.h" // for accessing parameters set by the user interface

/// Synthesizer sound class
class PMSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote      (int) override { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override { return true; }
};

 /// Synthesizer voice class.
 /// Each voice corresponts to one note when synthesizer is
 /// played polyphonically. This class handles all of the DSP
 /// associated with the synthesizer.
class PMSynthVoice : public juce::SynthesiserVoice
{
public:
    /// constructor synthesizer voice which handles parameters assignment
    /// @param Parameters*, pointer to parameters set by the user interface
    PMSynthVoice(Parameters* _param) :
        param (_param),
        filter (_param->apvts.getParameterRange("filterFrequency"), _param->apvts.getParameterRange("filterResonance")),
        lfo {_param->apvts.getParameterRange("lfo1Rate"), _param->apvts.getParameterRange("lfo2Rate")}
    {
    }

    /// update synthesizer's elements when a note starts playing
    /// @param int, midi note number
    /// @param float, midi note velocity
    /// @param SynthesiserSound, unused
    /// @param int, unused
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

    /// define what is done when a note stops
    /// @param float, unused (velocity)
    /// @param bool, flag to do a tail-off
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        for (int i = 0; i < param->numOperators; i++)
            ops[i].stopNote();
        filter.stopNote();
    }

    /// synthesize next block of samples
    /// @param AudioSampleBuffer&, output buffer
    /// @param int, start sample position
    /// @param int, number of samples
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        // check if this voice should be playing
        if (playing)
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
                    // operators phases modulation
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
                        lfo[i-1].setFrequencyOffset (lfoSample);
                    // previous LFO amount modulation
                    if (lfo[i].isAppliedToLFOAmount (lfoDestination, param->numOperators, param->numLFOs))
                        lfo[i-1].setAmountOffset (lfoSample);
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
                // write the current sample to the output buffer for each channel
                float outSample = filterOut;
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                    outputBuffer.addSample (chan, sampleIndex, 0.3f * outSample);
                // check envelope end for output operators
                bool isActive = false;
                for (int i = 0; i < param->numOperators; i++)
                {   
                    if (isOutput[i] == true)
                        isActive = isActive || ops[i].isEnvActive();
                }
                // clear current note
                if (isActive == false)
                {
                    clearCurrentNote();
                    playing = false;
                }
            }
        }
    }
    
    void pitchWheelMoved(int) override {}
    
    void controllerMoved(int, int) override {}
    
    /// check if this synthesizer voice play a sound
    /// @param SynthesiserSound*, pointer to synthesizer sound class
    /// @return bool, true if this voice can play a sound
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<PMSynthSound*> (sound) != nullptr;
    }
private:
    bool playing = false; // flag for voice output

    // base members
    Operator ops[4];      // four operators
    Algorithm algorithm;  // phase modulation algorithm
    Filter filter;        // filter
    LFO lfo[2];           // two LFOs

    // parameters pointer
    Parameters* param;    // parameters set by the user interface
};

#endif // !PM_SYNTH_H