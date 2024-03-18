# Brina Luna

Kyle Klein

A synth plugin meant to serve as my introduction to the JUCE framework/software.

Originally intended to be a phaser audio plugin, the project pivoted toward using
a JUCE tutorial for DSP and reverse engineering it to implement and tweak a phaser 
effect. Reverb and distortion effects were added subsequently.

The standalone plugin has (currently) only been tested by building with Xtools and 
briefly using an old synth connected via USB for external MIDI. The project should
be able to be opened and built with (Microsoft) VS Code and a Linux Makefile. 
While it may be possible to build the plugin as VST3, AU, etc., please note that
the plugin has only been built and tested as a standalone plugin (currently).
Once built, the executable file should reside in the accompanying directory path:
<code>Builds/YourChosenIDE/build/Debug/Brina Luna</code>

NOTE: It is assumed that the user has installed the JUCE software.

**For building the plugin:**</br>
Please install JUCE software and familiarize with the Projucer application.
The Projucer will allow the user to choose which option (Xtools/VS Code/Makefile)
to edit and build the synth with. User should be able to build and open the 
standalone plugin from their chosen editor/environment.

The (JUCE modules/juce_dsp/widgets/) Phaser.h and Phaser.cpp file have been edited for this project. 
The one small difference is the change in line 202 of the Phaser.h file:

The <code>static constexpr int numStages = 6;</code> was changed to 4:
<code>static constexpr int numStages = 4;</code> </br>
This comes down to user preference and is optional.

The plugin currently has some issues with popping and crackling if using the 
reverb and distortion effects together. The distortion is not quite to the point
of adequacy. Phaser + reverb is adequate and enjoyable. Efforts will be made for
improved audio and greater GUI options for effect controls.

**Changing the audio effects:**

In the AudioEngine class portion of the BLPhaser.h file, there will be comments instructing user 
on how to disable any effects. User can also change some parameters for reverb and phaser here. 
Each effect has multiple lines within this class to comment out for disabling purposes.
There are also multiple sections throughout this same file where uncommenting/commenting out
code can produce different audio effects - These are undocumented: use at own risk!!

**Licensing information available in LICENSE.txt**
