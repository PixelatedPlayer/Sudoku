#pragma once

#include <string>
#include <vector>
#include <stdarg.h> //va list

#include "Graphics\Frame.h"

enum LOG_LEVEL{
    logINFO,
    logWARNING,
    logERROR,
    logCRITICAL
};

class Log {
private:
    struct LogNode {
        LOG_LEVEL level;
        std::string msg;
    };

    std::vector<LogNode> log;
    static std::string Format(std::string str, va_list argv);
public:
    static Log Debug;

    Log();
    void Info(std::string msg, ...);
    void Warn(std::string msg, ...);
    void Error(std::string msg, ...);
    void Critical(std::string msg, ...);
        
    void Display(Frame* console, LOG_LEVEL lvl = logINFO, int xOffset = 0, int yOffset = 0, bool ignoreLast = false);
    void ToFile(std::string file, LOG_LEVEL lvl = logINFO);
};