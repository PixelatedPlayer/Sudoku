#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include "Engine/Log.h"
using namespace std; // editor's note: don't do this


/*
 TODO:
 * AISolve using RequestHint
 * Difficulty settings
 * Difficulty changes seed
 * More intelligent number removing
 * Optimize number removing (worst case takes about 2 minutes per square? VERY BAD but possibly unavoidable)
 */

class SudokuPuzzle {
private:
    bool AISolve(int startingPuzzle[81]){
        //puzzle is copied, not reference so we can modify to solve and return false if we cant
        bool notes[81][9];
        int puzzle[81];
        for (int i = 0; i < 81; i++){
            puzzle[i] = startingPuzzle[i];
        }
        FillNotes(puzzle, notes);
        hint hint = RequestHint(puzzle, notes);
        while (hint.type != ERR){
            //perform hint on puzzle
            switch(hint.type){
                case mark:
                    puzzle[hint.x + hint.y*9] = hint.z+1;
                    FillNotes(puzzle, notes); //TODO this scans the whole puzzle when we only need to scan where we marked
                    break;
            }
            
            if (IsComplete(puzzle))
                return true;

            hint = RequestHint(puzzle, notes);
        }
        return false;
    }
    
    static int GetXFromI(int i){ return i % 9; }
    static int GetYFromI(int i){ return i / 9; }
public: // Should every variable and method be public? Probably not. Is that going to stop me? Definitely not.
    
    int sudokuSolved[81] = {}; // Complete sudoku with every square filled
    int sudokuUser[81] = {}; // The sudoku the user sees, with the numbers in certain squares removed
    
    bool sudokuStarting[81] = {};
    // INTERACTIONS
    bool notes[81][9] = {}; // User entered notes
    bool aiNotes[81][9] = {};
    int Get(int x, int y){
        return sudokuUser[x+y*9];
    }
    int GetNote(int x, int y, int z){
        return notes[x+y*9][z];
    }
    void Set(int x, int y, int v) { sudokuUser[x+y*9] = v; }
    void SetNote(int x, int y, int z, bool v) { notes[x+y*9][z] = v; }
    bool IsStarting(int x, int y){
        return sudokuStarting[x + y * 9];
    }
    
    struct coord{
        coord(int x, int y) : x(x), y(y) {}
        int x, y;
    };

    enum hintType{
        mark, //mark x,y with z
        remove, //remove hint z at x,y
        ERR
    };
    struct hint{
        hint(int x, int y, int z, hintType type) : x(x), y(y), z(z), type(type) {}
        hint(int i, int z, hintType type): x(GetXFromI(i)), y(GetYFromI(i)), z(z), type(type) {}
        hint(): x(-1), y(-1), z(-1), type(ERR) {}
        int x, y, z;
        hintType type;
    };
    
    bool IsComplete(int puzzle[81]){
        for (int i = 0; i < 81; i++){
            if (puzzle[i] == 0)
                return false;
        }
        return true;
    }
    
    vector<coord> MarkErrors(int x, int y, int v){
        vector<coord> obstructions;
        //Check rows
        for(int r=0; r<9;r++) {
            if (r!=y&&sudokuUser[x+r*9]==v)
              obstructions.push_back(coord(x,r));
            }
        //Check columns
        for(int c=0;c<9;c++){
            if(c!=x&&sudokuUser[c+y*9]==v)
              obstructions.push_back(coord(c,y));
        }
        
        //Check block
        int startX = (x/3)*3;
        int startY = (y/3)*3;
        for (int xx = 0; xx < 3; xx++){
            for (int yy = 0; yy < 3; yy++){
                int tx = startX+xx;
                int ty = startY+yy;
                if ((tx!=x||ty!=y)&&sudokuUser[tx+ty*9]==v)
                    obstructions.push_back(coord(tx,ty));
            }
        }
        return obstructions;
    }
    
    hint RequestHint(){
        return RequestHint(sudokuUser, notes);
    }
    
