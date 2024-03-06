#!/usr/bin/python3

import scipy.io.wavfile
import sounddevice as sd
import numpy as np
import sys, time


samp_rate = 48000 # samples per second
fs = 440 # Hz

# create float data first for loss of precision, otherwise 
#   introduces quantize noise which accumulates after further calculations 
samples = np.array([0.0] * samp_rate, dtype=np.float32)
t = np.linspace(0., 1., samp_rate)
amplitude = 0.25 * (np.iinfo(np.int16).max)

# create sine wave, convert to 16-bit and write to file
samples = amplitude * np.sin(2. * np.pi * fs * t)
scipy.io.wavfile.write("sine.wav", samp_rate, samples.astype(np.int16))

# clip sine wave, convert to 16-bit and write to file
clip_amplitude = 2 * amplitude
new_samples = clip_amplitude * np.sin(2. * np.pi * fs * t)
clip_samples = np.clip(new_samples, -8192, 8192)
scipy.io.wavfile.write("clipped.wav", samp_rate, clip_samples.astype(np.int16))

# play clipped wave directly to system's audio device after converting to 16-bit
sd.play(data=(clip_samples.astype(np.int16)), samplerate=samp_rate, blocking=True)
