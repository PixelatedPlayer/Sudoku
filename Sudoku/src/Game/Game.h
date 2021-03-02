#pragma once
#include "Engine/Engine.h"
#include "Game/Sudoku.h"

class Game : public Engine {
private:
    SudokuPuzzle puzzle;
    Frame* legend;
    
    int xSel=0, ySel=0, lSel=0;
    //user customizations
    bool checkerboard = true; //not functional
    bool hintToggles = true;
    bool hintOn = false; //only used when hintToggles is true
    bool alwaysHints = false; //not functional
    
    
    void DrawBoard();
    void DrawHints();
    void Clear();
public:
    Game(int width, int height) : Engine(width, height, "Sudoku") { }

    virtual bool Create();
    virtual bool Update(int key, Engine::keyState state);
    virtual void Destroy();
};