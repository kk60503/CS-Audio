# Brina Luna

A synth plugin meant to serve as my introduction to the JUCE framework/software.

Originally intended to be a phaser audio plugin, the project pivoted toward using
a JUCE tutorial for DSP and reverse engineering it to implement and tweak a phaser 
effect. Reverb and distortion effects were added subsequently.

The standalone plugin has (currently) only been tested by building with Xtools and 
briefly using an old synth connected via USB for external MIDI. The project should
be able to be opened and built with (Microsoft) VS Code and a Linux Makefile.
While it may be possible to build the plugin as VST3, AU, etc., please note that
the plugin has only been built and tested as a standalone plugin (currently).

NOTE: It is assumed that the user has installed the JUCE software.

**For building the plugin:**</br>
Please install JUCE software and familiarize with the Projucer application.
The Projucer will allow the user to choose which option (Xtools/VS Code/Makefile)
to edit and build the synth with. User should be able to build and open the 
standalone plugin from their chosen editor/environment.


**Licensing information available in LICENSE.txt**
