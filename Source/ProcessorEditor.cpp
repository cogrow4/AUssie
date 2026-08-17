/*
 ================================================================================
 AU-VST-Bridge: ProcessorEditor - Shows loading status, then embeds Kontakt 8 editor
 ================================================================================
*/

#include "ProcessorEditor.h"

//==============================================================================
ProcessorEditor::ProcessorEditor (MainProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setOpaque(true);
    setSize(400, 200);
    
    // Try to create the plugin editor immediately (might already be loaded)
    checkPluginEditor();
    
    // Poll for loading completion every 100ms
    startTimer(100);
}

void ProcessorEditor::checkPluginEditor()
{
    if (processor_.getPluginInstance() != nullptr && pluginEditor_ == nullptr)
    {
        pluginEditor_ = processor_.getPluginInstance()->createEditorIfNeeded();
        if (pluginEditor_ != nullptr)
        {
            addAndMakeVisible(pluginEditor_);
            
            // Size to the plugin editor
            auto bc = pluginEditor_->getConstrainer();
            int w = 1000, h = 700;
            if (bc != nullptr)
            {
                w = jmax(bc->getMinimumWidth(), 800);
                h = jmax(bc->getMinimumHeight(), 600);
            }
            pluginEditor_->setBounds(0, 0, w, h);
            setSize(w, h);
            
            if (pluginEditor_->isResizable())
            {
                setResizable(true, true);
                pluginEditor_->setTopLeftPosition(0, 0);
            }
        }
    }
}

void ProcessorEditor::timerCallback()
{
    if (pluginEditor_ == nullptr)
    {
        checkPluginEditor();
    }
    else
    {
        stopTimer();
    }
    
    repaint();
}

ProcessorEditor::~ProcessorEditor()
{
    stopTimer();
    if (pluginEditor_ != nullptr)
        delete pluginEditor_;
}

void ProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    
    if (pluginEditor_ == nullptr)
    {
        String statusText = "Loading Kontakt 8...";
        if (processor_.getPluginInstance() == nullptr)
        {
            statusText = "Loading Kontakt 8...";
        }
        else
        {
            statusText = "Initializing Kontakt 8...";
        }
        
        g.setColour(Colours::white);
        g.setFont(15.0f);
        g.drawText(statusText, getLocalBounds().reduced(20), 
                   Justification::centred, true);
    }
}

void ProcessorEditor::resized()
{
    if (pluginEditor_ != nullptr)
    {
        pluginEditor_->setBounds(getLocalBounds());
    }
}