/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:            Brina Luna Synth
 version:          1.0.0
 vendor:           Fern Hands
 website:          http://juce.com
 description:      Explores the DSP module by implementing phaser, reverb and distortion effects.
            Based on a JUCE DSP tutorial.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures, juce_dsp,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             AudioProcessor
 mainClass:        BrinaLunaAudioProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
template <typename Type>
class CustomOscillator
{
public:
    //==============================================================================
    CustomOscillator() {
        auto& osc = processorChain.template get<oscIndex>();      
        osc.initialise ([] (Type x) { return std::sin (x); }, 128);
    }

    //==============================================================================
    void setFrequency (Type newValue, bool force = false)
    {
        auto& osc = processorChain.template get<oscIndex>();
        osc.setFrequency (newValue, force);     // [7]
    }

    //==============================================================================
    void setLevel (Type newValue)
    {
        auto& gain = processorChain.template get<gainIndex>();
        gain.setGainLinear (newValue);          // [8]
    }

    //==============================================================================
    void reset() noexcept {}

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        processorChain.process (context);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        processorChain.prepare (spec); // [3]
    }

private:
    //==============================================================================
    enum
    {
        oscIndex,
        gainIndex   // [2]
    };
 
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<Type>, juce::dsp::Gain<Type>> processorChain; // [1]
};

//==============================================================================
template <typename Type>
class Distortion
{
public:
    //==============================================================================
    Distortion()
    {
        auto& waveshaper = processorChain.template get<waveshaperIndex>();         // [5]
        waveshaper.functionToUse = [] (Type x)
                                   {
                                       // soft
//                                       return std::tanh (x);
//                                       // hard
//                                       return juce::jlimit (Type (-0.1), Type (0.1), x); // [6]
                                       // fuzz?
                                       return juce::jlimit (Type (-0.65), Type (0.65), (std::tanh ((std::tanh (x)))));
                                   };

       auto& preGain = processorChain.template get<preGainIndex>();   // [5]
       preGain.setGainDecibels (50.0f);                               // [6]

       auto& postGain = processorChain.template get<postGainIndex>(); // [7]
       postGain.setGainDecibels (-12.0f);                             // [8]
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        auto& filter = processorChain.template get<filterIndex>();                      // [3]
        filter.state = FilterCoefs::makeFirstOrderHighPass (spec.sampleRate, 4000.0f);  // [4]
 
        processorChain.prepare (spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        processorChain.process (context); // [7]
    }

    //==============================================================================
    void reset() noexcept 
    {
        processorChain.reset();
    }

private:
    //==============================================================================
    enum
    {
        filterIndex,        // [2]
        preGainIndex,    // [2]
        waveshaperIndex,
        postGainIndex    // [3]
    };
    using Filter = juce::dsp::IIR::Filter<Type>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<Type>;
 
    juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
                              juce::dsp::Gain<Type>, juce::dsp::WaveShaper<Type>, juce::dsp::Gain<Type>> processorChain;
};

//==============================================================================
class Voice  : public juce::MPESynthesiserVoice
{
public:
    Voice()
    {
        auto& masterGain = processorChain.get<masterGainIndex>();
        masterGain.setGainLinear (0.2f);
        
        // Ladder Filter
        //auto& filter = processorChain.get<filterIndex>();
        //filter.setCutoffFrequencyHz (1000.0f);          // [3]
        //filter.setResonance (0.7f);
        
        // Second LFO
        //lfo.initialise ([] (float x) { return std::sin(x); }, 128);
        //lfo.setFrequency (3.0f);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
        processorChain.prepare (spec);
        
        // Second LFO
        //lfo.prepare ({ spec.sampleRate / lfoUpdateRate, spec.maximumBlockSize, spec.numChannels }); // [4]
        
    }

    //==============================================================================
    void noteStarted() override
    {
        auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
        auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();

        processorChain.get<osc1Index>().setFrequency (freqHz, true);
        processorChain.get<osc1Index>().setLevel (velocity);

        processorChain.get<osc2Index>().setFrequency (freqHz * 1.01f, true);    // [3]
        processorChain.get<osc2Index>().setLevel (velocity);
    }

    //==============================================================================
    void notePitchbendChanged() override
    {
        auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();
        processorChain.get<osc1Index>().setFrequency (freqHz);
        processorChain.get<osc2Index>().setFrequency (freqHz * 1.01f);
    }

    //==============================================================================
    void noteStopped (bool) override
    {
        clearCurrentNote();
    }

    //==============================================================================
    void notePressureChanged() override {}
    void noteTimbreChanged()   override {}
    void noteKeyStateChanged() override {}

    //==============================================================================
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        auto output = tempBlock.getSubBlock (0, (size_t) numSamples);
        output.clear();
 
