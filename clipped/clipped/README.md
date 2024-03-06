# Clipped - CS410-011 - Kyle Klein

### Requirements:
    - Python 3.9 or greater
    - 'numpy'
    - 'scipy'
    - 'sounddevice'

'Clipped' is a program in the form of a Python script for generating a 
sine wave, and then "clipping" that sine wave. This would represent how 
waveforms often clip in audio engineering due to various reasons. 

The program first generates the sine wave and saves it to disk, then 
clips the sine wave and saves that clipped wave to disk, and finally plays 
that new clipped wave directly to the system's audio device.

This program first creates a 32-bit float sample array with 48000 samples per
second as the sample rate. It then sets an amplitude value as 16-bit int 
that is 25% of the maximum. Next, it uses numpy's sin() function to generate
the sine wave. Numpy then converts the sine wave to 16-bit int, and finally 
the program uses scipy to write this sine wave to a .wav file. 

Next, the sine wave is brought to 50% of the maximum and clipped using numpy.
This clipped sine wave is then written to disk using scipy. 

The last function of the program uses sounddevice to play the clipped sine wave 
directly through the system's audio device.

The project went well. I was able to create a sine wave and clipped wave easily 
enough, but figuring out how to play a generated wave directly through the
system's audio device proved to be more difficult to achieve. When I did acheive 
playback, the volume was extremely loud. I was able to fix this by converting 
the samples to 16-bit.