    hint RequestHint(int puzzle[81], bool notes[81][9]){
        //SCAN SINGLE NOTES - entry with only one note
        for (int i = 0; i < 81; i++){
            if (puzzle[i] != 0) continue;
            int n = 0;
            int h;
            for (int j = 0; j < 9; j++){
                if (notes[i][j]){
                    n++;
                    h = j;
                }
            }
            if (n==1) return hint(i, h, mark);
        }
       
        //SCAN LONE NOTE - for a column/row/block with only one of any value
        //scan each row
        for (int y = 0; y < 9; y++){
            for (int n = 0; n < 9; n++){
                int num = 0;
                int i = -1;
                for (int x = 0; x < 9; x++){
                    if (puzzle[x + y * 9] != 0) continue;
                    if (notes[x + y * 9][n]) {
                        num++;
                        i = x;
                    }
                }
                if (num == 1)
                    return hint(i, y, n, mark);
            }
        }
        //scan each col
        for (int x = 0; x < 9; x++){
            for (int n = 0; n < 9; n++){
                int num = 0;
                int i = -1;
                for (int y = 0; y < 9; y++){
                    if (puzzle[x + y * 9] != 0) continue;
                    if (notes[x + y * 9][n]) {
                        num++;
                        i = y;
                    }
                }
                if (num == 1)
                    return hint(x,i,n,mark);
            }
        }
        //scan each block
        for (int x = 0; x < 3; x++)
            for (int y = 0; y < 3; y++){
                for (int n = 0; n < 9; n++){
                    int num = 0;
                    int ix = -1;
                    int iy = -1;
                    for (int yy = 0; yy < 3; yy++){
                        for (int xx = 0; xx < 3; xx++){
                            int trueX = x*3+xx;
                            int trueY = y*3+yy;
                            if (puzzle[trueX + trueY*9] != 0) continue;
                            if (notes[trueX + trueY*9][n]) {
                                num++;
                                ix = trueX;
                                iy = trueY;
                            }
                        }
                    }
                    if (num == 1)
                        return hint(ix, iy, n, mark);
                }
            }
        
        return hint();
    }
    
    void FillNotes(){
        FillNotes(sudokuUser, notes);
    }
    
    void FillNotes(int (&puzzle)[81], bool (&notes)[81][9]){
        //start by filling them, and we'll remove from there
        for (int i = 0; i < 81; i++){
            for (int j = 0; j < 9; j++)
                if (puzzle[i] == 0)
                    notes[i][j] = true;
                else
                    notes[i][j] = false;
        }
        //scan each location
        for (int x = 0; x < 9; x++){
            for (int y = 0; y < 9; y++){
                //at each location, remove it's value from the notes of the rest of its house (row/column/block){
                if (puzzle[x + y * 9] != 0) {
                    //row
                    for (int xx = 0; xx < 9; xx++)
                        notes[xx + y * 9][puzzle[x + y * 9]-1] = false;
                    //column
                    for (int yy = 0; yy < 9; yy++)
                        notes[x + yy * 9][puzzle[x + y * 9]-1] = false;
                    //block
                    int xBlockStart = (x / 3)*3;
                    int yBlockStart = (y / 3)*3;
                    for (int xx = 0; xx < 3; xx++){
                        for (int yy = 0; yy < 3; yy++){
                            notes[xBlockStart+xx+(yBlockStart+yy)*9][puzzle[x+y*9]-1] = false;
                        }
                    }
                }
            }
        }
    }
    
    void printPuzzle() {
        // Prints sudokuUser as a 9x9 character printout, like this:
        // ..3.24...
        // 87..3.2.4
        // .9487.6..
        // .5......8
        // ....1.5..
        // 4.....19.
        // .189..3..
        // 6..283..1
        // .3....9..

        for (int y = 0; y < 9; y++) {
            for (int x = 0; x < 9; x++) {
                int n = sudokuUser[x + y*9];
                if (n > 0) cout << n;
                else cout << '.'; //empty spaces are replaced with dots
            }
            cout << '\n';
        }
    }
    
    
    bool possible(int *sudoku, int x, int y, int n) {
        //Given a sudoku puzzle, a square position, and a number value,
        //Determine whether that value can legally be placed in that square
        //Weird behavior if the square is already filled in
        
        //get position of box
        int boxX = x/3 * 3;
        int boxY = y/3 * 3;

        for (int i = 0; i < 9; i++) {
            if (sudoku[i + y*9] == n) return false; // check for number in row
            if (sudoku[x + i*9] == n) return false; // check for number in column
            if (sudoku[i % 3 + boxX + (i/3 + boxY) * 9] == n) return false; // check for number in box
        }
        return true;
    }
    bool possible(int *sudoku, int i, int n) {
        // function overload using an index rather than an xy position
        return possible(sudoku, i % 9, i / 9, n);
    }
    
    
    bool fillInSudoku(int start) {
        // Recursive function which naively fills in random numbers and backtracks if an impossible puzzle is created
        // Each call of the function fills in a single empty square.
        // Returns true iff all squares are successfully filled.
        
        for (int i = start; i < 81; i++) {
            if (sudokuSolved[i] == 0) { // Check for empty squares
                // Randomly shuffle a list of possible values and check each if they're possible
                vector<int> possibles;
                for (int j = 1; j < 10; j++) possibles.push_back(j);
                random_shuffle(possibles.begin(), possibles.end());
                //for (int j = 0; j < 9; j++) cout << possibles[j] << "\n";
                for (int j = 0; j < 9; j++) {
                    int n = possibles[j];
                    if (possible(sudokuSolved, i, n)) {
                        sudokuSolved[i] = n; // Naively fill in a random possible number
                        //cout << i % 9 << ", " << i / 9 << ": " << n << '\n';
                        if (fillInSudoku(start+1)) return true; // Fill in remaining squares recursively to find valid puzzle.
                        // TODO: in solveSudoku, you can set sudoku[i] to 0 right before 'return true' so that you don't modify the original sudoku
                    }
                }
                // This square cannot be filled with a number, and the puzzle is therefore impossible to solve.
                // Erase work on the square and backtrack.
                sudokuSolved[i] = 0;
                return false;
            }
        }
        return true;
    }
    
    
//    int numSolutions(int cap, int start) { // UNUSED
//        // Returns the number of solutions to the sudoku
//        // Calculation will stop if number equals cap
//        
//        int num = 0;
//        int emptySpaces = 0;
//        vector<int> spaces;
//        for (int i = 0; i < 81; i++) {
//            if (sudokuUser[i] == 0) {
//                emptySpaces++;
//                spaces.push_back(i);
//                for (int n = 1; n <= 9; n++) {
//                    if (possible(sudokuUser, i, n)) {
//                        sudokuUser[i] = n; // Naively fill in a random possible number
//                        num += numSolutions(cap, i+1); // See how many solutions exist in this branch
//                        if (num - emptySpaces >= cap) {
//                            for (int j: spaces) sudokuUser[j] = 0; // Erase our guesses
//                            return cap; //stop computations here, we've found the max allotted number of solutions
//                        }
//                        // TODO: in solveSudoku, you can set sudoku[i] to 0 right before 'return true' so that you don't modify the original sudoku
//                    }
//                }
//                for (int j: spaces) sudokuUser[j] = 0; // Erase our guesses
//                return min(num + 1 - emptySpaces, cap);
//            }
//        }
//        for (int j: spaces) sudokuUser[j] = 0; // Erase our guesses
//        return min(num + 1 - emptySpaces, cap);
//    }
    
    
    int multipleSolutions(int start) {
        // Returns the number of solutions to the sudoku
        // Calculation will stop if number equals cap
        
        bool hasSolution = false;
        for (int i = 0; i < 81; i++) {
            if (sudokuUser[i] == 0) {
                for (int n = 1; n <= 9; n++) {
                    if (possible(sudokuUser, i, n)) {
                        sudokuUser[i] = n; // Naively fill in a random possible number
                        int branchSols = multipleSolutions(i+1);
                        if (branchSols > 1) {
                            sudokuUser[i] = 0;
                            return 2;
                        }
                        if (branchSols == 1) { // See how many solutions exist in this branch
                            sudokuUser[i] = 0;
                            if (hasSolution) return 2;
                            hasSolution = true;
                        }
                    }
                }
                sudokuUser[i] = 0;
                return 0;
            }
        }
        return 1;
    }
    
