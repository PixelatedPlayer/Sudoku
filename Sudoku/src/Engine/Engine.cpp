
#include <cstdio>

#include "Engine.h"
#include "Log.h"
#ifdef REAL_TIME
#include <chrono>
#endif

Engine::Engine(int width, int height, std::string name){
    screenWidth = width;
    screenHeight = height;
    
    console = GetStdHandle(STD_OUTPUT_HANDLE);

    // initialize key data
    for (int i = 0; i < 256; i++) {
        keyNewState[i] = 0;
        keyOldState[i] = 0;
        keys[i].pressed = false;
        keys[i].released = false;
        keys[i].held = false;
    }
    
    this->name = name;
    SetConsoleTitle(name.c_str());
    
    // CONSTRUCT CONSOLE
    
    if (console == INVALID_HANDLE_VALUE) {
        Log::Debug.Critical("CORE: Invalid Handle");
        return;
    }
    
    window = {0, 0, 1, 1};
    SetConsoleWindowInfo(console, TRUE, &window);

    //Set Window Style
    HWND hwnd = GetConsoleWindow();
    if (!SetWindowPos(hwnd, 0, 20, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER))
        Log::Debug.Warn("CORE: Could not set console position");
    long dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    //SET STYLES - ^ is exclusive or (bit), so it reverses the bit
    dwStyle ^= WS_MAXIMIZEBOX; //disable maximize button
    dwStyle ^= WS_SIZEBOX; //disable border size change

    SetWindowLong(hwnd, GWL_STYLE, dwStyle);
	
    // set size of screen buffer
    COORD coord = {(short)screenWidth, (short)screenHeight};
    if (!SetConsoleScreenBufferSize(console, coord))
        Log::Debug.Error("CORE: SetConsoleScreenBufferSize");

    if (!SetConsoleActiveScreenBuffer(console)) {
        Log::Debug.Critical("CORE: SetConsoleActiveScreenBuffer");
        return;
    }

    // is desired window size within max?
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(console, &csbi)) {
        Log::Debug.Critical("CORE: GetConsoleScreenBufferInfo");
        return;
    }
    if (screenHeight > csbi.dwMaximumWindowSize.Y) {
        Log::Debug.Critical("CORE: Height too large");
        return;
    }
    if (screenWidth > csbi.dwMaximumWindowSize.X) {
        Log::Debug.Critical("CORE: Width too large");
        return;
    }

    // set window size
    window = {0, 0, (short)(screenWidth - 1), (short)(screenHeight - 1)};
    if (!SetConsoleWindowInfo(console, TRUE, &window)){
        Log::Debug.Critical("CORE: SetConsoleWindowInfo");
        return;
    }

    // create screen buffer
    screenBuffer = new CHAR_INFO[screenWidth * screenHeight];
    for (int i = 0; i < screenWidth * screenHeight; i++) {
        screenBuffer[i].Char.AsciiChar = 0;
        screenBuffer[i].Attributes = 0;
    }

    // disable cursor (blinking) - consider having a toggle function for implementation control
    CONSOLE_CURSOR_INFO cursor;
    GetConsoleCursorInfo(console, &cursor);
    cursor.bVisible = false;
    SetConsoleCursorInfo(console, &cursor);
}

void Engine::SetBit(int x, int y, char glyph, char col, bool transparent) {
    if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
        char c = col;
        if (transparent)//fg only
            c = (screenBuffer[x + y * screenWidth].Attributes / 16) * 16 + col;
        
        screenBuffer[x + y * screenWidth].Char.AsciiChar = glyph;
        screenBuffer[x + y * screenWidth].Attributes = c;
    }
}

void Engine::Fill(int x1, int y1, int x2, int y2, char glyph, char col) {
    if (x1 < 0)
        x1 = 0;
    else if (x1 > screenWidth)
        x1 = screenWidth;

    if (y1 < 0)
        y1 = 0;
    else if (y1 > screenHeight)
        y1 = screenHeight;

    if (x2 < 0)
        x2 = 0;
    else if (x2 > screenWidth)
        x2 = screenWidth;

    if (y2 < 0)
        y2 = 0;
    else if (y2 > screenHeight)
        y2 = screenHeight;

    for (int x = x1; x < x2; x++) {
        for (int y = y1; y < y2; y++)
            SetBit(x, y, glyph, col);
    }
}

void Engine::DrawString(int x, int y, std::string str, char col, bool transparent) {
    if (y >= screenHeight || y < 0)
        return;
    //consider word wrap - will automatically letter wrap
    for (unsigned int i = 0; i < str.size(); i++) {
        SetBit(x + i, y, str[i], col, transparent);
    }
}

void Engine::DrawFrame(int x, int y, Frame* frame) {
    if (frame == nullptr){
        Log::Debug.Error("Attempted to draw invalid frame");
        return;
    }

    for (int i = 0; i < frame->GetWidth(); i++) {
        for (int j = 0; j < frame->GetHeight(); j++) {
            SetBit(x + i, y + j, frame->GetGlyph(i, j), frame->GetColor(i, j));
        }
    }
}

void Engine::Start() {
    if (!Create())
        running = false;

#ifdef REAL_TIME
    auto tp1 = std::chrono::system_clock::now(); // time point 1
    auto tp2 = std::chrono::system_clock::now(); // time point 2
    float perSec = 1; //1 so is set in first frame
#else
    //initial draw, since we won't draw until we update
    WriteConsoleOutput(console, screenBuffer,{ (short)screenWidth, (short)screenHeight }, { 0, 0 }, &window);
#endif
    
    while (running) {
        if (GetForegroundWindow() != GetConsoleWindow())
            continue;
#ifdef REAL_TIME
        //DETERMINE TIMING
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> duration = tp2 - tp1;
        tp1 = tp2;
        float deltaTime = duration.count();
        perSec += duration.count();
#endif  

        for (int i = 0; i < 256; i++) {
            keyNewState[i] = GetAsyncKeyState(i);

            keys[i].pressed = false;
            keys[i].released = false;

            if (keyNewState[i] != keyOldState[i]) {
                if (keyNewState[i]) {
                    keys[i].pressed = !keys[i].held;
                    keys[i].held = true;
                } else {
                    keys[i].released = true;
                    keys[i].held = false;
                }
            }

            if (keyNewState[i] != keyOldState[i]){
#ifndef REAL_TIME
                if (!Update(i, keys[i]))
                    running = false;
                //Screen buffer: only update on key input for non-realtime game
                WriteConsoleOutput(console, screenBuffer,{ (short)screenWidth, (short)screenHeight }, { 0, 0 }, &window);
#else
                HandleInput(i, keys[i]);
#endif
            }
            keyOldState[i] = keyNewState[i];
        }

#ifdef REAL_TIME
        // Update
        if (!Update(deltaTime))
            running = false;

        //Title FPS
        if (perSec >= 1){
            char s[256];
            snprintf(s, 256, "%s - FPS: %4.0f", name.c_str(), 1.0f / deltaTime);
            SetConsoleTitle(s);
            perSec = 0;
       }
        //Screen buffer
        WriteConsoleOutput(console, screenBuffer,{ (short)screenWidth, (short)screenHeight }, { 0, 0 }, &window);
#endif
    }

    //only heap memory in core
    delete[] screenBuffer;
}