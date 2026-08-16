/*
 ================================================================================
 AU-VST-Bridge: ProcessorEditor - Auto-shows Kontakt 8 editor
 ================================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MainProcessor.h"

//==============================================================================
class ProcessorEditor : public AudioProcessorEditor
{
public:
    ProcessorEditor (MainProcessor&);
    ~ProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    MainProcessor& processor_;
    AudioProcessorEditor* pluginEditor_ = nullptr;

    void setupUi();
    void checkAndShowPluginEditor();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorEditor)
};