        for (size_t pos = 0; pos < (size_t) numSamples;)
        {
            auto max = juce::jmin ((size_t) numSamples - pos, lfoUpdateCounter);
            auto block = output.getSubBlock (pos, max);
 
            juce::dsp::ProcessContextReplacing<float> context (block);
            processorChain.process (context);
 
            pos += max;
            lfoUpdateCounter -= max;
 
            if (lfoUpdateCounter == 0)
            {
                lfoUpdateCounter = lfoUpdateRate;
                
                // Second LFO
                //auto lfoOut = lfo.processSample (0.0f);                                 // [5]
                //auto curoffFreqHz = juce::jmap (lfoOut, -1.0f, 1.0f, 100.0f, 2000.0f);  // [6]
                //processorChain.get<filterIndex>().setCutoffFrequencyHz (curoffFreqHz);  // [7]
            }
        }

        juce::dsp::AudioBlock<float> (outputBuffer)
            .getSubBlock ((size_t) startSample, (size_t) numSamples)
            .add (tempBlock);
    }

private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        osc1Index,
        osc2Index,
        
        // Ladder Filter
        // filterIndex,
        
        //waveshaperIndex,
        masterGainIndex
    };

    juce::dsp::ProcessorChain<CustomOscillator<float>, CustomOscillator<float>,
           /* juce::dsp::WaveShaper<float>,*/ juce::dsp::Gain<float>> processorChain;

    static constexpr size_t lfoUpdateRate = 100;
    size_t lfoUpdateCounter = lfoUpdateRate;
    
    // Second LFO
    //juce::dsp::Oscillator<float> lfo;   // [1]
};

//==============================================================================
class AudioEngine  : public juce::MPESynthesiser
{
public:
    static constexpr auto maxNumVoices = 4;

    //==============================================================================
    AudioEngine()
    {
        for (auto i = 0; i < maxNumVoices; ++i)
            addVoice (new Voice);

        setVoiceStealingEnabled (true);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate (spec.sampleRate);

        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare (spec);
        
        juce::dsp::Reverb::Parameters reverbParameters;
        reverbParameters.dryLevel = 0.2;
        reverbParameters.wetLevel = 0.1;
        reverbParameters.roomSize = 0.9; // 0 small to 1 big
        reverbParameters.damping = 0.9; // 0…1 1=damped
        reverbParameters.width = 0.9; // 0…1 1=very wide
        reverbParameters.freezeMode = 0.4; // 0…1 1=frrrrozzenn
        reverb.setParameters(reverbParameters);
        
        reverb.prepare(spec);
        
        //fxChain.prepare (spec); //reverb
        
        BLPhaser.prepare(spec);
        
        /** Sets the rate (in Hz) of the LFO modulating the phaser all-pass filters. This
            rate must be lower than 100 Hz.
        */
        BLPhaser.setRate(1);
    
        /** Sets the volume (between 0 and 1) of the LFO modulating the phaser all-pass
            filters.
        */
        BLPhaser.setDepth(0.2);
    
        /** Sets the centre frequency (in Hz) of the phaser all-pass filters modulation.
        */
        BLPhaser.setCentreFrequency(300);
    
        /** Sets the feedback volume (between -1 and 1) of the phaser. Negative can be
            used to get specific phaser sounds.
        */
        BLPhaser.setFeedback(-0.2);
    
        /** Sets the amount of dry and wet signal in the output of the phaser (between 0
            for full dry and 1 for full wet).
        */
        BLPhaser.setMix(0.5);
        
        disto1.prepare(spec);
    }

private:
    //==============================================================================
    void renderNextSubBlock (juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        MPESynthesiser::renderNextSubBlock (outputAudio, startSample, numSamples);
        
        auto block = juce::dsp::AudioBlock<float> (outputAudio);
        auto blockToUse = block.getSubBlock ((size_t) startSample, (size_t) numSamples);
        auto contextToUse = juce::dsp::ProcessContextReplacing<float> (blockToUse);
        
        reverb.process(contextToUse);
        //fxChain.process (contextToUse);// fxChainreverb
        BLPhaser.process(contextToUse);
        disto1.process(contextToUse);
    }
//    
//     fxChain reverb
//    enum
//    {
//        reverbIndex // reverb
//    };
 
  
//    juce::dsp::ProcessorChain<juce::dsp::Reverb> fxChain; // fxChainreverb
    juce::dsp::Reverb reverb;
    juce::dsp::Phaser<float> BLPhaser;
    Distortion<float> disto1;
};

//==============================================================================
template <typename SampleType>
class AudioBufferQueue
{
public:
    //==============================================================================
    static constexpr size_t order = 9;
    static constexpr size_t bufferSize = 1U << order;
    static constexpr size_t numBuffers = 5;

