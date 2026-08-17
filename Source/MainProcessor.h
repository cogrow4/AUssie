/*
 ================================================================================
 AU-VST-Bridge: Auto-loading Kontakt 8 version
 ================================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DebugTools.h"

class ProcessorEditor;

//==============================================================================
class MainProcessor : public AudioProcessor
{
public:
    MainProcessor();
    virtual ~MainProcessor();

    //==============================================================================
    const String getName() const override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    //==============================================================================
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;
    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    //==============================================================================
    void setPluginInstance(AudioPluginInstance* instance);
    AudioPluginInstance* getPluginInstance() const;
    //==============================================================================
    void setCurrentEditorDimension(std::pair<int,int> dimension);
    std::pair<int, int> getCurrentEditorDimension();

    // Thread-safe status accessors
    bool isPluginLoaded() const { return pluginLoaded.load(); }
    bool isLoading() const;
    String getLoadingStatus() const;
    void finishLoading();

private:
    //==============================================================================
    friend class ProcessorEditor;
    friend class LoadingThread;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessor)

    void loadKontakt8();
    void loadKontakt8Sync();

    AudioPluginFormatManager formatManager_;
    std::unique_ptr<AudioPluginInstance> pluginInstance_;
    std::unique_ptr<Thread> loadingThread_;
    
    std::atomic<bool> pluginLoaded{false};
    std::atomic<bool> kontaktLoadingAttempted{false};
    std::atomic<bool> loadErrorOccurred{false};
    juce::String loadingError;
    std::pair<int, int> editorsDimension_;
};
