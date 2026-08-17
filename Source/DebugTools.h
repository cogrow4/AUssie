#pragma once

#include <iostream>
#include <sstream>
#include <string>

#ifdef _MSC_VER
#include <Windows.h>
#endif

class DebugTools {
private:
    DebugTools() {}

public:
    static void log(std::stringstream msg) {
        msg << std::endl;
        log(msg.str());
    };

    static void log(const char *msg) {
#ifdef _MSC_VER
        OutputDebugStringA(std::string(msg) + "\n");
#else
        std::cout << msg << std::endl;
#endif
    };

    static void log(const std::string& msg) {
        log(msg.c_str());
    };

    // Overload for juce::String
    static void log(const juce::String& msg) {
        log(msg.toStdString().c_str());
    };
};