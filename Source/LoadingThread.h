/*
 ================================================================================
 LoadingThread: Background thread for loading VST3 plugins
 ================================================================================
*/

#pragma once

#include <JuceHeader.h>

class MainProcessor;

class LoadingThread : public Thread
{
public:
    LoadingThread(MainProcessor& owner)
        : Thread("Kontakt8Loader"), owner_(owner) {}

    void run() override
    {
        owner_.loadKontakt8Sync();
        owner_.finishLoading();
    }

private:
    MainProcessor& owner_;
};
