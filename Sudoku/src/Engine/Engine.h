#pragma once

#include <Windows.h>
#include "Graphics/Frame.h"

/* ENGINE INFO
 * To be implemented by Game class
 * Key input (HandleInput or Update) uses VK_[key] for non-character keys
 * 
 */

class Engine {
protected:
    struct keyState {
        bool pressed;
        bool released;
        bool held;
    };
    static bool destroyed;
    
private:
    int screenWidth;
    int screenHeight;
    CHAR_INFO* screenBuffer;
    std::string name;
    HANDLE console;
    SMALL_RECT window;
    short keyOldState[256] = { 0 };
    short keyNewState[256] = { 0 };
    static bool running;

    keyState keys[256];

    static BOOL OnClose(DWORD dword);
public:
    Engine(int width, int height, std::string name); //construct the console and initialize engine variables

    void SetBit(int x, int y, char glyph = '\0', char col = FG_WHITE, bool transparent = false);
    Bit GetBit(int x, int y) { return Bit(screenBuffer[x + y * screenWidth].Char.AsciiChar, screenBuffer[x + y * screenWidth].Attributes); }
    void Fill(int x1, int y1, int x2, int y2, char glyph = '\0', char col = FG_WHITE);
    void DrawString(int x, int y, std::string str, char col = FG_WHITE, bool transparent = true);
    void DrawFrame(int x, int y, Frame* frame);
    void Clear(char col = BG_BLACK) { Fill(0, 0, screenWidth, screenHeight, '\0', col); }
    keyState GetKey(int key) { return keys[key]; }
    int GetWidth() { return screenWidth; }
    int GetHeight() { return screenHeight; }

    void Start(); // game loop

    //USER IMPLEMENTATIONS
    virtual bool Create() = 0; //initialize implementation variables
#ifdef REAL_TIME
    virtual bool Update(float deltaTime) = 0;
    virtual void HandleInput(int key, keyState state) { }
#else
    virtual bool Update(int key, keyState state) = 0;
#endif
    virtual void Destroy() { destroyed = true; } //similar to destructor - free memory from implementation variables
};