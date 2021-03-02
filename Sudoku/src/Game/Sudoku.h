#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
using namespace std; // editor's note: don't do this


/*
 TODO:
 * Difficulty settings
 * Difficulty changes seed
 * More intelligent number removing
 * Optimize number removing (worst case takes about 2 minutes per square? VERY BAD but possibly unavoidable)
 */

class SudokuPuzzle {
    
public: // Should every variable and method be public? Probably not. Is that going to stop me? Definitely not.
    
    int sudokuSolved[81] = {}; // Complete sudoku with every square filled
    int sudokuUser[81] = {}; // The sudoku the user sees, with the numbers in certain squares removed
    
    // INTERACTIONS
    bool notes[81][9] = {}; // User entered notes
    int Get(int x, int y){
        return sudokuUser[x+y*9];
    }
    int GetHint(int x, int y, int z){
        return notes[x+y*9][z];
    }
    void Set(int x, int y, int v) { sudokuUser[x+y*9] = v; }
    void SetHint(int x, int y, int z, bool v) { notes[x+y*9][z] = v; }
    
    
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
    
    void removeSquares() {
        //Remove all possible squares while maintaining unique solution
        //To generate a minimal sudoku puzzle
        
        vector<int> squares;
        for (int i = 0; i < 81; i++) squares.push_back(i);
        random_shuffle(squares.begin(), squares.end());

        
        // NOTE: This is bad. 50 should be changed to 81, but the higher the number is, the more likely the program grinds to a halt.
        // A much less naive approach is necessary for harder puzzles.
        // Or, perhaps, a max amount of time per square removed
        for (int s = 0; s < 50; s++) {
            //cout << "sanity check\n";
            int i = squares[s];
            // Remove the randomly selected square
            int n = sudokuUser[i];
            sudokuUser[i] = 0;

            // If square is necessary for a unique solution, fill the number back in.
            //cout << "Num Solutions:" << numSolutions(sudoku, 1000000, 0) << '\n';
            //cout << "trying index " << i << '\n';
            //int num = numSolutions(sudoku, 2, 0);
            //cout << "Num Solutions: " << num << '\n';
            if (multipleSolutions(0) >= 2) {
                sudokuUser[i] = n;
                //cout << "nah this aint it\n";
            }
            //break;
        }
    }
    
    
    void createSudoku() {
        // Create a random solved sudoku puzzle
        //int sudoku [81] = {};
        
        for (int i = 0; i < 81; i++) sudokuSolved[i] = 0;

        fillInSudoku(0);

        for (int i = 0; i < 81; i++) sudokuUser[i] = sudokuSolved[i];
        
        removeSquares();
    }
};

//int main() {
//    clock_t t, cur, maxT;
//    int iterations = 1;
//    
//    SudokuPuzzle sud;
//    //srand(69420);
//    srand(time(NULL));
//    
//    t = clock();
//    cur = t;
//    maxT = t;
//    for (int n = 0; n < iterations; n++) {
//        //for (int i = 0; i < 81; i++) sudoku[i] = 0;
//        //createSudoku(sudoku);
//        sud.createSudoku();
//        maxT = max(maxT, clock() - cur);
//        cur = clock();
//    }
//    t = clock() -  t;
//    sud.printPuzzle();
//    cout << "Time elapsed: " << ((float)t)/CLOCKS_PER_SEC/iterations*1000 << " ms\n";
//    cout << "Max time: " << ((float)maxT)/CLOCKS_PER_SEC*1000 << " ms\n";
//    // On average, creating a solved sudoku takes ~0.3187 ms to execute. Not bad.
//    // The entire createSudoku function takes more like 35 ms, and that's with the function nerfed so it doesn't check everything.
//    // Worst case so far has been 1700 ms :/
//    return 0;
//}

//class Sudoku {
//private:
//    bool hints[9][9][9] = {}; //x, y, n-hint; initialize to 0
//    char board[9][9];
//    char solution[9][9];
//public:
//    Sudoku(){
//       for (int i = 0; i < 9; i++){
//           for (int j = 0; j < 9; j++){
//               board[i][j] = ' ';
//           }
//       } 
//    }
//    bool GetHint(int x, int y, int z){
//        return hints[x][y][z];
//    }
//    
//    char Get(int x, int y){
//        return board[x][y];
//    }
//    
//    void Set(int x, int y, char v) { board[x][y] = v; }
//    void SetHint(int x, int y, int z, bool v) { hints[x][y][z] = v; }
//};