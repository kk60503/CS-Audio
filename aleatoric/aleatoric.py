#!/usr/bin/python3

'''NOTE: Runs a little loud!'''

import random
from scipy import signal
import scipy.io.wavfile as scifile
import sounddevice as sd
import numpy as np


'''A minor natural notes and frequnecies'''
A4 = 440
B = 494
C = 523
D = 587
E = 659
F = 698
G = 784
A5 = 880
scale = [A4, B, C, D, E, F, G, A5]
samp_rate = 48000 # samples per second
bps = 3.67
# time linspace
t = np.linspace(0., 1., samp_rate)


def ret_sin():
    fs = random.choice(scale)
    ret_samples = sin_amp * np.sin(2. * np.pi * fs * t)
    split_samples = np.hsplit(ret_samples, 4)
    micro_split = np.hsplit(ret_samples, 100)
    mid_samples = np.append(split_samples[0], micro_split[0])
    ret_samples = np.append(mid_samples, micro_split[1])
    return ret_samples


def for_loop(sin_samples, sqr_samples):
    split_samples = np.hsplit(sqr_samples, 4)
    micro_split = np.hsplit(sqr_samples, 100)
    mid_samples = np.append(split_samples[0], micro_split[0])
    wav_file = np.append(mid_samples, micro_split[1])
    for x in range(7):
        sin_samples = ret_sin()
        wav_file = np.append(wav_file, sin_samples)
    return wav_file


'''Main'''
sin_samples = np.array([0.0] * samp_rate, dtype=np.float32)
sqr_samples = np.array([0.0] * samp_rate, dtype=np.float32)
sin_amp = 0.5 * (np.iinfo(np.int16).max)
sqr_amp = 0.75 * (np.iinfo(np.int16).max)

sqr_samples = sqr_amp * signal.square(2. * np.pi * A4 * t)

a_file = for_loop(sin_samples, sqr_samples)
b_file = for_loop(sin_samples, sqr_samples)
wav_file = np.append(a_file, b_file)
scifile.write("aleatoric.wav", samp_rate, wav_file.astype(np.int16))

print("\npress ctrl + c to quit, or ctrl + break if using Windows")
loop_on = True
while loop_on is True:
    wav_file = for_loop(sin_samples, sqr_samples)
    sd.play(data=(wav_file.astype(np.int16)), samplerate=samp_rate, blocking=True)