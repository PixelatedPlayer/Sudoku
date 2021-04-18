#include "Sudoku.h"

bool Sudoku::AISolve(char startingPuzzle[81]){        
    bool notes[81][9];
    char puzzle[81];
    for (int i = 0; i < 81; i++){
        puzzle[i] = startingPuzzle[i];
    }
    Log aiDebug;
    FillNotes(puzzle, notes);
    hint hint = RequestHint(puzzle, notes);
    while (hint.type != ERR){
        //perform hint on puzzle
        switch(hint.type){
            case mark:
            {
                aiDebug.Info("Marking at [%d,%d] with value %d", hint.x, hint.y, hint.z);
                //mark location
                puzzle[hint.x + hint.y*9] = hint.z+1;
                //remove marked value from notes in each of it's houses
                for (int x = 0; x < 9; x++){
                    notes[x + hint.y *9][hint.z] = false;
                }
                for (int y = 0; y < 9; y++){
                    notes[hint.x +y*9][hint.z] = false;
                }
                int blockX = hint.x / 3;
                int blockY = hint.y / 3;
                for (int x = 0; x < 3; x++){
                    for (int y = 0; y < 3; y++){
                        notes[(blockX * 3 + x) + (blockY * 3 + y)*9][hint.z] = false;
                    }
                }
            }
                break;
            case pair:
                //remove all other notes from the house of the pair
                switch (hint.h){
                    case row:
                        aiDebug.Info("Pair found, removing notes %d and %d from ROW %d", hint.y, hint.z, hint.x);
                        for (int x = 0; x < 9; x++){
                            if (x == hint.i || x == hint.j) continue;
                            notes[x + hint.x * 9][hint.y] = false;
                            notes[x + hint.x * 9][hint.z] = false;
                        }
                        break;
                    case col:
                        aiDebug.Info("Pair found, removing notes %d and %d from COL %d", hint.y, hint.z, hint.x);
                        for (int y = 0; y < 9; y++){
                            if (y == hint.i || y == hint.j) continue;
                            notes[hint.x + y * 9][hint.y] = false;
                            notes[hint.x + y * 9][hint.z] = false;
                        }
                        break;
                    case block:
                        aiDebug.Info("Pair found, removing notes %d and %d from BLOCK %d", hint.y, hint.z, hint.x);
                        int blockX = hint.x % 3;
                        int blockY = hint.x / 3;
                        for (int x = 0; x < 3; x++){
                            for (int y = 0; y < 3; y++){
                                if (y*3+x == hint.i || y*3+x == hint.j) continue;
                                notes[(blockX*3+x) + (blockY * 3 + y)*9][hint.y] = false;
                                notes[(blockX*3+x) + (blockY * 3 + y)*9][hint.z] = false;
                            }
                        }
                        break;
                }
                break;
        }

        if (IsComplete(puzzle)){
            aiDebug.ToFile("aiDebug.txt");
            return true;
        }

        hint = RequestHint(puzzle, notes);
    }
    aiDebug.ToFile("aiDebug.txt");
    return false;
}

coord Sudoku::GetCoordFromHouseIndex(house house, int i, int j){
    coord c;
    switch (house){
        case row:
            c.y = i;
            c.x = j;
            break;
        case col:
            c.x = i;
            c.y = j;
            break;
        case block:
            c.x = (i % 3) * 3 + j % 3;
            c.y = (i / 3) * 3 + j / 3;
            break;
    }

    return c;
}

bool Sudoku::IsComplete(char puzzle[81]){
    for (int i = 0; i < 81; i++){
        if (puzzle[i] == 0)
            return false;
    }
    return true;
}

