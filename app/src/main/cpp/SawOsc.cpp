//
// Created by Brendan Huffman on 9/23/23.
//

#include "SawOsc.h"
#include <cmath>

void SawOsc::setFreq(float newFreq) {
    freq = newFreq;
    scalingFactor = sampleRate / (4.0f * newFreq);
    delta = newFreq / sampleRate;
}

/*
 * These params can't be initialized in the constructor.
 * The sample rate isn't available until after the stream is opened in the AudioEngine's constructor.
 * Means sample rate is unknown at the moment SawOsc is instantiated as an AudioEngine member variable.
**/
void SawOsc::initParams(int newSampleRate) {
    sampleRate = (float) newSampleRate;
    // A more accurate but also more computationally expensive scaling factor can be used here.
    // For more details visit the link to the research paper at the top of the header file.
    // The scaling factor below makes the most sense for our use case.
    scalingFactor = sampleRate / (4.0f * freq);
    delta = freq / sampleRate;
}

float SawOsc::render() {
    phase = std::fmod(phase + delta, 1.0f);    // calc raw sawtooth with modulo
    float normPhase = 2.0f * phase - 1.0f;                  // normalize between [-1,1]
    float parabPhase = std::pow(normPhase, 2); // parabolic transform
    float diff = parabPhase - z1;                           // differentiate the parabola (moving average)

    // The differentiator below mitigates some (more) aliasing at high frequencies but it's not computationally justifiable for our use case.
    // float diff = sq - ((1 - z1) * (1 + z1) / 2);

    // Store current value as previous for use in moving average differentiation.
    z1 = parabPhase;

    return scalingFactor * diff;
}
