//
// Created by Brendan Huffman on 9/23/23.
//

#ifndef DROIDAUDIO_AUDIOENGINE_H
#define DROIDAUDIO_AUDIOENGINE_H

#include <oboe/Oboe.h>
#include <cmath>
#include <array>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "SawOsc.h"
#include "LPF.h"
#include "dj_fft.h"

class AudioEngine : oboe::AudioStreamCallback {
public:
    AudioEngine();
    ~AudioEngine();

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;
    std::array<std::string, 3> getSpectralHexes();
    bool playSound = false;
    SawOsc sawOsc;
    LPF filter;

private:
    void updateSpectralData(float *output, int numFrames);
    void buildHexCache();
    oboe::AudioStreamBuilder builder;
    oboe::AudioStream *stream;
    int sampleRate;
    int cacheCount;
    std::vector<std::complex<float>> *fftCache = new std::vector<std::complex<float>>(0);
    std::array<std::string, 3> hexCache = {"ffffff", "ffffff", "ffffff"};
};


#endif //DROIDAUDIO_AUDIOENGINE_H
