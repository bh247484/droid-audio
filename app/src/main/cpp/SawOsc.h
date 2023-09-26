//
// Created by Brendan Huffman on 9/23/23.
//

#ifndef DROIDAUDIO_SAWOSC_H
#define DROIDAUDIO_SAWOSC_H

/*
 * Implemented using the DPW (differentiated parabolic waveform) approach.
 * Research paper explaining the technique available here: https://www.researchgate.net/publication/220386519_Oscillator_and_Filter_Algorithms_for_Virtual_Analog_Synthesis
 * Search for a section titled: 'DPW Algorithm and the Sawtooth Waveform.'
 * Many thanks to Vesa Välimäki, the originator of the technique, for sharing/publishing his discovery.
**/
class SawOsc {
public:
    SawOsc() = default;
    ~SawOsc() = default;

    void setFreq(float freq);
    void initParams(int newSampleRate);
    float render();

private:
    float sampleRate, scalingFactor, delta;
    float freq = 440.0f;
    float phase, z1 = 0.0f;
};


#endif //DROIDAUDIO_SAWOSC_H