    //==============================================================================
    void push (const SampleType* dataToPush, size_t numSamples)
    {
        jassert (numSamples <= bufferSize);

        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite (1, start1, size1, start2, size2);

        jassert (size1 <= 1);
        jassert (size2 == 0);

        if (size1 > 0)
            juce::FloatVectorOperations::copy (buffers[(size_t) start1].data(), dataToPush, (int) juce::jmin (bufferSize, numSamples));

        abstractFifo.finishedWrite (size1);
    }

    //==============================================================================
    void pop (SampleType* outputBuffer)
    {
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead (1, start1, size1, start2, size2);

        jassert (size1 <= 1);
        jassert (size2 == 0);

        if (size1 > 0)
            juce::FloatVectorOperations::copy (outputBuffer, buffers[(size_t) start1].data(), (int) bufferSize);

        abstractFifo.finishedRead (size1);
    }

private:
    //==============================================================================
    juce::AbstractFifo abstractFifo { numBuffers };
    std::array<std::array<SampleType, bufferSize>, numBuffers> buffers;
};

//==============================================================================
template <typename SampleType>
class ScopeDataCollector
{
public:
    //==============================================================================
    ScopeDataCollector (AudioBufferQueue<SampleType>& queueToUse)
        : audioBufferQueue (queueToUse)
    {}

    //==============================================================================
    void process (const SampleType* data, size_t numSamples)
    {
        size_t index = 0;

        if (state == State::waitingForTrigger)
        {
            while (index++ < numSamples)
            {
                auto currentSample = *data++;

                if (currentSample >= triggerLevel && prevSample < triggerLevel)
                {
                    numCollected = 0;
                    state = State::collecting;
                    break;
                }

                prevSample = currentSample;
            }
        }

        if (state == State::collecting)
        {
            while (index++ < numSamples)
            {
                buffer[numCollected++] = *data++;

                if (numCollected == buffer.size())
                {
                    audioBufferQueue.push (buffer.data(), buffer.size());
                    state = State::waitingForTrigger;
                    prevSample = SampleType (100);
                    break;
                }
            }
        }
    }

private:
    //==============================================================================
    AudioBufferQueue<SampleType>& audioBufferQueue;
    std::array<SampleType, AudioBufferQueue<SampleType>::bufferSize> buffer;
    size_t numCollected;
    SampleType prevSample = SampleType (100);

    static constexpr auto triggerLevel = SampleType (0.05);

    enum class State { waitingForTrigger, collecting } state { State::waitingForTrigger };
};

//==============================================================================
template <typename SampleType>
class ScopeComponent  : public juce::Component,
                        private juce::Timer
{
public:
    using Queue = AudioBufferQueue<SampleType>;

    //==============================================================================
    ScopeComponent (Queue& queueToUse)
        : audioBufferQueue (queueToUse)
    {
        sampleData.fill (SampleType (0));
        setFramesPerSecond (30);
    }

    //==============================================================================
    void setFramesPerSecond (int framesPerSecond)
    {
        jassert (framesPerSecond > 0 && framesPerSecond < 1000);
        startTimerHz (framesPerSecond);
    }

    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        g.setColour (juce::Colours::white);

        auto area = getLocalBounds();
        auto h = (SampleType) area.getHeight();
        auto w = (SampleType) area.getWidth();

        // Oscilloscope
        auto scopeRect = juce::Rectangle<SampleType> { SampleType (0), SampleType (0), w, h / 2 };
        plot (sampleData.data(), sampleData.size(), g, scopeRect, SampleType (1), h / 4);

        // Spectrum
        auto spectrumRect = juce::Rectangle<SampleType> { SampleType (0), h / 2, w, h / 2 };
        plot (spectrumData.data(), spectrumData.size() / 4, g, spectrumRect);
    }

    //==============================================================================
    void resized() override {}

