#pragma once
#include "Engine/Engine.h"
#include "Game/Sudoku.h"

class View : public Engine {
private:
    SudokuPuzzle puzzle; //sudoku object
    Frame* legend; //visual frame pointer to draw legend
    Frame* debug; //visual frame pointer to draw debug info
    Frame* menu;
    
    int xSel=0, ySel=0, mSel=0; //selection integers, xSel and ySel for the board and mSel for the menu
    float totalTime = 0; //running time; updated in Update and used to track time records
    SudokuPuzzle::hint hint; //last requested hint
    bool boardSelection = true; //if false we are in the menu
    bool autoRemove = true; //if true, we auto remove notes when marking
    bool notesOn = false; //if true, display notes on board instead of values
    bool hintToggle = false; //if true, draw hint
    
    
    //USER SETTINGS
    bool checkerboard = true; //if true, draw a checkerboard for hints
    bool noteToggles = true; //if true, shift toggles notes; if false shift must be held
    bool invertNotes = false; //not yet functional
    
    bool begun = false; //marks if we've left the main menu
    bool canContinue = false;  //marks if a saved game was discovered
    
    
    //HELPER FUNCTIONS
    void DrawBoard(); //Draw the board and values
    void DrawNotes(); //Draw notes
    void DrawBorders(); //Draw borders
    void DrawLegend(); //Draw Legend
    void DrawMenu();
    void DrawHints(SudokuPuzzle::hint hints); //Draw hints
    void Clear(); //Clear Board (only within borders, Engine::Clear clears whole console)
    void RequestHint(); //Request a hint from puzzle
public:
    
    //INHERITED FUNCTIONS
    View(int width, int height) : Engine(width, height, "Sudoku") { }   //Construct console

    virtual bool Create(); //Console initialization
    virtual void HandleInput(int key, Engine::keyState state); //Handle console input
    virtual bool Update(float deltaTime); //Frame tick, deltaTime is time since last tick
    virtual void Destroy(); //Called on destruction; necessary for console close events (multithreaded)
};