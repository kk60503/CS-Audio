# Echomorph - CS410-011 - Kyle Klein

### Requirements:

    - Python 3.9 or greater
    - 'numpy'
    - 'scipy'
    - 'sounddevice'
    - 'wave'
    - 'struct'

'Echomorph' is a program in the form of a Python script that provides an audio
effect applied in this case to a given .wav file. The effect is a bit like a
delay that distorts the signal more and more with each iteration. It was
modeled after the effect heard in Pink Floyd's song "Dogs" at around 8 minutes.

<code>emorph.py</code> was my first attempt at the Echomorph program, and
has it's own voicing and delay, so I kept it around.

This project began trying to decipher scipy's documentation for its
<code>signal</code> package's filter functions; specifically
<code>iirfilter()</code> and <code>sosfilt()</code>. It is a bit personal for
me personally to decipher some of scipy's documentation, as it is obviously
scientific and reads as quite academic. Eventually after playing around with
these functions for long enough, I began to understand what the parameters
meant/accomplish.

I also realized that my MacBook was auto-playing my created .wav files for
this and the 'clipped' project through Apple Music, which must have some
sort of auto-correcting features because the .wav files sounded and
behaved much differently when played through programs such as Audacity.

I enjoyed using this project as a way to better understand and apply digital sound
manipulation and how to use tools like numpy and scipy to easily achieve
such feats. I feel like the project is pretty well finished.
