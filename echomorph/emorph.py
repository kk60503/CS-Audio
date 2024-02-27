#!/usr/bin/python3

from scipy import signal
import scipy.io.wavfile
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
    print(A)
    print(B)
    check = A + B
    if check > 0:
        peak = B
    if check < 0:
        peak = A
    multiplier = 2 - peak
    print(multiplier)

    clipped = np.clip(input, (A*0.4), (B*0.4))
    #scipy.io.wavfile.write(("clipped.wav"), sample_rate, clipped.astype(np.float32))
    sos = signal.iirfilter(4, 0.0333, btype='highpass', ftype='butter', output='sos')
    filtered = signal.sosfilt(sos, clipped)
    amped = filtered * multiplier
    #scipy.io.wavfile.write(("amped.wav"), sample_rate, amped.astype(np.float32))
    mixed = mix(amped, input)
    result = mixed * 0.7
    
    return result


with wave.open(infile, "rb") as w:
    assert w.getnchannels() == 1
    assert w.getsampwidth() == 2
    nframes = w.getnframes()
    frames = w.readframes(nframes)
    framedata = struct.unpack(f"<{nframes}h", frames)
    samples = [s / (1 << 15) for s in framedata]
    #params = w.getparams()
    sample_rate = w.getframerate()

result = np.array(samples)

sd.play(data=(result.astype(np.float32)), samplerate=sample_rate, blocking=True)

for i in range(9):
    result = echomorph(result)
    sd.play(data=(result.astype(np.float32)), samplerate=sample_rate, blocking=True)
    #scipy.io.wavfile.write(("test" + str(i) + ".wav"), sample_rate, result.astype(np.float32))
