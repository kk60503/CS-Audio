#!/usr/bin/python3

from scipy import signal
import scipy.io.wavfile
import sounddevice as sd
import numpy as np
import librosa
import wave
import struct

infile = './voice-note.wav'

rng = np.random.default_rng()


def mix(s1, s2):
    return ((0.8 * s1) + (0.2 * s2))

def echomorph(input):
    A = np.min(input)
    B = np.max(input)
    amped = input * 2.7
    peaks, _ = signal.find_peaks(amped)
    print(peaks)
    clipped = np.clip(input, (A*0.4), (B*0.4))
    sos = signal.iirfilter(4, 0.03, btype='highpass', ftype='butter', output='sos')
    filtered = signal.sosfilt(sos, clipped)
    #num, den = signal.normalize(filtered, B)
    mixed = mix(filtered, input)
    #result = mixed * 0.7
    
    return mixed


#y, sample_rate = librosa.load(infile)

with wave.open(infile, "rb") as w:
    assert w.getnchannels() == 1
    assert w.getsampwidth() == 2
    nframes = w.getnframes()
    frames = w.readframes(nframes)
    framedata = struct.unpack(f"<{nframes}h", frames)
    samples = [s / (1 << 15) for s in framedata]
    params = w.getparams()
    sample_rate = w.getframerate()

result = np.array(samples)

for i in range(9):
    result = echomorph(result)
    sd.play(data=(result.astype(np.float32)), samplerate=sample_rate, blocking=True)
    scipy.io.wavfile.write(("test" + str(i) + ".wav"), sample_rate, result.astype(np.float32))
