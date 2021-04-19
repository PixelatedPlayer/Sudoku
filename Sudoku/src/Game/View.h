/* View.h
 * Chris Schlenker, Mark Welch, Braxton Mott
 * This file describes a class which extends Engine. It implements all console 
 * and interacts with a Controller object to display the sudoku puzzle and
 * provide a medium for input. 
*/

#pragma once
#include "Engine/Engine.h"
#include "Game/Controller.h"
#include "Game/Sudoku.h"

//TODO multithread to begin game creation while in main menu, update process

class View : public Engine {
private:
    Controller controller; //controller - puzzle updates and requests go through here.
    Frame* gameMenu; //visual frame to draw game menu for legend and settings controls
    Frame* debug; //visual frame to draw debug info
    Frame* loadingMenu; //visual frame to draw main menu with options for continue, new game and exit
    Sudoku::hint hint; //last requested hint, stored for display
    
    const int LOADING_MENU_X = 18;
    const int LOADING_MENU_Y = 5;
    
    int xSel=0, ySel=0, mSel=0; //selection integers, xSel and ySel for the board and mSel for the menu
    bool incomplete = true;
    bool boardSelection = true; //if false we are in the menu
    bool notesOn = false; //if true, display notes on board instead of values
    bool hintToggle = false; //if true, draw hint
    bool difficultySelection = false; //we are selecting the game difficulty
    bool showDebug = false;
    
    //USER SETTINGS
    bool checkerboard = true; //if true, draw a checkerboard for hints
    bool noteToggles = true; //if true, shift toggles notes; if false shift must be held
    
    std::vector<coord> obstructions;
    float validateTimer = 0.0f;
    
    //HELPER FUNCTIONS
    void DrawBoard(); //Draw the board and values
    void DrawNotes(); //Draw notes
    void DrawBorders(); //Draw borders
    void DrawGameMenu(); //Draw Game menu and legend
    void DrawMenu(); //Draw main menu
    void DrawHints(Sudoku::hint hints); //Draw hints
    void Victory();
    void ClearBoard(); //Clear Board (only within borders, Engine::Clear clears whole console)
public:
    
    //INHERITED FUNCTIONS
    View(int width, int height) : Engine(width, height, "Sudoku") { }   //Construct console

    virtual bool Create(); //Console initialization
    virtual void HandleInput(int key, Engine::keyState state); //Handle console input
    virtual bool Update(float deltaTime); //Frame tick, deltaTime is time since last tick
    ~View();
};