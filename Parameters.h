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
    std::atomic<float>* opLevelParam[4];
    std::atomic<float>* opCoarseParam[4];
    std::atomic<float>* opFineParam[4];
    std::atomic<float>* opWaveshapeParam[4];
    std::atomic<float>* opAttackParam[4];
    std::atomic<float>* opDecayParam[4];
    std::atomic<float>* opSustainParam[4];
    std::atomic<float>* opReleaseParam[4];

    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        for (int i = 0; i < numOperators; i++)
        {
            std::string paramIdBase ("op");
            paramIdBase.push_back (getLetter (i));
            std::string paramNameBase ("Op ");
            paramNameBase.push_back (getLetter (i));
            layout.add (std::make_unique<juce::AudioParameterChoice> ((paramIdBase + "Waveshape").c_str(), (paramNameBase + ": waveshape").c_str(), juce::StringArray{"Sine", "Triangle", "Saw", "Square"}, 0));
            layout.add (std::make_unique<juce::AudioParameterInt> ((paramIdBase + "Coarse").c_str(), (paramNameBase + ": coarse").c_str(), 1, 48, 1));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Level").c_str(), (paramNameBase + ": level").c_str(), 0.0f, 1.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Attack").c_str(), (paramNameBase + ": attack").c_str(), 0.0f, 10.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Decay").c_str(), (paramNameBase + ": decay").c_str(), 0.0f, 10.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Sustain").c_str(), (paramNameBase + ": sustain").c_str(), 0.0f, 1.0f, 1.0f));
            layout.add (std::make_unique<juce::AudioParameterFloat> ((paramIdBase + "Release").c_str(), (paramNameBase + ": release").c_str(), 0.0f, 10.0f, 1.0f));
        }

        return layout;
    }

    // constructor
    Parameters (juce::AudioProcessor& audioProcessor)
        : apvts (audioProcessor, nullptr, "ParameterTree", createParameterLayout())
    {
        for (int i = 0; i < numOperators; i++)
        {
            std::string paramIdBase ("op");
            paramIdBase.push_back (getLetter (i));
            opWaveshapeParam[i] = apvts.getRawParameterValue (paramIdBase + "Waveshape");;
            opLevelParam[i] = apvts.getRawParameterValue (paramIdBase + "Level");
            opCoarseParam[i] = apvts.getRawParameterValue (paramIdBase + "Coarse");
            opAttackParam[i] = apvts.getRawParameterValue (paramIdBase + "Attack");
            opDecayParam[i] = apvts.getRawParameterValue (paramIdBase + "Decay");
            opSustainParam[i] = apvts.getRawParameterValue (paramIdBase + "Sustain");
            opReleaseParam[i] = apvts.getRawParameterValue (paramIdBase + "Release");
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