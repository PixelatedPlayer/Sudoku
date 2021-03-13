#include "Game/Game.h"
#include "Graphics/Frame.h"
#include "Sudoku.h"

/* TODO
 * record times (categorized on high scores by difficulty)
 * save/load game (and save time scores)

 * New Game/Restart/Quit
 */
bool Game::Create() {
    srand(time(NULL));
    puzzle.createSudoku();
    
    Frame board = Frame(11,11);
    board.DrawBorder(' ', BG_WHITE);
    
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            DrawFrame(x*10, y*10, &board);
        }
    }
    
    //LEGEND
    legend = new Frame(30, 28);
    legend->DrawString(0,2,"WASD/Arrow Keys to move");
    legend->DrawString(0,3,"selection. Shift to view notes");
    legend->DrawString(0,4,"1-9 to write (0 clears)");
    legend->DrawString(0,5,"SHIFT + 1-9 to edit notes");
    legend->DrawString(0,6,"ESC to edit options");
    
    legend->DrawString(0,9,"GAME OPTIONS");
    
    debug = new Frame(50, 10);
    
    DrawMenu();
    DrawBoard();
    return true;
}

void Game::HandleInput(int key, keyState state){
    if (state.held && !state.pressed && !state.released) return;
    
    //board[i], where i is the index in series. so i=3 is row 1 column 2 (out of 3,3, 0 exclusive)
    if (boardSelection && (key == VK_RIGHT || key == 'D') && state.pressed){
        xSel++;
        if (xSel > 8)
            xSel = 0;
    } else if (boardSelection && (key == VK_LEFT || key == 'A') && state.pressed){
        xSel--;
        if (xSel < 0)
            xSel = 8;
    }
    if ((key == VK_DOWN || key == 'S') && state.pressed){
        if (boardSelection){
            ySel++;
            if (ySel > 8)
                ySel = 0;
        }
        else{
            mSel++;
            if (mSel > 4)
                mSel = 0;
            DrawMenu();
        }
    } else if ((key == VK_UP || key == 'W') && state.pressed){
        if (boardSelection){
            ySel--;
            if (ySel < 0)
                ySel = 8;
        } else{
            mSel--;
            if (mSel < 0)
                mSel = 4;
            DrawMenu();
        }
    } 
    
    if (key == VK_ESCAPE && state.pressed){
        boardSelection = !boardSelection;
        hintToggle = false;
        DrawMenu();
    }
    if (!boardSelection && (key == VK_RETURN || key == VK_SPACE) && state.pressed){
        //toggle mSel
        switch(mSel){
            case 0:
                noteToggles = !noteToggles;
                notesOn = false;
                break;
            case 1:
                checkerboard = !checkerboard;
                break;
            case 2:
                invertNotes = !invertNotes;
                break;
            case 3:
                hint = puzzle.RequestHint();
                hintToggle = true;
                break;
            case 4: //fill notes
                puzzle.FillNotes();
                break;
        }
        DrawMenu();
    }
    
    if (key == VK_SHIFT && state.pressed) {
        if (noteToggles) notesOn = !notesOn;
        else notesOn = true;
    } else if (key == VK_SHIFT && state.released && !noteToggles)
        notesOn = false;
        
    
    if (key >= '0' && key <= '9' && !puzzle.IsStarting(xSel, ySel)){ //we are modifying note
        if (notesOn){
            if (key == '0'){ //clear note
                for (int i = 0; i < 9; i++)
                    puzzle.SetNote(xSel,ySel,i,false);
            } else if (state.pressed)//toggle note
                puzzle.SetNote(xSel,ySel,key-'1', !puzzle.GetNote(xSel, ySel, key-'1'));
        }
        else{
            if (key != '0'){
                std::vector<SudokuPuzzle::coord> obstructions = puzzle.MarkErrors(xSel, ySel,key-'0');
                for (int i = 0; i < obstructions.size(); i++){
                    int tx = 2+obstructions[i].x*3+(obstructions[i].x/3);
                    int ty = 2+obstructions[i].y*3+(obstructions[i].y/3);
                    SetBit(tx, ty, GetBit(tx,ty).glyph, BG_RED + FG_BLACK);
                }
                if (obstructions.size() > 0) return;
            }
            puzzle.Set(xSel, ySel, (key=='0'||key==puzzle.Get(xSel,ySel)?0:key-'0'));
        }
    }
    
    if (notesOn){ // || GetKey(VK_SHIFT).held
        DrawNotes();
    } else
        DrawBoard();
    if (hintToggle)
        DrawHints(hint);
}

