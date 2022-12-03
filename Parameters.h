#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <JuceHeader.h>
#include <sstream>
#include <string>

class Parameters
{
public:
    // define parameters
    const int numOperators = 4; // A, B, C, D operators
    int algorithm = 1; // 1
    juce::AudioProcessorValueTreeState apvts;

    // operators parameters
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
    
    /// create parameters layout
    /// @return, parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
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
        }
        // filter layout
        layout.add (std::make_unique<juce::AudioParameterChoice> ("filterType", "Filter : type", juce::StringArray{"Low-pass", "High-pass", "Band-pass", "Notch"}, 0));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterFrequency", "Filter: frequency", 0.1f, 22000.0f, 10000.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterResonance", "Filter: resonance", 0.1f, 1.5f, 0.1f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterEnvAmount", "Filter: envelope amount", -1.0f, 1.0f, 0.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterAttack", "Filter: attack", 0.0f, 10.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterDecay", "Filter: decay", 0.0f, 10.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterSustain", "Filter: sustain", 0.0f, 1.0f, 1.0f));
        layout.add (std::make_unique<juce::AudioParameterFloat> ("filterRelease", "Filter: release", 0.0f, 10.0f, 1.0f));
        return layout;
    }

    /// constructor which initializes parameter layout and assigns parameter pointers to public class variables
    /// @param audio processor
    Parameters (juce::AudioProcessor& audioProcessor)
        : apvts (audioProcessor, nullptr, "ParameterTree", createParameterLayout())
    {
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
    }
private:
    char getLetter(int n)
    {
        jassert (n >= 0 && n <= 26);
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[n];
    }
};

#endif // PARAMETERS_H