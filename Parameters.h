#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <JuceHeader.h> // for JUCE classes
#include <string>       // for std::string

/// Parameters class.
/// This class handles creation of parameter layout
/// and assignment of AudioProcessorValueTreeState's
/// parameter pointers to public variables of this
/// class. A pointer to a Parameters class can be
/// passed to any other class where access to user
/// defined parameters is needed.
class Parameters
{
public:
    // define value tree
    juce::AudioProcessorValueTreeState apvts;

    // define number of operators and oscillators in the synth
    const int numOperators;
    const int numLFOs;

    // operators parameters
    std::atomic<float>* algorithm;                 // algorithm number
    std::atomic<float>* opLevelParam[4];           // operators' levels
    std::atomic<float>* opCoarseParam[4];          // operators' coarse frequency
    std::atomic<float>* opFineParam[4];            // operators' fine frequency
    std::atomic<float>* opWaveshapeParam[4];       // operators' waveshape
    std::atomic<float>* opAttackParam[4];          // attack for operators' amplitudes
    std::atomic<float>* opDecayParam[4];           // decay for operators' amplitudes
    std::atomic<float>* opSustainParam[4];         // sustain for operators' amplitudes
    std::atomic<float>* opReleaseParam[4];         // release for operators' amplitudes
    std::atomic<float>* opFixedModeParam[4];       // on/off switch for operators' fixed frequency mode
    std::atomic<float>* opFixedFreqParam[4];       // fixed frequency for operators' amplitudes
    // filter parameters
    std::atomic<float>* filterOnParam;             // on/off switch for filter
    std::atomic<float>* filterTypeParam;           // filter type
    std::atomic<float>* filterFrequencyParam;      // filter cutoff frequency
    std::atomic<float>* filterResonanceParam;      // filter resonance
    std::atomic<float>* filterEnvAmountParam;      // amount for the cutoff envelope
    std::atomic<float>* filterAttackParam;         // attack for the cutoff envelope
    std::atomic<float>* filterDecayParam;          // decay for the cutoff envelope
    std::atomic<float>* filterSustainParam;        // sustain for the cutoff envelope
    std::atomic<float>* filterReleaseParam;        // release for the cutoff envelope
    // LFOs parameters
    std::atomic<float>* lfoOnParam[2];             // on/off switch for LFOs
    std::atomic<float>* lfoDestinationParam[2];    // LFOs destinations
    std::atomic<float>* lfoWaveshapeParam[2];      // LFOs waveshapes
    std::atomic<float>* lfoRateParam[2];           // LFOs rate
    std::atomic<float>* lfoAmountParam[2];         // LFOs amount
    std::atomic<float>* lfoRetriggerParam[2];      // LFOs retrigger switch
    // pitch envelope parameters
    std::atomic<float>* pitchEnvOnParam;           // on/off switch for operators pitch envelope
    std::atomic<float>* pitchEnvInitialLevelParam; // initial level for pitch envelope
    std::atomic<float>* pitchEnvDecayParam;        // decay for pitch envelope
    // delay parameters
    std::atomic<float>* delayOnParam;              // on/off switch for delay
    std::atomic<float>* delayDryWetParam;          // delay dry/wet
    std::atomic<float>* delayTimeParam[2];         // delay time for left/right channels
    std::atomic<float>* delayTimeLinkParam;        // delay stereo link switch
    std::atomic<float>* delayFeedbackParam;        // delay feedback
    // reverb parameters
    std::atomic<float>* reverbOnParam;             // on/off switch for reverb
    std::atomic<float>* reverbDryWetParam;         // reverb dry/wet
    std::atomic<float>* reverbRoomSizeParam;       // reverb room size
    std::atomic<float>* reverbWidthParam;          // reverb width
    std::atomic<float>* reverbDampingParam;        // reverb damping
    
