# AUssie - Kontakt 8 AU Bridge

A VST3-to-AUv2 plugin wrapper (bridge) that allows using Native Instruments Kontakt 8 (VST3) inside AU-only DAWs like Logic Pro.

## Overview

AUssie is a JUCE-based Audiounit plugin that auto-loads the Kontakt 8 VST3 plugin and embeds its editor UI within the AUv2 plugin window. It allows you to use Kontakt 8 in Logic Pro (which only supports AU plugins natively) without needing a commercial wrapper solution.

## Features

- Auto-loading of Kontakt 8 VST3 (no manual plugin selection)
- Threaded loading to prevent DAW hangs during plugin initialization
- Embedded Kontakt 8 editor UI (no separate floating window)
- MIDI pass-through to the hosted VST3 plugin
- Audio passthrough (stereo in/out)

## Requirements

- macOS 12+
- Logic Pro (or any AUv2-compatible DAW)
- Native Instruments Kontakt 8 (VST3 version) installed at:
  `/Library/Audio/Plug-Ins/VST3/Kontakt 8.vst3`
- JUCE 8+ (project uses the CMake build system with JUCE submodule)

## Building from Source

### Prerequisites

- Xcode Command Line Tools (`xcode-select --install`)
- CMake 3.22+
- JUCE submodule (or set `JUCE_CMAKE_SUBMODULE_PATH` to your JUCE install)

### Build Instructions

```bash
git clone https://github.com/cogrow4/AUssie.git
cd AUssie

# Initialize JUCE submodule (or set JUCE_CMAKE_SUBMODULE_PATH)
git submodule update --init --recursive

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Install (copies the component to ~/Library/Audio/Plug-Ins/Components/)
cmake --install build --config Release
```

## Usage

1. Restart Logic Pro (or any AU host) after installing the plugin
2. Create a new Software Instrument track
3. Select "Kontakt 8 AU Bridge" from the instrument menu
4. The plugin will show "Loading Kontakt 8..." while initializing
5. Once loaded, the Kontakt 8 interface appears automatically
6. Use Kontakt 8 normally (load libraries, instruments, etc.)

## How It Works

AUssie acts as a bridge/wrapper:
1. The AUv2 plugin is recognized by Logic Pro as a standard instrument
2. In its constructor, the plugin creates a background thread to load the Kontakt 8 VST3 plugin using JUCE's `AudioPluginFormatManager`
3. The editor displays a "Loading..." message while waiting for the background thread to complete
4. Once loaded, the Kontakt 8 editor is embedded (using `createEditorIfNeeded()`) inside the AUv2 editor window
5. Audio and MIDI data are passed through to the hosted VST3 plugin

This approach uses synchronous plugin loading (`createPluginInstance`) but runs it on a background thread to avoid blocking the host's message loop, which was the cause of the original "loading kontakt 8" hang.

## Project Structure

```
AUssie/
├── Source/
│   ├── MainProcessor.h/cpp    # Core AUv2 plugin processor (auto-loads Kontakt 8)
│   ├── ProcessorEditor.h/cpp  # Editor with threaded loading indicator
│   ├── LoadingThread.h        # Background thread for non-blocking VST3 loading
│   ├── VSTPluginsHelper.hpp   # Plugin discovery/loading utilities
│   └── DebugTools.h           # Debug logging utilities
├── CMakeLists.txt             # CMake build configuration
├── AuVstBridge.jucer          # Original Projucer file (from upstream)
└── README.md
```

## Based on AuVstBridge

This project is derived from [TristanIsrael/AuVstBridge](https://github.com/TristanIsrael/AuVstBridge) (MIT licensed) but has been significantly upgraded:

- Simplified to single-plugin auto-loading (removed combobox selector)
- Added threaded background loading to prevent DAW hangs
- Modernized to work with JUCE master branch (CMake)
- Embedded editor UI instead of separate window

## License

MIT License - see LICENSE file.

## Credits

- Based on AuVstBridge by TristanIsrael
- Uses JUCE framework (GPL/Commercial)
- Native Instruments Kontakt 8
