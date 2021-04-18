#pragma once
#include "Sudoku.h"

class Controller{
private:
    Sudoku puzzle; // sudoku object
    
    bool canContinue = false; // if a save file is found, we can continue a game
    bool begun = false; //if the main menu has been left and the puzzle has been started
    const char* fileName = "Save.bin"; //save and load location
    float elapsedTime = 0.0f; //the elapsed time of the puzzle
public:
    Controller();
    
    bool CanContinue() { return canContinue; } //returns if the puzzle can be continued
    bool Begun() { return begun; } //returns if the puzzle has begun
    Sudoku::hint RequestHint() { return puzzle.RequestHint(); } //returns a hint for the puzzle
    void FillNotes() { puzzle.FillNotes(); } //fills the puzzle notes for hints
    bool IsStarting(int x, int y) { return puzzle.IsStarting(x, y); } //returns if the indicated location is a starting location
    bool IsComplete() { return puzzle.IsComplete(); }
    int Get(int x, int y){ return puzzle.Get(x, y); } //returns the value at the specified location
    int GetNote(int x, int y, int z){ return puzzle.GetNote(x, y, z); } //returns the value of the specified note at location
    void IncrementTime(float deltaTime) { elapsedTime += deltaTime; }
    float GetTime() { return elapsedTime; }
    coord GetCoordFromHouseIndex(house house, int i, int j) { return puzzle.GetCoordFromHouseIndex(house, i, j); }
    std::vector<coord> MarkErrors(int x, int y, int v) { return puzzle.MarkErrors(x, y, v); }
    
    void Set(int x, int y, int v); //sets the value at the specified location
    void SetNote(int x, int y, int z, bool v); //sets the note at the specified location
    
    void Load(); //loads the puzzle
    void NewGame(); //generates a puzzle
    void DeleteSave(); //delete the save game, only done when completed
};