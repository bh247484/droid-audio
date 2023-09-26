# What is it?

An android synthesizer app written in Kotlin and C++. The Kotlin frontend tracks a users' touch input and sends the x,y position to the backend as parameters for custom C++ DSP code. The y position maps to the synth oscillator frequency (musical note/pitch) and the x position maps to the synth filter's cutoff.

Spectral data about the synth output's harmonics/frequency content is mapped to RGB color space, encoded as hex values, and sent back to the frontend as parameters for the view's background color gradient.

Kotlin frontend code located here in ...

Native C++ code with dsp algorithims located here in ...

# What does it look/sound like?

--insert video

# How does it work?

## Android Oboe Audio
The [oboe library](https://github.com/google/oboe) is a C++ library created by Google. It makes low latency Android audio apps possible by allowing developers to write DSP code into system level audio callbacks.

I wrote a simple `AudioEngine` Singleton class (defined here) that implements the `oboe::AudioStreamCallback` abstract class, configures some oboe properties in its constructor, and overrides the all important `onAudioReady()` audio callback method.

With that class in place I was able to begin writing DSP code to make some noise.

## DSP
### Sawtooth Oscillator

Originally I set out to create a Wavetable style sawtooth oscillator but after some research I bumped into a progressive (relatively) recently discovered method for minimal aliased sawtooth oscillators. A PDF of the research paper documenting/publishing the technique can be found [here](https://www.researchgate.net/publication/220386519_Oscillator_and_Filter_Algorithms_for_Virtual_Analog_Synthesis).

The DPW (differentiated parabolic waveform) technique, in brief, involves computing a heavily aliased sawtooth waveform, then squaring that signal (parabolic transformation), and finally differentiating (taking a moving average of) that parabolic signal to end with a sawtooth waveform with minimal aliasing.

It can also be applied to square and triangle waves and other waveforms whose odd harmonics suffer from aliasing in their higher frequencies.

My implementation can be found in `SawOsc.cpp` [here](). Many thanks to Vesa Välimäki, the originator of the technique, for sharing/publishing his discovery.

### Low Pass Filter

The LPF was much simpler. I used the venerable [RBJ cookbook](https://www.w3.org/TR/audio-eq-cookbook/) to create a biquad parametric lowpass filter. My implementation can be found in `LPF.cpp` [here]().

Thanks to Robert Bristow-Johnson for his famous cookbook.

### Spectral Data Collection

I included the `dj_fft` header only fft library (public domain/MIT licensing) for spectral analysis. The header also included multi dimensional and gpu accelerated fft methods. I removed them and included only the code I used directly.

Thanks to Jonathan Dupuy for making that concise and useful library.

configurable caching mechanism that tracks the samples elapsed 