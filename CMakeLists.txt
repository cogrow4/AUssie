cmake_minimum_required(VERSION 3.22)

project(AuVstBridge VERSION 1.0.5)

# Add JUCE
add_subdirectory(/tmp/JUCE /tmp/JUCE-build)

juce_add_plugin(AuVstBridge
    JUCE_PROJECT_NAME "AU-VST-Bridge"
    PLUGIN_MANUFACTURER_CODE KV8B
    PLUGIN_CODE K8AU
    FORMATS AU
    PRODUCT_NAME "Kontakt 8 AU Bridge"
    COMPANY_NAME "cogrow4"
    COMPANY_WEBSITE "https://github.com/cogrow4/AUssie"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    NEEDS_PROCESSOR TRUE
    NEEDS_EDITOR TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VERSION 1.0.5
    BUNDLE_ID "com.cogrow4.kontakt8-aubridge"
)

# Generate JuceHeader
juce_generate_juce_header(AuVstBridge)

# Source files
target_sources(AuVstBridge
    PRIVATE
        Source/MainProcessor.cpp
        Source/MainProcessor.h
        Source/ProcessorEditor.cpp
        Source/ProcessorEditor.h
        Source/VSTPluginsHelper.hpp
        Source/DebugTools.h
)

# Compile definitions - matching the jucer project settings
target_compile_definitions(AuVstBridge
    PRIVATE
        JucePlugin_Name="Kontakt 8 AU Bridge"
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_PLUGINHOST_VST3=1
        JUCE_PLUGINHOST_VST=0
        JUCE_PLUGINHOST_AU=1
        JucePlugin_IsSynth=1
        JucePlugin_WantsMidiInput=1
        JucePlugin_ProducesMidiOutput=1
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        KONTAKT_VST3_PATH="/Library/Audio/Plug-Ins/VST3/Kontakt 8.vst3"
)

# JUCE modules - using non-headless modules for UI support
target_link_libraries(AuVstBridge
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_data_structures
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

# macOS specific frameworks
if(APPLE)
    find_library(APPKIT_FRAMEWORK AppKit)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    find_library(COREMIDI_FRAMEWORK CoreMIDI)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore)
    find_library(AUDIOUNIT_FRAMEWORK AudioUnit)
    find_library(COREAUDIO_FRAMEWORK CoreAudio)
    target_link_libraries(AuVstBridge PRIVATE 
        ${APPKIT_FRAMEWORK} 
        ${AUDIOTOOLBOX_FRAMEWORK} 
        ${COREMIDI_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
        ${QUARTZCORE_FRAMEWORK}
        ${AUDIOUNIT_FRAMEWORK}
        ${COREAUDIO_FRAMEWORK}
    )
endif()
