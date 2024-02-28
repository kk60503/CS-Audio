#!/usr/bin/python3
'''NOTE: Runs a little loud!'''

from scipy import signal
#import scipy.io.wavfile
import sounddevice as sd
import numpy as np
import wave
import struct

infile = './voice-note.wav'

rng = np.random.default_rng()


def read_wav(infile):
    with wave.open(infile, "rb") as w:
        assert w.getnchannels() == 1
        assert w.getsampwidth() == 2
        nframes = w.getnframes()
        frames = w.readframes(nframes)
        framedata = struct.unpack(f"<{nframes}h", frames)
        samples = [s / (1 << 15) for s in framedata]
        sample_rate = w.getframerate()
        return (samples, sample_rate)
    

def normalize(samples, peak):
    multiplier = (1/peak)
    normald = samples * multiplier
    return normald


def mix(s1, s2):
    return ((0.8 * s1) + (0.2 * s2))


def echomorph(input): 
    peak = np.max(np.abs(input))
    normald = normalize(input, peak)
    
    A = np.min(input)
    B = np.max(input)
    clipped = np.clip(normald, (A*0.4), (B*0.4))
    '''Kinda like using min/max instead of abs. peak, ears could be biased'''
    #clipped = np.clip(normald, (-0.4*peak), (0.4*peak))
    
    sos = signal.iirfilter(4, 0.03, btype='highpass', ftype='butter', output='sos')
    filtered = signal.sosfilt(sos, clipped)

    filt_peak = np.max(np.abs(filtered)) 
    normalized = normalize(filtered, filt_peak)
    
    mixed = mix(normalized, input)
    result = mixed * 0.7
    return result


'''
Did not have good luck with scipy read-in instead of wave read-in; 
Very loud and disorted
Probably just need to do some converting, but like result from wave
'''
#sample_rate, samples = scipy.io.wavfile.read(infile)

'''Main'''
samples, sample_rate = read_wav(infile)
result = np.array(samples)
sd.play(data=(result.astype(np.float32)), samplerate=sample_rate, blocking=True)

for i in range(9):
    result = echomorph(result)
    sd.play(data=(result.astype(np.float32)), samplerate=sample_rate, blocking=True)
  