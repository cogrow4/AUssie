/*
 ================================================================================
 AU-VST-Bridge: ProcessorEditor - Threaded loading with UI feedback
 ================================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MainProcessor.h"

//==============================================================================
class ProcessorEditor : public AudioProcessorEditor,
                        public Timer
{
public:
    ProcessorEditor (MainProcessor&);
    ~ProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    MainProcessor& processor_;
    AudioProcessorEditor* pluginEditor_ = nullptr;
    
    void createPluginEditor();
    void checkPluginEditor();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorEditor)
};
