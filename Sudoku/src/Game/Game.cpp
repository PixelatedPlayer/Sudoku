#include "Game/Game.h"
#include "Graphics/Frame.h"
#include "Sudoku.h"

/* TODO
 * we actually want 9x9 per sub board and 27x27 for major board, so we can show hints
 * 
 *  
 */
bool Game::Create() {
    int i = 0;
    Frame board[9] = {Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11),Frame(11,11)};
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            board[i].DrawBorder('#');
            DrawFrame(x*10, y*10, &board[i++]);
        }
    }
    DrawValues();
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
    
    if (key == VK_SHIFT){
        if (state.pressed){ //show hints
            for (int x = 0; x < 3; x++){
                for (int y = 0; y < 3; y++){
                    for (int xx = 0; xx < 3; xx++){
                        for (int yy = 0; yy < 3; yy++){
                            SetBit(x*9+1+xx, y*9+1+yy, (puzzle.GetHint(x,y,xx+yy*3)?xx+yy*3:'-'));
                        }
                    }
                }
            }
        } else if (state.released){ //show puzzle
            for (int x = 0; x < 3; x++){
                for (int y = 0; y < 3; y++){
                    Fill(1+x*10, 1+y*10,10+x*10,10+y*10, ' ', BG_BLACK);
                }
            }
        }
    } else{
        DrawValues();
    }
    return true;
}

void Game::DrawValues(){
    for (int x = 0; x < 9; x++){
        for (int y = 0; y < 9; y++){
            SetBit(2+x*3+(x/3), 2+y*3+(y/3), (puzzle.Get(x,y) == ' '?'_':puzzle.Get(x,y)), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + FG_WHITE);
        }
    }
}

void Game::Destroy(){    
    destroyed = true;
}
