#include <fstream>
#include "Graphics/Colors.h"
#include "Log.h"

Log Log::Debug;

Log::Log() {
    std::vector<LogNode> log;
}

//this returns formatted string using vsnprintf, but of appropriate size. might not be worth the effort?
std::string Log::Format(std::string fmt, va_list argv) {
    int size = (int) fmt.size() + 20;
    std::string str;
    while (true) {
        str.resize(size);
        int n = vsnprintf((char *) str.data(), size, fmt.c_str(), argv);
        if (n > -1 && n < size) { // Everything worked, remove overhead
            str.resize(n);
            return str;
        }
        //string big enough?
        if (n > -1) //1 more space (\0)
            size = n + 1;
        else // > 1
            size *= 2;
    }
    return str;
}

void Log::Display(Frame* frame, LOG_LEVEL lvl, int xOffset, int yOffset, bool ignoreLast) {
    int y = yOffset;
	int start = log.size() - frame->GetHeight() + 1 + ignoreLast;
	if (start < 0)
		start = 0;

    for (int i = start; i < log.size(); i++){
        if (log[i].level >= lvl) {
            std::string out;
            char col;
            switch (log[i].level) {
                case logCRITICAL:
                    out = "CRITICAL: ";
                    col = BG_BLACK + FG_DARK_RED;
                    break;
                case logERROR:
                    out = "ERROR: ";
                    col = BG_BLACK + FG_DARK_RED;
                    break;
                case logWARNING:
                    out = "WARNING: ";
                    col = BG_BLACK + FG_YELLOW;
                    break;
                case logINFO:
                    out = "INFO: ";
                    col = BG_BLACK + FG_WHITE;
                    break;
            }
            out += log[i].msg;
            frame->DrawString(xOffset, y++, out, col);
        }
    }
}

void Log::Info(std::string msg, ...) {
    va_list argv;
    va_start(argv, msg);

    LogNode node = {logINFO, Format(msg, argv)};
    log.push_back(node);

    va_end(argv);
}

void Log::Warn(std::string msg, ...) {
    va_list argv;
    va_start(argv, msg);

    LogNode node = {logWARNING, Format(msg, argv)};
    log.push_back(node);

    va_end(argv);
}

void Log::Error(std::string msg, ...) {
    va_list argv;
    va_start(argv, msg);

    LogNode node = {logERROR, Format(msg, argv)};
    log.push_back(node);

    va_end(argv);
}

void Log::Critical(std::string msg, ...) {
    va_list argv;
    va_start(argv, msg);

    LogNode node = {logCRITICAL, Format(msg, argv)};
    log.push_back(node);

    va_end(argv);
}

void Log::ToFile(std::string file, LOG_LEVEL lvl) {
    std::ofstream f;
    f.open(file);

    for (LogNode node : log) {
        if (node.level >= lvl) {
            std::string out;
            switch (node.level) {
                case logCRITICAL:
                    out = "CRITICAL: ";
                    break;
                case logERROR:
                    out = "ERROR: ";
                    break;
                case logWARNING:
                    out = "WARNING: ";
                    break;
                case logINFO:
                    out = "INFO: ";
                    break;
            }

            out += node.msg;
            f << out << std::endl;
        }
    }
}