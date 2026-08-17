/*
 ================================================================================
 AU-VST-Bridge: Plugin Processor - Auto-loading Kontakt 8 with threaded loading
 ================================================================================
*/

#include "MainProcessor.h"
#include "ProcessorEditor.h"
#include "VSTPluginsHelper.hpp"
#include "LoadingThread.h"

//==============================================================================
MainProcessor::MainProcessor()
     : AudioProcessor(
         BusesProperties()
            .withInput("Input", AudioChannelSet::stereo(), true)
            .withOutput("Output", AudioChannelSet::stereo(), true)
       )
{
    juce::addHeadlessDefaultFormatsToManager(formatManager_);
}

MainProcessor::~MainProcessor()
{
    if (loadingThread_)
    {
        loadingThread_->stopThread(5000);
    }
}

//==============================================================================
void MainProcessor::loadKontakt8()
{
    if (kontaktLoadingAttempted.exchange(true))
        return;
    
    // Start loading on a background thread to avoid blocking Logic
    loadingThread_ = std::make_unique<LoadingThread>(*this);
    loadingThread_->startThread();
}

void MainProcessor::loadKontakt8Sync()
{
    const String kontaktPath = "/Library/Audio/Plug-Ins/VST3/Kontakt 8.vst3";
    File vst3File(kontaktPath);
    
    if (!vst3File.exists())
    {
        loadingError = "Kontakt 8 VST3 not found at " + kontaktPath;
        return;
    }
    
    // Find VST3 format
    AudioPluginFormat* vst3Format = nullptr;
    for (int i = 0; i < formatManager_.getNumFormats(); ++i)
    {
        if (formatManager_.getFormat(i)->getName() == "VST3")
        {
            vst3Format = formatManager_.getFormat(i);
            break;
        }
    }
    
    if (vst3Format == nullptr)
    {
        loadingError = "VST3 format not available";
        return;
    }
    
    OwnedArray<PluginDescription> descs;
    vst3Format->findAllTypesForFile(descs, kontaktPath);
    
    if (descs.isEmpty())
    {
        loadingError = "No VST3 plugins found in Kontakt 8";
        return;
    }
    
    for (int i = 0; i < descs.size(); ++i)
    {
        if (descs[i]->isInstrument)
        {
            String msg;
            std::unique_ptr<AudioPluginInstance> instance = formatManager_.createPluginInstance(*descs[i], 44100.0, 512, msg);
            
            if (instance != nullptr)
            {
                pluginInstance_ = std::move(instance);
                pluginLoaded = true;
            }
            else
            {
                loadingError = "Failed to load Kontakt 8: " + msg;
            }
            return;
        }
    }
    
    loadingError = "No instrument VST3 found in Kontakt 8";
}

void MainProcessor::finishLoading()
{
    const double sampleRate = getSampleRate() > 0 ? getSampleRate() : 44100.0;
    const int blockSize = getBlockSize() > 0 ? getBlockSize() : 512;
    
    if (pluginInstance_)
    {
        pluginInstance_->setRateAndBufferSizeDetails(sampleRate, blockSize);
        pluginInstance_->prepareToPlay(sampleRate, blockSize);
    }
    
    // Notify the editor to refresh
    if (AudioProcessorEditor* editor = getActiveEditor(); editor != nullptr)
    {
        editor->repaint();
    }
}

//==============================================================================
bool MainProcessor::isLoading() const
{
    return kontaktLoadingAttempted.load() && !pluginLoaded.load() && !loadErrorOccurred.load();
}

String MainProcessor::getLoadingStatus() const
{
    if (pluginLoaded.load())
        return "Loaded";
    if (loadErrorOccurred.load())
        return loadingError;
    if (kontaktLoadingAttempted.load())
        return "Loading Kontakt 8...";
    return "Ready";
}

//==============================================================================
const String MainProcessor::getName() const
{
    return "Kontakt 8 AU Bridge";
}

bool MainProcessor::acceptsMidi() const
{
    return true;
}

bool MainProcessor::producesMidi() const
{
    return true;
}

double MainProcessor::getTailLengthSeconds() const
{
    if (pluginInstance_)
        return pluginInstance_->getTailLengthSeconds();
    return 0.0;
}

int MainProcessor::getNumPrograms()
{
    if (pluginInstance_)
        return jmax(1, pluginInstance_->getNumPrograms());
    return 1;
}

int MainProcessor::getCurrentProgram()
{
    if (pluginInstance_)
        return pluginInstance_->getCurrentProgram();
    return 0;
}

void MainProcessor::setCurrentProgram (int index)
{
    if (pluginInstance_)
        pluginInstance_->setCurrentProgram(index);
}

const String MainProcessor::getProgramName (int index)
{
    if (pluginInstance_)
        return pluginInstance_->getProgramName(index);
    return String{};
}

void MainProcessor::changeProgramName (int index, const String& newName)
{
    if (pluginInstance_)
        pluginInstance_->changeProgramName(index, newName);
}

//==============================================================================
void MainProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (!kontaktLoadingAttempted.load())
        loadKontakt8();
    
    if (pluginInstance_)
    {
        pluginInstance_->setRateAndBufferSizeDetails(sampleRate, samplesPerBlock);
        pluginInstance_->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void MainProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (pluginInstance_)
    {
        pluginInstance_->processBlock(buffer, midiMessages);
    }
}

void MainProcessor::releaseResources()
{
    if (pluginInstance_)
        pluginInstance_->releaseResources();
}

bool MainProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannels() == layouts.getMainOutputChannels();
}

//==============================================================================
bool MainProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* MainProcessor::createEditor()
{
    return new ProcessorEditor(*this);
}

//==============================================================================
void MainProcessor::getStateInformation (MemoryBlock& destData)
{
    MemoryOutputStream st(destData, true);
    
    if (pluginInstance_ && pluginLoaded) 
    {
        st.writeByte(1);
        MemoryBlock pluginData;
        pluginInstance_->getStateInformation(pluginData);
        st.write(pluginData.begin(), (size_t)pluginData.getSize());
    }
    else
    {
        st.writeByte(0);
    }
}

void MainProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    MemoryInputStream st(data, sizeInBytes, false);
    
    bool pluginInitialised = (st.readByte() == 0 ? false : true);
    
    if (pluginInitialised) 
    {
        MemoryBlock pluginData;
        st.readIntoMemoryBlock(pluginData);
        
        if (!pluginInstance_)
        {
            VSTPluginsHelper vstPluginsHelper;
            AudioPluginInstance* instance = vstPluginsHelper.getPluginWithPath("/Library/Audio/Plug-Ins/VST3/Kontakt 8.vst3");
            if (instance != nullptr)
                pluginInstance_.reset(instance);
        }
        
        if (pluginInstance_ && pluginData.getSize() > 0) {
            pluginInstance_->setStateInformation(pluginData.getData(), (int)pluginData.getSize());
        }
    }
    
    suspendProcessing(false);
}

//==============================================================================
void MainProcessor::setPluginInstance(AudioPluginInstance* instance)
{
    pluginInstance_.reset(instance);
}

AudioPluginInstance* MainProcessor::getPluginInstance() const
{
    return pluginInstance_.get();
}

//==============================================================================
void MainProcessor::setCurrentEditorDimension(std::pair<int,int> dimension)
{
    editorsDimension_ = dimension;
}

std::pair<int, int> MainProcessor::getCurrentEditorDimension()
{
    return editorsDimension_;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MainProcessor();
}
