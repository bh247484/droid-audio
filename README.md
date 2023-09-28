# What is it?

An Android synthesizer app written in Kotlin and C++. The Kotlin frontend tracks a users' touchscreen input and sends the `x,y` position to the backend as parameters for custom C++ DSP code. The `y` position maps to the synth oscillator frequency (musical note/pitch) and the `x` position maps to the synth filter's cutoff.

Spectral data (harmonics/frequency content) from the synth's output is mapped to RGB color space, encoded as hex values, and sent back to the frontend as parameters for the view's background color gradient.

The frontend Kotlin code is located [here](https://github.com/bh247484/droid-audio/blob/main/app/src/main/java/com/example/droidaudio/MainActivity.kt) in `app/src/main/java/com/example/droidaudio/MainActivity.kt`.

The native C++ code and DSP algorithms are located [here](https://github.com/bh247484/droid-audio/tree/main/app/src/main/cpp) in `app/src/main/cpp/`.

# What does it look and sound like?

https://github.com/bh247484/droid-audio/assets/57693937/39a8b592-e789-4c45-b52a-e8e96f520ab4

**Attn**: Github mutes embeds by default. Be sure to unmute the above video.

# How does it work?

## Audio Engine and Android Oboe
The [oboe library](https://github.com/google/oboe) is a C++ library created by Google. It makes low latency Android audio apps possible by allowing developers to write DSP code into low level audio callbacks.

I wrote a simple `AudioEngine` Singleton class ([here](https://github.com/bh247484/droid-audio/blob/main/app/src/main/cpp/AudioEngine.cpp)) which implements the `oboe::AudioStreamCallback` abstract class, configures some oboe properties in its constructor, and overrides the all important `onAudioReady()` audio callback method.

With that class in place I was able to begin writing DSP code to make some noise.

The native JNICALL glue that allows data to pass between the front (Kotlin) and backend (C++) is written [here](https://github.com/bh247484/droid-audio/blob/main/app/src/main/cpp/native-lib.cpp) in `native-lib.cpp`.

## DSP
### Sawtooth Oscillator and DPW

Originally I set out to create a Wavetable style sawtooth oscillator but after some research I bumped into a compelling, progressive method for producing minimally aliased sawtooth waveforms. A PDF of the research paper documenting/publishing the technique can be found [here](https://www.researchgate.net/publication/220386519_Oscillator_and_Filter_Algorithms_for_Virtual_Analog_Synthesis).

The sawtooth oscillator's output is a DPW, a differentiated polynomial (or more specifically in this case, parabolic) waveform.

In brief, the algorithm computes a heavily aliased "naive" sawtooth waveform, then squares that signal (parabolic transformation), and finally differentiates (takes the moving average of) that parabolic signal to reproduce the sawtooth waveform. This time with minimal aliasing.

Higher order polynomial transformations (cubic, quartic, etc) can reduce high frequency aliasing even further but since this synth is limited to the bass register (at most A2 or `110.00hz`) the extra cpu cycles would be wasted here.

My implementation can be found in `SawOsc.cpp` [here](https://github.com/bh247484/droid-audio/blob/main/app/src/main/cpp/SawOsc.cpp). Many thanks to Vesa Välimäki, the originator of the technique, for sharing/publishing his discovery.

### Low Pass Filter

The LPF was much simpler. I used the venerable [RBJ cookbook](https://www.w3.org/TR/audio-eq-cookbook/) to create a biquad parametric lowpass filter. My implementation can be found in `LPF.cpp` [here](https://github.com/bh247484/droid-audio/blob/main/app/src/main/cpp/LPF.cpp).

Many thanks to Robert Bristow-Johnson for his famous cookbook.

### Spectral Data

I used ffts to perform some harmonic analysis on the synth's output signal. The ffts are calculated [here](https://github.com/bh247484/droid-audio/blob/main/app/src/main/cpp/AudioEngine.cpp#L85-L106) in the `AudioEngine::updateSpectralData()`. More† on the fft library I used later.

The harmonic analysis is performed [here](https://github.com/bh247484/droid-audio/blob/main/app/src/main/cpp/AudioEngine.cpp#L33-L83) in `AudioEngine::buildHexCache()` and later in that method the results are mapped to RGB color space and finally converted to hex color codes for use on the frontend. Ultimately they're applied to the view's background color gradients so that the color corresponds to the frequency content of the synth's output signal.

#### FFT Caching

Ffts are computationally expensive so the results are cached instead of recomputed every buffer.

The caching mechanism is configurable. It tracks samples elapsed in a member variable and then calculates time elapsed by scaling samples by the sampling rate. The cache resets after whatever configurable fraction of a second has elapsed.

Likewise, the hex color values are cached by the same mechanism.

#### †FFT Library

For fft computation I included the `dj_fft` [header only fft library](https://github.com/jdupuy/dj_fft) (public domain/MIT licensed). The header also included multi dimensional and gpu accelerated fft methods. I removed those and included only the code I intended to invoke.

Many thanks to Jonathan Dupuy for writing and sharing that concise, useful, and intuitive library.
