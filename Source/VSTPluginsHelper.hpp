#pragma once

#include <JuceHeader.h>
#include <sstream>
#include "DebugTools.h"

class VSTPluginsHelper
{
public:
    VSTPluginsHelper() {};

    // Load a VST3 plugin by file path
    AudioPluginInstance* getPluginWithPath(const String& pluginPath)
    {
        DebugTools::log("Trying to load plugin from: " + pluginPath);
        
        File vst3File(pluginPath);
        if (!vst3File.exists())
        {
            DebugTools::log("File not found: " + pluginPath);
            return nullptr;
        }
        
        AudioPluginFormatManager pluginFormatManager;
        juce::addHeadlessDefaultFormatsToManager(pluginFormatManager);
        
        // Find VST3 format
        AudioPluginFormat* vst3Format = nullptr;
        for (int i = 0; i < pluginFormatManager.getNumFormats(); ++i)
        {
            if (pluginFormatManager.getFormat(i)->getName() == "VST3")
            {
                vst3Format = pluginFormatManager.getFormat(i);
                break;
            }
        }
        
        if (vst3Format == nullptr)
        {
            DebugTools::log("VST3 format not available");
            return nullptr;
        }
        
        // Get plugin descriptions
        OwnedArray<PluginDescription> descs;
        vst3Format->findAllTypesForFile(descs, pluginPath);
        
        if (descs.isEmpty())
        {
            DebugTools::log("No plugins found in: " + pluginPath);
            return nullptr;
        }
        
        // Try to create the first instrument plugin
        for (int i = 0; i < descs.size(); ++i)
        {
            if (descs[i]->isInstrument)
            {
                String msg;
                std::unique_ptr<AudioPluginInstance> instance = pluginFormatManager.createPluginInstance(*descs[i], 44100, 512, msg);
                
                if (instance != nullptr)
                {
                    DebugTools::log("Plugin loaded successfully: " + descs[i]->name);
                    return instance.release();
                }
                else
                {
                    DebugTools::log("Failed to load: " + msg);
                }
            }
        }
        
        DebugTools::log("No instrument plugin found");
        return nullptr;
    }
};
