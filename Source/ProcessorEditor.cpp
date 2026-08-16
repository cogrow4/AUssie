/*
 ================================================================================
 AU-VST-Bridge: ProcessorEditor - Auto-shows Kontakt 8 editor
 ================================================================================
*/

#include "ProcessorEditor.h"
#include "VSTPluginsHelper.hpp"

#define SPACE_BETWEEN_COMPONENTS 20

//==============================================================================
ProcessorEditor::ProcessorEditor (MainProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setupUi();
    
    if (processor_.getPluginInstance() != nullptr)
    {
        pluginEditor_ = processor_.getPluginInstance()->createEditorIfNeeded();
        if (pluginEditor_ != nullptr)
        {
            addAndMakeVisible(pluginEditor_);
            
            auto bc = pluginEditor_->getConstrainer();
            int w = 1000, h = 700;
            if (bc != nullptr)
            {
                w = bc->getMinimumWidth();
                h = bc->getMinimumHeight();
            }
            pluginEditor_->setBounds(0, 0, w, h);
            setSize(w, h);
            
            if (pluginEditor_->isResizable())
            {
                setResizable(true, true);
            }
        }
    }
    
    if (pluginEditor_ == nullptr)
    {
        setSize(500, 300);
    }
}

void ProcessorEditor::setupUi()
{
    setOpaque(true);
}

ProcessorEditor::~ProcessorEditor()
{
    if (pluginEditor_ != nullptr) {
        delete pluginEditor_;
    }
}

//==============================================================================
void ProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void ProcessorEditor::resized()
{
}
