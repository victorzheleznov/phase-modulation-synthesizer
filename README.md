# Phase modulation synthesizer

### Overview ###

This repository includes JUCE implementation of a phase modulation synthesizer with:
- four operators with selectable waveshape (sine, triangle, saw or square);
- a filter (lowpass, highpass, bandpass or notch) with a cutoff envelope;
- two LFOs with different routing options (operators level and phase, filter frequency and resonance, another LFO rate);
- a pitch envelope;
- built-in delay and reverb effects.

Sound examples can be found [here](https://soundcloud.com/ferrumovich/sets/pmsynth-examples/s-wcMFYgNs2w5?si=1edc54cc61d64f0cb2fc7199b601eeed&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing).

<p align="center">
<img src="https://user-images.githubusercontent.com/53834063/217703986-74b3b422-ad81-4318-a732-4486f84b2894.png" width="650">
</p>
<p align="center">
Fig. 1: phase modulation algorithms (adopted from <a href="https://www.ableton.com/en/manual/live-instrument-reference/#26-6-operator">Ableton's Operator</a>).
</p>

### Build instructions ###

1. Create a new JUCE project using basic plug-in template.
2. Check boxes *Plugin is a Synth* and *Plugin MIDI Input* under *Plugin Characteristics* in project settings.
3. Add source code files from this repository.
4. Open and build the project in an IDE of your choice.
