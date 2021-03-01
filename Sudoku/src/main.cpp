//OPTIONAL DEF's
//MAX_SIZE maximizes console size to the screen. pretty sure this grabs the biggest monitor in a dual-monitor system
//REAL_TIME if you want to update outside of keyboard input, in which case the update function is
//  Update(float deltaTime); with an additional HandleInput(int key, keyState state)
//  in keyboard input it's instead Update(int key, keyState state);

/*TODO
 * Hints show when holding shift and bg is changed
 * 
 * 
 * 
 */

#include <cstdlib>
#include <iostream>
#include "Game/Game.h"
#include "Engine/Log.h"

using namespace std;

int main(int argc, char** argv) {
    COORD size;
    int w=80;
    int h=40;
    size = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));

#ifdef MAX_SIZE
    //can handle this all differently for different resolutions
    //can change ratio from 2:1 to 16:9
    //or have an array of predetermined options, cycling through until valid
    int dec = 10; //start above 0 to guarantee some space for window borders
    float ratioX = 3;
    float ratioY = 1;
    
    do {
        w = size.X - dec;
        h = w * (ratioY/ratioX);
        dec++;
    } while (h > size.Y);

    Log::Debug.Info("Console Size Determined: %d, %d", w, h);

    if (w > 0 && h > 0) {
#else
    if (w < size.X && h < size.Y) {
#endif
        Game game(w, h);
        game.Start();
    } else {
        Log::Debug.Critical("Game cannot run on your maximum console size. Please reduce your console font size or play on a greater resolution monitor.");
        std::cout << "Game cannot run on your maximum console size. Please reduce your console font size or play on a greater resolution monitor.";
        std::cin.get();
    }
    return 0;
}