    /// create parameters layout
    /// @param int, number of operators in the synthesizer
    /// @param int, number of LFOs in the sunthesizer
    /// @return, parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(const int numOperators, const int numLFOs)
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout; // parameters layout
        juce::StringArray lfoDestinations;                          // possible destinations for LFOs
        // algorithm
        layout.add (std::make_unique<juce::AudioParameterChoice> ("algorithm", "PM algorithm", juce::StringArray{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"}, 0));
        // operators layout
        for (int i = 0; i < numOperators; i++)
        {
            std::string paramIdBase ("op");
            paramIdBase.push_back (getLetter (i));
            std::string paramNameBase ("Op ");
            paramNameBase.push_back (getLetter (i));
            layout.add (std::make_unique<juce::AudioParameterChoice> ((paramIdBase + "Waveshape").c_str(), (paramNameBase + ": waveshape").c_str(), juce::StringArray{"Sine", "Triangle", "Saw", "Square"}, 0));
            layout.add (std::make_unique<juce::AudioParameterInt> ((paramIdBase + "Coarse").c_str(), (paramNameBase + ": coarse").c_str(), 1, 48, 1));
            layout.add (std::make_unique<juce::AudioParameterInt> ((paramIdBase + "Fine").c_str(), (paramNameBase + ": fine").c_str(), 0, 1000, 0));
            if (i == 0)
                layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Level").c_str(), (paramNameBase + ": level").c_str(), 0.0f, 1.0f, 1.0f));
            else
                layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Level").c_str(), (paramNameBase + ": level").c_str(), 0.0f, 1.0f, 0.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Attack").c_str(), (paramNameBase + ": attack").c_str(), juce::NormalisableRange<float> (1e-3f, 20.0f, 0.0f, 0.25f), 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Decay").c_str(), (paramNameBase + ": decay").c_str(), juce::NormalisableRange<float> (1e-3f, 60.0f, 0.0f, 0.25f), 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Sustain").c_str(), (paramNameBase + ": sustain").c_str(), 0.0f, 1.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Release").c_str(), (paramNameBase + ": release").c_str(), juce::NormalisableRange<float> (1e-3f, 60.0f, 0.0f, 0.25f), 1.0f));
            layout.add (std::make_unique<juce::AudioParameterBool> ((paramIdBase + "FixedMode").c_str(), (paramNameBase + ": fixed freq mode").c_str(), false));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "FixedFreq").c_str(), (paramNameBase + ": frequency").c_str(), juce::NormalisableRange<float> (10.0f, 2000.0f, 0.0f, 0.25f), 100.0f));
            lfoDestinations.add ((paramNameBase + " level").c_str());
        }
        lfoDestinations.add ("Operators phase");
        // filter layout
        layout.add (std::make_unique<juce::AudioParameterBool> ("filterOn", "Filter: on", true));
        layout.add (std::make_unique<juce::AudioParameterChoice> ("filterType", "Filter: type", juce::StringArray{"Low-pass", "High-pass", "Band-pass", "Notch"}, 0));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterFrequency", "Filter: frequency", juce::NormalisableRange<float> (30.0f, 18500.0f, 0.0f, 0.25f), 10000.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterResonance", "Filter: resonance", 0.1f, 10.0f, 0.1f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterEnvAmount", "Filter: envelope amount", -1.0f, 1.0f, 0.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterAttack", "Filter: attack", juce::NormalisableRange<float> (1e-3f, 20.0f, 0.0f, 0.25f), 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterDecay", "Filter: decay", juce::NormalisableRange<float> (1e-3f, 60.0f, 0.0f, 0.25f), 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterSustain", "Filter: sustain", 0.0f, 1.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterRelease", "Filter: release", juce::NormalisableRange<float> (1e-3f, 60.0f, 0.0f, 0.25f), 1.0f));
        lfoDestinations.add ("Filter frequency");
        lfoDestinations.add ("Filter resonance");
        // LFOs layout
        for (int i = 0; i < numLFOs; i++)
        {
            std::string paramIdBase ("lfo");
            paramIdBase += std::to_string (i+1);
            std::string paramNameBase ("LFO ");
            paramNameBase += std::to_string (i+1);
            layout.add (std::make_unique<juce::AudioParameterBool> ((paramIdBase + "On").c_str(), (paramNameBase + ": on").c_str(), false));
            layout.add (std::make_unique<juce::AudioParameterChoice> ((paramIdBase + "Destination").c_str(), (paramNameBase + ": destination").c_str(), lfoDestinations, 0));
            layout.add (std::make_unique<juce::AudioParameterChoice> ((paramIdBase + "Waveshape").c_str(), (paramNameBase + ": waveshape").c_str(), juce::StringArray{ "Sine", "Triangle", "Saw", "Square" }, 0));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Rate").c_str(), (paramNameBase + ": rate").c_str(), juce::NormalisableRange<float> (0.01f, 40.0f, 0.0f, 0.25f), 0.01f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Amount").c_str(), (paramNameBase + ": amount").c_str(), -1.0f, 1.0f, 0.0f));
            layout.add (std::make_unique<juce::AudioParameterBool> ((paramIdBase + "Retrigger").c_str(), (paramNameBase + ": retrigger").c_str(), true));
            lfoDestinations.add ((paramNameBase + " rate").c_str());
            lfoDestinations.add ((paramNameBase + " amount").c_str());
        }
        // pitch envelope
        layout.add (std::make_unique<juce::AudioParameterBool> ("pitchEnvOn", "Pitch env: on", false));
        layout.add (std::make_unique<juce::AudioParameterInt> ("pitchEnvInitialLevel", "Pitch env: initial level", -48, 48, 0));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("pitchEnvDecay", "Pitch env: decay", juce::NormalisableRange<float> (1e-3f, 60.0f, 0.0f, 0.25f), 1.0f));
        // delay
        layout.add (std::make_unique<juce::AudioParameterBool> ("delayOn", "Delay: on", false));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("delayDryWet", "Delay: dry/wet", 0.0f, 1.0f, 0.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("delayTimeLeft", "Delay: left time", 0.01f, 3.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("delayTimeRight", "Delay: right time", 0.01f, 3.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterBool> ("delayTimeLink", "Delay: link stereo", true));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("delayFeedback", "Delay: feedback", 0.0f, 1.0f, 0.0f));
        // reverb
        layout.add (std::make_unique<juce::AudioParameterBool> ("reverbOn", "Reverb: on", false));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("reverbDryWet", "Reverb: dry/wet", 0.0f, 1.0f, 0.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("reverbRoomSize", "Reverb: room size", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("reverbWidth", "Reverb: width", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("reverbDamping", "Reverb: damping", 0.0f, 1.0f, 0.5f));
        return layout;
    }

    /// constructor which initializes parameter layout and assigns parameter pointers to public class variables
    /// @param AudioProcessor&, audio processor
    /// @param int, number of operators in the synthesizer
    /// @param int, number of LFOs in the sunthesizer
    Parameters (juce::AudioProcessor& audioProcessor, const int _numOperators, const int _numLFOs) :
        numOperators (_numOperators), numLFOs (_numLFOs), 
        apvts (audioProcessor, nullptr, "ParameterTree", createParameterLayout(_numOperators, _numLFOs))
    {
        // algorithm
        algorithm = apvts.getRawParameterValue ("algorithm");
        // operators parameters
        for (int i = 0; i < numOperators; i++)
        {
            std::string paramIdBase ("op");
            paramIdBase.push_back (getLetter (i));
            opWaveshapeParam[i] = apvts.getRawParameterValue (paramIdBase + "Waveshape");;
            opCoarseParam[i] = apvts.getRawParameterValue (paramIdBase + "Coarse");
            opFineParam[i] = apvts.getRawParameterValue (paramIdBase + "Fine");
            opLevelParam[i] = apvts.getRawParameterValue (paramIdBase + "Level");
            opAttackParam[i] = apvts.getRawParameterValue (paramIdBase + "Attack");
            opDecayParam[i] = apvts.getRawParameterValue (paramIdBase + "Decay");
            opSustainParam[i] = apvts.getRawParameterValue (paramIdBase + "Sustain");
            opReleaseParam[i] = apvts.getRawParameterValue (paramIdBase + "Release");
            opFixedModeParam[i] = apvts.getRawParameterValue (paramIdBase + "FixedMode");
            opFixedFreqParam[i] = apvts.getRawParameterValue (paramIdBase + "FixedFreq");
        }
        // filter parameters
        filterOnParam = apvts.getRawParameterValue ("filterOn");
        filterTypeParam = apvts.getRawParameterValue ("filterType");
        filterFrequencyParam = apvts.getRawParameterValue ("filterFrequency");
        filterResonanceParam = apvts.getRawParameterValue ("filterResonance");
        filterEnvAmountParam = apvts.getRawParameterValue ("filterEnvAmount");
        filterAttackParam = apvts.getRawParameterValue ("filterAttack");
        filterDecayParam = apvts.getRawParameterValue ("filterDecay");
        filterSustainParam = apvts.getRawParameterValue ("filterSustain");
        filterReleaseParam = apvts.getRawParameterValue ("filterRelease");
        // LFOs parameters
        for (int i = 0; i < numLFOs; i++)
        {
            std::string paramIdBase ("lfo");
            paramIdBase += std::to_string (i + 1);
            lfoOnParam[i] = apvts.getRawParameterValue (paramIdBase + "On");
            lfoDestinationParam[i] = apvts.getRawParameterValue (paramIdBase + "Destination");
            lfoWaveshapeParam[i] = apvts.getRawParameterValue (paramIdBase + "Waveshape");
            lfoRateParam[i] = apvts.getRawParameterValue (paramIdBase + "Rate");
            lfoAmountParam[i] = apvts.getRawParameterValue (paramIdBase + "Amount");
            lfoRetriggerParam[i] = apvts.getRawParameterValue (paramIdBase + "Retrigger");
        }
        // pitch envelope
        pitchEnvOnParam = apvts.getRawParameterValue ("pitchEnvOn");
        pitchEnvInitialLevelParam = apvts.getRawParameterValue ("pitchEnvInitialLevel");
        pitchEnvDecayParam = apvts.getRawParameterValue ("pitchEnvDecay");
        // delay
        delayOnParam = apvts.getRawParameterValue ("delayOn");
        delayDryWetParam = apvts.getRawParameterValue ("delayDryWet");
        delayTimeParam[0] = apvts.getRawParameterValue ("delayTimeLeft");
        delayTimeParam[1] = apvts.getRawParameterValue ("delayTimeRight");
        delayTimeLinkParam = apvts.getRawParameterValue ("delayTimeLink");
        delayFeedbackParam = apvts.getRawParameterValue ("delayFeedback");
        // reverb
        reverbOnParam = apvts.getRawParameterValue ("reverbOn");
        reverbDryWetParam = apvts.getRawParameterValue ("reverbDryWet");
        reverbRoomSizeParam = apvts.getRawParameterValue ("reverbRoomSize");
        reverbWidthParam = apvts.getRawParameterValue ("reverbWidth");
        reverbDampingParam = apvts.getRawParameterValue ("reverbDamping");
    }
private:
    /// get nth letter from alphabet
    /// @param int, letter's number in alphabet
    /// @return char, alphaber letter
    char getLetter(int n)
    {
        jassert (n >= 0 && n <= 26);
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[n];
    }
};

#endif // PARAMETERS_H