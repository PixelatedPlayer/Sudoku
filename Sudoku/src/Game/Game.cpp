#include "Game/Game.h"
#include "Graphics/Frame.h"
#include "Sudoku.h"

/* TODO
 * don't let user change the value of a starting value; maybe color starting values differently 
 * mark error when entering a clearly incorrect (another in the same row/column/block exists)
 * record times (categorized on high scores by difficulty)
 * save/load game (and save time scores)
 * Game options
 *      always show notes/hints
 *      toggle hints
 *      disable color checkerboard
 * 
 *      color customization?? 
 * New Game/Restart/Quit
 * Entering same value clears it
 */
bool Game::Create() {
    srand(NULL);
    puzzle.createSudoku();
    
    int i = 0;
    Frame board[9] = {Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11)};
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            board[i].DrawBorder(' ', BG_WHITE);
            DrawFrame(x*10, y*10, &board[i++]);
        }
    }
    
    //LEGEND
//    legend = new Frame(30, 28);
//    legend->DrawString(0,0,"WASD/Arrow Keys to move");
//    legend->DrawString(0,1,"selection. Shift to view notes");
//    legend->DrawString(0,2,"1-9 to write (0 clears)");
//    legend->DrawString(0,3,"SHIFT + 1-9 to edit notes");
//    legend->DrawString(0,4,"ESC to edit options");
//    
//    legend->DrawString(0,7,"GAME OPTIONS");
//    legend->DrawString(0,8,"GAME OPTIONS");
//    DrawFrame(40, 0, legend);
    DrawBoard();
    return true;
}

bool Game::Update(int key, keyState state){
    //board[i], where i is the index in series. so i=3 is row 1 column 2 (out of 3,3, 0 exclusive)
    if ((key == VK_RIGHT || key == 'D') && state.pressed){
        xSel++;
        if (xSel > 8)
            xSel = 0;
    } else if ((key == VK_LEFT || key == 'A') && state.pressed){
        xSel--;
        if (xSel < 0)
            xSel = 8;
    }
    if ((key == VK_DOWN || key == 'S') && state.pressed){
        ySel++;
        if (ySel > 8)
            ySel = 0;
    } else if ((key == VK_UP || key == 'W') && state.pressed){
        ySel--;
        if (ySel < 0)
            ySel = 8;
    } 
    
    if (GetKey(VK_SHIFT).pressed || GetKey(VK_SHIFT).held){
        if (key >= '1' && key <= '9' && state.pressed){ //we are modifying hint
            puzzle.SetHint(xSel,ySel,key-'1', !puzzle.GetHint(xSel, ySel, key-'1'));
        }
        DrawHints();
    }
    else{
        if (key >= '0' && key <= '9' && state.pressed){ //we are modifying value
            puzzle.Set(xSel, ySel, (key=='0'||key==puzzle.Get(xSel,ySel)?0:key-'0'));
        }
        DrawBoard();
    }
    
    return true;
}

void Game::DrawBoard(){
    Clear();
    for (int x = 0; x < 9; x++){
        for (int y = 0; y < 9; y++){
            SetBit(2+x*3+(x/3), 2+y*3+(y/3), (puzzle.Get(x,y) == 0?'_':puzzle.Get(x,y) + '0'), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + FG_WHITE);
        }
    }
}

void Game::Clear(){
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            Fill(1+x*10, 1+y*10,10+x*10,10+y*10, ' ', BG_BLACK);
        }
    }
}

void Game::DrawHints(){
    bool c = false;
    for (int x = 0; x < 9; x++){
        for (int y = 0; y < 9; y++){
            if (puzzle.Get(x, y) != 0)
                SetBit(2+x*3+(x/3), 2+y*3+(y/3), (puzzle.Get(x,y) == 0?'_':puzzle.Get(x,y)+'0'), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + FG_WHITE);
            else{
                for (int z = 0; z < 9; z++){
                    SetBit(1+x*3+(x/3)+ z%3, 1+y*3+(y/3) + z/3, puzzle.GetHint(x,y,z)?z+'1':' ', (x==xSel&&y==ySel?BG_WHITE + FG_BLACK:c?BG_DARK_GREEN+FG_WHITE:BG_DARK_BLUE+FG_WHITE));
                }
            }
            c = !c;
        }
    }
}

void Game::Destroy(){    
    destroyed = true;
}