#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <random>
#include <ctime>
#include "Engine/Log.h"

/** Coordinate data structure */
struct coord{
    coord(int x, int y) : x(x), y(y) {}
    coord() : x(0), y(0) {}
    int x, y;
};

/** Hint Types */
enum hintType{
    mark, //mark x,y with z
    pair, //remove hints y, z, from the house type at x, except for those who have only those two (i, j)
    //that is, at house type h: index x, mark indices i,j as only options for notes y,z: remove y,z as notes from all others in that house
    ERR
};

/** House enum (row, column or block)*/
enum house{
    row,
    col,
    block
};

/** Difficultyt enum*/
enum Difficulty {
    Easy,
    Medium,
    Hard
};

/**
 * Sudoku Model class. Contains the data and generation logic for the sudoku puzzle.
 */
class Sudoku {
private:
    //DATA
    const int INCREASED_REMOVALS_PER_DIFFICULTY = 4; /**< Number of additional removals per increased difficulty level*/
    
    char solved[81] = {}; // Complete sudoku with every square filled
    char data[81] = {}; // The sudoku the user sees, with the numbers in certain squares removed
    bool starting[81] = {};
    bool notes[81][9] = {}; // User entered notes
    float elapsedTime = 0.0f;
    bool leaderboardSave = true;
    Difficulty difficulty;
    
    //FUNCTIONS
    bool AISolve(char startingPuzzle[81], Difficulty dGoal, Difficulty& current); //Solve the puzzle - used to ensure generated puzzles are solvable
    
    static int GetXFromI(int i) { return i % 9; } //returns x location from the index (out of 81)
    static int GetYFromI(int i) { return i / 9; } //returns Y location from the index (out of 81)
public: // Should every variable and method be public? Probably not. Is that going to stop me? Definitely not.
    //DATA STRUCTURES
    // HINT DATA
    struct hint{
        hint(int x, int y, int z, hintType type, Difficulty difficulty = Easy) : x(x), y(y), z(z), type(type), difficulty(difficulty) {}
        hint(int i, int z, hintType type, Difficulty difficulty = Easy): x(GetXFromI(i)), y(GetYFromI(i)), z(z), type(type), difficulty(difficulty) {}
        hint(): x(0), y(0), z(0), type(ERR), difficulty(Easy) {}

        int x, y, z; //x, y are locations, z is note location where applicable (by type)
        int i, j, k = 0; //extra info for some hint types (pair: i, j index in house)
        house h = row; //house for hint: 0==row,1==col,2==block
        hintType type;
        Difficulty difficulty;
    };

    // DATA INTERFACTING
    int Get(int x, int y){ return data[x+y*9]; }
    int GetNote(int x, int y, int z){ return notes[x+y*9][z]; }
    void Set(int x, int y, int v) { data[x+y*9] = v; }
    void SetNote(int x, int y, int z, bool v) { notes[x+y*9][z] = v; }
    bool IsStarting(int x, int y){ return starting[x + y * 9]; }
    coord GetCoordFromHouseIndex(house house, int i, int j);
    bool IsComplete(char puzzle[81]);
    bool IsComplete() { return IsComplete(data); }
    void IncrementTime(float deltaTime) { elapsedTime += deltaTime; }
    float GetTime() { return elapsedTime; }
    void SetLeaderboardSave(bool save) { leaderboardSave = save; }
    bool GetLeadboardSave() { return leaderboardSave; }
    Difficulty GetDifficulty() { return difficulty; }
    
    std::vector<coord> MarkErrors(int x, int y, int v);
    std::vector<coord> IsValid();
    
    //AI FUNCTIONS
    //The big bad function for AI and hints
    static hint RequestHint(char puzzle[81], bool notes[81][9]);
    //Request hint from this puzzle
    hint RequestHint(){ return RequestHint(data, notes); }
    //Fill this puzzles notes
    static void FillNotes(char (&puzzle)[81], bool (&notes)[81][9]);
    void FillNotes() { FillNotes(data, notes); }
    void ClearNotesInHouses(int x, int y, int v);
    
    // FILE FUNCTIONS
    void Save(std::string fileName);
    bool Load(std::string fileName);
    bool LoadFileExists(std::string fileName);
    
    // PUZZLE CREATION FUNCTIONS
    bool Fill(int start);
    bool Possible(char *sudoku, int x, int y, int n);
    // function overload using an index rather than an xy position
    bool Possible(char *sudoku, int i, int n) { return Possible(sudoku, i % 9, i / 9, n); }
    int MultipleSolutions(int start);
    bool RemoveSquares(Difficulty difficulty);
    void CreateSudoku(Difficulty difficulty);
};