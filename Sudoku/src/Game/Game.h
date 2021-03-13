#pragma once
#include "Engine/Engine.h"
#include "Game/Sudoku.h"

class Game : public Engine {
private:
    SudokuPuzzle puzzle;
    Frame* legend;
    Frame* debug;
    
    int xSel=0, ySel=0, mSel=0;
    float totalTime = 0;
    SudokuPuzzle::hint hint;
    //user customizations
    bool boardSelection = true; //as opposed to menu selection
    bool checkerboard = true; //not functional
    bool noteToggles = true;
    bool notesOn = false; //only used when hintToggles is true
    bool hintToggle = false;
    bool invertNotes = false; //not functional
    
    
    void DrawBoard();
    void DrawNotes();
    void DrawMenu();
    void DrawHints(SudokuPuzzle::hint hints);
    void Clear();
    void RequestHint();
public:
    Game(int width, int height) : Engine(width, height, "Sudoku") { }

    virtual bool Create();
    virtual void HandleInput(int key, Engine::keyState state);
    virtual bool Update(float deltaTime);
    virtual void Destroy();
};