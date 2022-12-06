#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <JuceHeader.h>
#include <sstream>
#include <string>

class Parameters
{
public:
    juce::AudioProcessorValueTreeState apvts;

    // define parameters
    const int numOperators; // A, B, C, D operators
    const int numLFOs;

    // on/off switches

    // operators parameters
    std::atomic<float>* algorithm;
    std::atomic<float>* opLevelParam[4];
    std::atomic<float>* opCoarseParam[4];
    std::atomic<float>* opFineParam[4];
    std::atomic<float>* opWaveshapeParam[4];
    std::atomic<float>* opAttackParam[4];
    std::atomic<float>* opDecayParam[4];
    std::atomic<float>* opSustainParam[4];
    std::atomic<float>* opReleaseParam[4];
    // filter parameters
    std::atomic<float>* filterTypeParam;
    std::atomic<float>* filterFrequencyParam;
    std::atomic<float>* filterResonanceParam;
    std::atomic<float>* filterEnvAmountParam;
    std::atomic<float>* filterAttackParam;
    std::atomic<float>* filterDecayParam;
    std::atomic<float>* filterSustainParam;
    std::atomic<float>* filterReleaseParam;
    // LFO parameters
    std::atomic<float>* lfoDestinationParam[2];
    std::atomic<float>* lfoWaveshapeParam[2];
    std::atomic<float>* lfoRateParam[2];
    std::atomic<float>* lfoAmountParam[2];
    std::atomic<float>* lfoRetrigger[2];
    
    /// create parameters layout
    /// @return, parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(const int numOperators, const int numLFOs)
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        juce::StringArray lfoDestinations;
        // algorithm
        layout.add (std::make_unique<juce::AudioParameterInt> ("algorithm", "PM algorithm", 1, 11, 1));
        // operators layout
        for (int i = 0; i < numOperators; i++)
        {
            std::string paramIdBase ("op");
            paramIdBase.push_back (getLetter (i));
            std::string paramNameBase ("Op ");
            paramNameBase.push_back (getLetter (i));
            layout.add (std::make_unique<juce::AudioParameterChoice> ((paramIdBase + "Waveshape").c_str(), (paramNameBase + ": waveshape").c_str(), juce::StringArray{"Sine", "Triangle", "Saw", "Square"}, 0));
            layout.add (std::make_unique<juce::AudioParameterInt> ((paramIdBase + "Coarse").c_str(), (paramNameBase + ": coarse").c_str(), 1, 48, 1));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Fine").c_str(), (paramNameBase + ": fine").c_str(), 0.0f, 1000.0f, 0.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Level").c_str(), (paramNameBase + ": level").c_str(), 0.0f, 1.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Attack").c_str(), (paramNameBase + ": attack").c_str(), 0.0f, 10.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Decay").c_str(), (paramNameBase + ": decay").c_str(), 0.0f, 10.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Sustain").c_str(), (paramNameBase + ": sustain").c_str(), 0.0f, 1.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Release").c_str(), (paramNameBase + ": release").c_str(), 0.0f, 10.0f, 1.0f));
            lfoDestinations.add ((paramNameBase + " level").c_str());
            lfoDestinations.add ((paramNameBase + " phase").c_str());
        }
        // filter layout
        layout.add (std::make_unique<juce::AudioParameterChoice> ("filterType", "Filter : type", juce::StringArray{"Low-pass", "High-pass", "Band-pass", "Notch"}, 0));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterFrequency", "Filter: frequency", 30.0f, 18500.0f, 10000.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterResonance", "Filter: resonance", 0.1f, 10.0f, 0.1f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterEnvAmount", "Filter: envelope amount", -1.0f, 1.0f, 0.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterAttack", "Filter: attack", 0.0f, 10.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterDecay", "Filter: decay", 0.0f, 10.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterSustain", "Filter: sustain", 0.0f, 1.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterRelease", "Filter: release", 0.0f, 10.0f, 1.0f));
        lfoDestinations.add ("Filter frequency");
        lfoDestinations.add ("Filter resonance");
        // LFOs layout
        for (int i = 0; i < numLFOs; i++)
        {
            std::string paramIdBase ("lfo");
            paramIdBase += std::to_string (i+1);
            std::string paramNameBase ("LFO ");
            paramNameBase += std::to_string (i+1);
            layout.add (std::make_unique<juce::AudioParameterChoice> ((paramIdBase + "Destination").c_str(), (paramNameBase + ": destination").c_str(), lfoDestinations, 0));
            layout.add (std::make_unique<juce::AudioParameterChoice> ((paramIdBase + "Waveshape").c_str(), (paramNameBase + ": waveshape").c_str(), juce::StringArray{ "Sine", "Triangle", "Saw", "Square" }, 0));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Rate").c_str(), (paramNameBase + ": rate").c_str(), 0.01f, 40.0f, 0.01f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Amount").c_str(), (paramNameBase + ": amount").c_str(), 0.0f, 1.0f, 0.0f));
            layout.add (std::make_unique<juce::AudioParameterBool> ((paramIdBase + "Retrigger").c_str(), (paramNameBase + ": retrigger").c_str(), true));
            lfoDestinations.add ((paramNameBase + " rate").c_str());
            lfoDestinations.add ((paramNameBase + " amount").c_str());
        }
        return layout;
    }

    /// constructor which initializes parameter layout and assigns parameter pointers to public class variables
    /// @param audio processor
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
        }
        // filter parameters
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
            lfoDestinationParam[i] = apvts.getRawParameterValue (paramIdBase + "Destination");
            lfoWaveshapeParam[i] = apvts.getRawParameterValue (paramIdBase + "Waveshape");
            lfoRateParam[i] = apvts.getRawParameterValue (paramIdBase + "Rate");
            lfoAmountParam[i] = apvts.getRawParameterValue (paramIdBase + "Amount");
            lfoRetrigger[i] = apvts.getRawParameterValue (paramIdBase + "Retrigger");
        }
    }
private:
    char getLetter(int n)
    {
        jassert (n >= 0 && n <= 26);
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[n];
    }
};

#endif // PARAMETERS_H