    bool removeSquares() {
        //Remove all possible squares while maintaining unique solution
        //To generate a minimal sudoku puzzle
        
        //int totalAttempts = 0;
        int attempts = 0;
        int successfulRemoves = 0;
        int goalRemoves = 27; //true removals is double this (-1 if the center was removed, bc it's mirror is itself) TODO increase this as the AI improves
        vector<int> squares;
        for (int i = 0; i < 81; i++) squares.push_back(i);
        random_shuffle(squares.begin(), squares.end());

        int iteration = 0;
        while (iteration < 81) {
            //cout << "sanity check\n";
            int i = squares[iteration++];
            int mirror = 80-i;
            // Remove the randomly selected square
            int n = sudokuUser[i];
            int o = sudokuUser[mirror];
            if (n == 0) //we already removed from here
                continue;
            sudokuUser[i] = 0;
            sudokuUser[mirror]=0;

            // If square is necessary for a unique solution, fill the number back in.
            //cout << "Num Solutions:" << numSolutions(sudoku, 1000000, 0) << '\n';
            //cout << "trying index " << i << '\n';
            //int num = numSolutions(sudoku, 2, 0);
            //cout << "Num Solutions: " << num << '\n';
//            int num = multipleSolutions(0);
//            Log::Debug.Info("Remove iteration: %d | Num Solutions: %d", s, num);
//            if (num >= 2) {
//                sudokuUser[i] = n;
//                //cout << "nah this aint it\n";
//            }
            //break;
            if (!AISolve(sudokuUser)){
                sudokuUser[i] = n;
                sudokuUser[mirror] = o;
                attempts++;
                //totalAttempts++;
            } else {
                //attempts = 0;
                successfulRemoves++;
                if (successfulRemoves == goalRemoves){
                    Log::Debug.Info("Puzzle removal completed, total backsteps: %d", attempts);
                    return true;
                }
            }
        }
        return false;
    }
    
    
    
    void createSudoku() {
        // Create a random solved sudoku puzzle
        //int sudoku [81] = {};
        
        int attempt = 0;
        do {   
            attempt ++;
            for (int i = 0; i < 81; i++) sudokuSolved[i] = 0;

            fillInSudoku(0);

            for (int i = 0; i < 81; i++) sudokuUser[i] = sudokuSolved[i];
        } while (!removeSquares());
        
        Log::Debug.Info("Puzzle selected, attempt #%d", attempt);
        for (int i = 0; i < 81; i++)
            sudokuStarting[i] = sudokuUser[i] != 0;
    }
};
