#pragma once
#include "Engine/Engine.h"
#include "Game/Sudoku.h"

class Game : public Engine {
private:
    Sudoku puzzle;
    int xSel=0, ySel=0;
    
    void DrawValues();
public:
    Game(int width, int height) : Engine(width, height, "Sudoku") { }

    virtual bool Create();
    virtual bool Update(int key, Engine::keyState state);
    virtual void Destroy();
};