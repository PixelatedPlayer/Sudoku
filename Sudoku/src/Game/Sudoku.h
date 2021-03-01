#pragma once

class Sudoku {
private:
    bool hints[9][9][9] = {}; //x, y, n-hint; initialize to 0
    char board[9][9];
public:
    Sudoku(){
       for (int i = 0; i < 9; i++){
           for (int j = 0; j < 9; j++){
               board[i][j] = ' ';
           }
       } 
    }
    bool GetHint(int x, int y, int z){
        return hints[x][y][z];
    }
    
    char Get(int x, int y){
        return board[x][y];
    }
    
    void Set(int x, int y, char v) { board[x][y] = v; }
    void SetHint(int x, int y, int z, bool v) { hints[x][y][z] = v; }
};