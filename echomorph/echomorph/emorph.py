#!/usr/bin/python3

'''This is Emorph, or little Echomorph. I kinda like Emorph. It has it's own voice with a trailing echo.'''

from scipy import signal
#import scipy.io.wavfile
import sounddevice as sd
import numpy as np
import wave
import struct

infile = './voice-note.wav'

rng = np.random.default_rng()


def mix(s1, s2):
    return ((0.8 * s1) + (0.2 * s2))

def echomorph(input):
    A = np.min(input)
    B = np.max(input)
    check = A + B
    if check > 0:
        peak = B
    if check < 0:
        peak = A

    multiplier = 2 - peak
    
    clipped = np.clip(input, (A*0.4), (B*0.4))
    
    sos = signal.iirfilter(4, 0.0333, btype='highpass', ftype='butter', output='sos')
    filtered = signal.sosfilt(sos, clipped)
    
    amped = filtered * multiplier
    mixed = mix(amped, input)
    result = mixed * 0.7
    return result


'''Main'''
with wave.open(infile, "rb") as w:
    assert w.getnchannels() == 1
    assert w.getsampwidth() == 2
    nframes = w.getnframes()
    frames = w.readframes(nframes)
    framedata = struct.unpack(f"<{nframes}h", frames)
    samples = [s / (1 << 15) for s in framedata]
    sample_rate = w.getframerate()

result = np.array(samples)
'''Play unadultered .wav file'''
sd.play(data=(result.astype(np.float32)), samplerate=sample_rate, blocking=True)

for i in range(9):
    result = echomorph(result)
    '''Play each mutation of .wav file'''
    sd.play(data=(result.astype(np.float32)), samplerate=sample_rate, blocking=True)