std::vector<coord> Sudoku::MarkErrors(int x, int y, int v){
    std::vector<coord> obstructions;
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

Sudoku::hint Sudoku::RequestHint(char puzzle[81], bool notes[81][9]){
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
    for (int x = 0; x < 3; x++) {
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
                if (num == 1){
                    return hint(ix, iy, n, mark);
                }
            }
        }
    }

    //SCAN NAKED PAIRS - can't combine with above bc x,n,y vs x,y,n
    //we don't want to return a naked pair if it's already been done. so confirm that the house the naked pair is in contains at least one more of the note before returning hint
    {
        //scan each row
        std::vector<hint> twoNotes;
        for (int y = 0; y < 9; y++){
            for (int x = 0; x < 9; x++){
                if (puzzle[x + y*9] != 0) continue;
                int num = 0; //number of notes on [x,y]
                int n1 = 0; //value of first note
                int n2 = 0; //value of second note
                for (int n = 0; n < 9; n++){
                    if(notes[x + y*9][n]){
                        if (num == 0)
                            n1 = n;
                        else
                            n2 = n;
                        num++;
                    }
                }
                if (num == 2) //PAIR
                    twoNotes.push_back(hint(x,n1,n2,ERR)); //save note data in hint: x==index in house (row, so actually x), y==first note value, z==second note value
            }
            if (twoNotes.size() >= 2){
                //check if any are the same
                for (int i = 0; i < twoNotes.size(); i++){
                    for (int j = 0; j < twoNotes.size(); j++){
                        if (i == j) continue; //dont compare pairs at same location
                        if (twoNotes[i].y == twoNotes[j].y && twoNotes[i].z == twoNotes[j].z) { //same two notes
                            //check that another note of this type exists in the row
                            int numN1 = 0;
                            int numN2 = 0;
                            for (int x = 0; x < 9; x++){
                                if(notes[x + y * 9][twoNotes[i].y])
                                    numN1++;
                                if (notes[x + y * 9][twoNotes[i].z])
                                    numN2++;
                            }
                            if (numN1 > 2 || numN2 > 2){
                                hint h(y, twoNotes[i].y, twoNotes[i].z, pair);
                                h.i = twoNotes[i].x;
                                h.j = twoNotes[j].x;
                                h.h = row;
                                return h;
                            }
                        }
                    }
                }
            }
            twoNotes.clear();
        }

        //scan each col
        for (int x = 0; x < 9; x++){
            for (int y = 0; y < 9; y++){
                if (puzzle[x + y*9] != 0) continue;
                int num = 0; //number of notes on [x,y]
                int n1 = 0; //value of first note
                int n2 = 0; //value of second note
                for (int n = 0; n < 9; n++){
                    if(notes[x + y*9][n]){
                        if (num == 0)
                            n1 = n;
                        else
                            n2 = n;
                        num++;
                    }
                }
                if (num == 2) //PAIR
                    twoNotes.push_back(hint(y,n1,n2,ERR)); //save note data in hint: x==index in house (row, so actually x), y==first note value, z==second note value
            }
            if (twoNotes.size() >= 2){
                //check if any are the same
                for (int i = 0; i < twoNotes.size(); i++){
                    for (int j = 0; j < twoNotes.size(); j++){
                        if (i == j) continue; //dont compare pairs at same location
                        if (twoNotes[i].y == twoNotes[j].y && twoNotes[i].z == twoNotes[j].z) { //same two notes
                            //check that another note of this type exists in the row
                            int numN1 = 0;
                            int numN2 = 0;
                            for (int y = 0; y < 9; y++){
                                if(notes[x + y * 9][twoNotes[i].y])
                                    numN1++;
                                if (notes[x + y * 9][twoNotes[i].z])
                                    numN2++;
                            }
                            if (numN1 > 2 || numN2 > 2){
                                hint h(x, twoNotes[i].y, twoNotes[i].z, pair);
                                h.i = twoNotes[i].x;
                                h.j = twoNotes[j].x;
                                h.h = col;
                                return h;
                            }
                        }
                    }
                }
            }
            twoNotes.clear();
        }

        //scan each block
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++){
                for (int yy = 0; yy < 3; yy++){
                    for (int xx = 0; xx < 3; xx++){
                        int trueX = x*3+xx;
                        int trueY = y*3+yy;
                        if (puzzle[trueX + trueY*9] != 0) continue;
                        int num = 0; //number of notes on [trueX, trueY]
                        int n1 = 0; //value of first note
                        int n2 = 0; //value of second note
                        for (int n = 0; n < 9; n++){
                            if (notes[trueX + trueY*9][n]){
                                if (num == 0)
                                    n1 = n;
                                else
                                    n2 = n;
                                num++;
                            }
                        }
                        if (num == 2) //PAIR
                            twoNotes.push_back(hint(yy*3+xx, n1, n2, ERR));
                    }
                }
                if (twoNotes.size() >= 2){
                    //check if any are the same
                    for (int i = 0; i < twoNotes.size(); i++){
                        for (int j = 0; j < twoNotes.size(); j++){
                            if (i == j) continue; //dont compare pairs at same location
                            if (twoNotes[i].y == twoNotes[j].y && twoNotes[i].z == twoNotes[j].z) {
                                //check that another note of this type exists in the row
                                int numN1 = 0;
                                int numN2 = 0;
                                for (int xx = 0; xx < 3; xx++){
                                    for (int yy = 0; yy < 3; yy++){
                                        if(notes[(x*3+xx) + (y*3+yy) * 9][twoNotes[i].y])
                                            numN1++;
                                        if (notes[(x*3+xx) + (y*3+yy) * 9][twoNotes[i].z])
                                            numN2++;
                                    }
                                }
                                if (numN1 > 2 || numN2 > 2){
                                    hint h(y*3+x, twoNotes[i].y, twoNotes[i].z, pair);
                                    h.i = twoNotes[i].x;
                                    h.j = twoNotes[j].x;
                                    h.h = block;
                                    return h;
                                }
                            }
                        }
                    }
                }
                twoNotes.clear();
            }
        }
    }
    
    return hint(); //return empty hit means we got nothing
}