bool Game::Update(float deltaTime){
    if (boardSelection)
        totalTime += deltaTime;
        
    char s[16];
    snprintf(s, 16, "Time: %.0f s", totalTime);
    DrawString(40,0,s);
    return true;
}

void Game::DrawBoard(){
    Clear();
    for (int x = 0; x < 9; x++){
        for (int y = 0; y < 9; y++){
            SetBit(2+x*3+(x/3), 2+y*3+(y/3), (puzzle.Get(x,y) == 0?'_':puzzle.Get(x,y) + '0'), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + (puzzle.IsStarting(x,y)?FG_DARK_GRAY:FG_WHITE));
        }
    }
}

void Game::DrawMenu(){
    legend->DrawString(0,10,"Shift Toggle: ", (!boardSelection && mSel == 0 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    legend->DrawString(0,11,"Checkerboard: ", (!boardSelection && mSel == 1 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    legend->DrawString(0,12,"Invert notes: ", (!boardSelection && mSel == 2 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    legend->DrawString(0,14, "Hint (cheat - Does not", (!boardSelection && mSel == 3 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    legend->DrawString(0,15, "validate your notes)", (!boardSelection && mSel == 3 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    legend->DrawString(0,16, "Fill notes (cheat)", (!boardSelection && mSel == 4 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    legend->SetBit(15,10, noteToggles?'X':' ', BG_GRAY + FG_BLACK);
    legend->SetBit(15,11, checkerboard?'X':' ', BG_GRAY + FG_BLACK);
    legend->SetBit(15,12, invertNotes?'X':' ', BG_GRAY + FG_BLACK);
    DrawFrame(40, 0, legend);
    Log::Debug.Display(debug, logINFO, 0, 1, false);
    DrawFrame(0, 32, debug);
}

void Game::Clear(){
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            Fill(1+x*10, 1+y*10,10+x*10,10+y*10, ' ', BG_BLACK);
        }
    }
}

void Game::DrawNotes(){
    bool c = false;
    for (int x = 0; x < 9; x++){
        for (int y = 0; y < 9; y++){
            if (puzzle.Get(x, y) != 0)
                SetBit(2+x*3+(x/3), 2+y*3+(y/3), (puzzle.Get(x,y) == 0?'_':puzzle.Get(x,y)+'0'), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + (puzzle.IsStarting(x,y)?FG_DARK_GRAY:FG_WHITE));
            else{
                for (int z = 0; z < 9; z++){
                    SetBit(1+x*3+(x/3)+ z%3, 1+y*3+(y/3) + z/3, puzzle.GetNote(x,y,z)?z+'1':' ', (x==xSel&&y==ySel?BG_WHITE + FG_BLACK:checkerboard?c?BG_DARK_GREEN+FG_WHITE:BG_DARK_BLUE+FG_WHITE:BG_BLACK+FG_WHITE));
                }
            }
            c = !c;
        }
    }
}

void Game::DrawHints(SudokuPuzzle::hint hint){
    switch(hint.type){
        case SudokuPuzzle::mark:
            SetBit(1+hint.x*3+(hint.x/3)+ hint.z%3, 1+hint.y*3+(hint.y/3) + hint.z/3, hint.z+'1', BG_GREEN + FG_BLACK);
            break;
        case SudokuPuzzle::ERR:
            DrawString(0, 0, "ERROR", BG_RED+FG_BLACK, false);
            break;
    }
}

void Game::Destroy(){    
    destroyed = true;
}