private:
    //==============================================================================
    Queue& audioBufferQueue;
    std::array<SampleType, Queue::bufferSize> sampleData;

    juce::dsp::FFT fft { Queue::order };
    using WindowFun = juce::dsp::WindowingFunction<SampleType>;
    WindowFun windowFun { (size_t) fft.getSize(), WindowFun::hann };
    std::array<SampleType, 2 * Queue::bufferSize> spectrumData;

    //==============================================================================
    void timerCallback() override
    {
        audioBufferQueue.pop (sampleData.data());
        juce::FloatVectorOperations::copy (spectrumData.data(), sampleData.data(), (int) sampleData.size());

        auto fftSize = (size_t) fft.getSize();

        jassert (spectrumData.size() == 2 * fftSize);
        windowFun.multiplyWithWindowingTable (spectrumData.data(), fftSize);
        fft.performFrequencyOnlyForwardTransform (spectrumData.data());

        static constexpr auto mindB = SampleType (-160);
        static constexpr auto maxdB = SampleType (0);

        for (auto& s : spectrumData)
            s = juce::jmap (juce::jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (s) - juce::Decibels::gainToDecibels (SampleType (fftSize))), mindB, maxdB, SampleType (0), SampleType (1));

        repaint();
    }

    //==============================================================================
    static void plot (const SampleType* data,
                      size_t numSamples,
                      juce::Graphics& g,
                      juce::Rectangle<SampleType> rect,
                      SampleType scaler = SampleType (1),
                      SampleType offset = SampleType (0))
    {
        auto w = rect.getWidth();
        auto h = rect.getHeight();
        auto right = rect.getRight();

        auto center = rect.getBottom() - offset;
        auto gain = h * scaler;

        for (size_t i = 1; i < numSamples; ++i)
            g.drawLine ({ juce::jmap (SampleType (i - 1), SampleType (0), SampleType (numSamples - 1), SampleType (right - w), SampleType (right)),
                          center - gain * data[i - 1],
                          juce::jmap (SampleType (i), SampleType (0), SampleType (numSamples - 1), SampleType (right - w), SampleType (right)),
                          center - gain * data[i] });
    }
};

//==============================================================================
class BrinaLunaAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    BrinaLunaAudioProcessor()
         : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    {}

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        audioEngine.prepare ({ sampleRate, (juce::uint32) samplesPerBlock, 2 });
        midiMessageCollector.reset (sampleRate);
    }

    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
         && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        return true;
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels  = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        midiMessageCollector.removeNextBlockOfMessages (midiMessages, buffer.getNumSamples());

        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

        audioEngine.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
        scopeDataCollector.process (buffer.getReadPointer (0), (size_t) buffer.getNumSamples());
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override
    {
        return new BrinaLunaAudioProcessorEditor (*this);
    }

    bool hasEditor() const override                                        { return true; }

    //==============================================================================
    const juce::String getName() const override                            { return JucePlugin_Name; }
    bool acceptsMidi() const override                                      { return true; }
    bool producesMidi() const override                                     { return false; }
    bool isMidiEffect() const override                                     { return false; }
    double getTailLengthSeconds() const override                           { return 0.0; }

    //==============================================================================
    int getNumPrograms() override                                          { return 1; }
    int getCurrentProgram() override                                       { return 0; }
    void setCurrentProgram (int) override                                  {}
    const juce::String getProgramName (int) override                       { return {}; }
    void changeProgramName (int, const juce::String&) override             {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock&) override                 {}
    void setStateInformation (const void*, int) override                   {}

    //==============================================================================
    juce::MidiMessageCollector& getMidiMessageCollector() noexcept { return midiMessageCollector; }
    AudioBufferQueue<float>& getAudioBufferQueue() noexcept        { return audioBufferQueue; }

private:
    //==============================================================================
    class BrinaLunaAudioProcessorEditor  : public juce::AudioProcessorEditor
    {
    public:
        BrinaLunaAudioProcessorEditor (BrinaLunaAudioProcessor& p)
            : AudioProcessorEditor (&p),
              dspProcessor (p),
              scopeComponent (dspProcessor.getAudioBufferQueue())
        {
            addAndMakeVisible (midiKeyboardComponent);
            addAndMakeVisible (scopeComponent);

            setSize (800, 400);

            auto area = getLocalBounds();
            scopeComponent.setTopLeftPosition (0, 80);
            scopeComponent.setSize (area.getWidth(), area.getHeight() - 100);

            midiKeyboardComponent.setMidiChannel (2);
            midiKeyboardState.addListener (&dspProcessor.getMidiMessageCollector());
        }

        ~BrinaLunaAudioProcessorEditor() override
        {
            midiKeyboardState.removeListener (&dspProcessor.getMidiMessageCollector());
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        }

        void resized() override
        {
            auto area = getLocalBounds();
            midiKeyboardComponent.setBounds (area.removeFromTop (80).reduced (8));
        }

    private:
        //==============================================================================
        BrinaLunaAudioProcessor& dspProcessor;
        juce::MidiKeyboardState midiKeyboardState;
        juce::MidiKeyboardComponent midiKeyboardComponent { midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard };
        ScopeComponent<float> scopeComponent;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrinaLunaAudioProcessorEditor)
    };

    //==============================================================================
    AudioEngine audioEngine;
    juce::MidiMessageCollector midiMessageCollector;
    AudioBufferQueue<float> audioBufferQueue;
    ScopeDataCollector<float> scopeDataCollector { audioBufferQueue };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrinaLunaAudioProcessor)
};