void Sudoku::ClearNotesInHouses(int x, int y, int v){
    //clear col
    for (int yy = 0; yy < 9; yy++){
        notes[x + yy * 9][v] = false;
    }
    
    //clear row
    for (int xx = 0; xx < 9; xx++){
        notes[xx + y * 9][v] = false;
    }
    
    //clear block
    int blockX = x / 3;
    int blockY = y / 3;
    for (int xx = 0; xx < 3; xx++){
        for (int yy = 0; yy < 3; yy++){
            notes[(blockX * 3 + xx) + (blockY * 3 + yy)*9][v] = false;
        }
    }
}

void Sudoku::FillNotes(char (&puzzle)[81], bool (&notes)[81][9]){
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

void Sudoku::Save(std::string fileName) {
    std::ofstream fout(fileName, std::ofstream::binary);

    for (int i = 0; i < 81; i++){
        fout << sudokuUser[i];
    }
    for (int i = 0; i < 81; i++) {
        fout << sudokuSolved[i];
    }
    for (int i = 0; i < 81; i++){
        fout << (char)sudokuStarting[i];
    }
    for (int i = 0; i < 81; i++){
        for (int j = 0; j < 9; j++){
            fout << (char)notes[i][j];
        }
    }

    fout.close();
}


bool Sudoku::Load(std::string fileName) {
    std::ifstream fin(fileName, std::ifstream::binary);
    if (!fin || fin.eof())
        return false;

    fin.read(sudokuUser, sizeof(sudokuUser));
    fin.read(sudokuSolved, sizeof(sudokuSolved));
    for (int i = 0; i < 81; i++){
        char c;
        fin >> c;
        if (c == (char)(false))
            sudokuStarting[i] = false;
        else
            sudokuStarting[i] = true;
    }
    for (int i = 0; i < 81; i++){
        for (int j = 0; j < 9; j++){
            char c;
            fin >> c;
            if (c == (char)(false))
                notes[i][j] = false;
            else
                notes[i][j] = true;
        }
    }

    fin.close();
    return true;
}

bool Sudoku::LoadFileExists(std::string fileName){
    std::ifstream f(fileName, std::ifstream::binary);
    bool good = f.good();
    f.close();
    return good;
}

bool Sudoku::Possible(char *sudoku, int x, int y, int n) {
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

bool Sudoku::Fill(int start) {
    // Recursive function which naively fills in random numbers and backtracks if an impossible puzzle is created
    // Each call of the function fills in a single empty square.
    // Returns true iff all squares are successfully filled.

    for (int i = start; i < 81; i++) {
        if (sudokuSolved[i] == 0) { // Check for empty squares
            // Randomly shuffle a list of possible values and check each if they're possible
            std::vector<int> possibles;
            for (int j = 1; j < 10; j++) possibles.push_back(j);
            std::random_shuffle(possibles.begin(), possibles.end());
            //for (int j = 0; j < 9; j++) cout << possibles[j] << "\n";
            for (int j = 0; j < 9; j++) {
                int n = possibles[j];
                if (Possible(sudokuSolved, i, n)) {
                    sudokuSolved[i] = n; // Naively fill in a random possible number
                    //cout << i % 9 << ", " << i / 9 << ": " << n << '\n';
                    if (Fill(start+1)) return true; // Fill in remaining squares recursively to find valid puzzle.
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

int Sudoku::MultipleSolutions(int start) {
    // Returns the number of solutions to the sudoku
    // Calculation will stop if number equals cap

    bool hasSolution = false;
    for (int i = 0; i < 81; i++) {
        if (sudokuUser[i] == 0) {
            for (int n = 1; n <= 9; n++) {
                if (Possible(sudokuUser, i, n)) {
                    sudokuUser[i] = n; // Naively fill in a random possible number
                    int branchSols = MultipleSolutions(i+1);
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

bool Sudoku::RemoveSquares() {
    //Remove all possible squares while maintaining unique solution
    //To generate a minimal sudoku puzzle

    int totalAttempts = 0;
    int attempts = 0; //number of
    int attemptMax = 15; //number of attempts at backstepping before we give up and try a new puzzle
    //TODO optimize this number ^
    int successfulRemoves = 0;
    int goalRemoves = 60; //TODO increase this as the AI improves
    std::vector<int> squares;
    for (int i = 0; i < 81; i++) squares.push_back(i);
    std::random_shuffle(squares.begin(), squares.end());

    int iteration = 0;
    while (iteration < 81 && attempts < attemptMax) {
        int i = squares[iteration++];
        //int mirror = 80-i;
        // Remove the randomly selected square
        int n = sudokuUser[i];
        //int o = sudokuUser[mirror];
        if (n == 0) //we already removed from here
            continue;
        sudokuUser[i] = 0;
        //sudokuUser[mirror]=0;

        if (!AISolve(sudokuUser)){
            sudokuUser[i] = n;
           // sudokuUser[mirror] = o;
            attempts++;
            totalAttempts++;
        } else {
            attempts = 0;
            successfulRemoves++;
            if (successfulRemoves == goalRemoves){
                Log::Debug.Info("Puzzle removal completed, total backsteps: %d", totalAttempts);
                return true;
            }
        }
    }
    return false;
}

void Sudoku::CreateSudoku() {
    // Create a random solved sudoku puzzle

    int attempt = 0;
    do {   
        attempt ++;
        for (int i = 0; i < 81; i++) sudokuSolved[i] = 0;

        Fill(0);

        for (int i = 0; i < 81; i++) sudokuUser[i] = sudokuSolved[i];
    } while (!RemoveSquares());

    Log::Debug.Info("Puzzle selected, attempt #%d", attempt);
    for (int i = 0; i < 81; i++)
        sudokuStarting[i] = sudokuUser[i] != 0;
}