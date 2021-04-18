#include "Game/View.h"
#include "Graphics/Frame.h"
#include "Sudoku.h"

/* TODO
 * comments
 * convert to MVC
 * record times (categorized on high scores by difficulty)
 * save/load game (and save time scores)

 * New Game/Restart/Quit
 */


bool View::Create() {
    srand(time(NULL));
    if (puzzle.LoadFileExists()){
        canContinue = true;
    } else mSel = 1;
    
    Frame board = Frame(11,11);
    board.DrawBorder(' ', BG_WHITE);
    
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            DrawFrame(x*10, y*10, &board);
        }
    }
    
    //MENU
    menu = new Frame(40, 80);
    menu->DrawString(0,0, "Sudoku!");
    menu->DrawString(0,1, "By Chris Schlenker,");
    menu->DrawString(0,2, "Mark Welch and Braxton Mott");
    
    
    //LEGEND
    legend = new Frame(30, 28);
    legend->DrawString(0,2,"WASD/Arrow Keys to move");
    legend->DrawString(0,3,"selection. Shift to view notes");
    legend->DrawString(0,4,"1-9 to write (0 clears)");
    legend->DrawString(0,5,"SHIFT + 1-9 to edit notes");
    legend->DrawString(0,6,"ESC to edit options");
    
    legend->DrawString(0,9,"GAME OPTIONS");
    
    debug = new Frame(100, 7);
    
    DrawMenu();
    return true;
}

void View::HandleInput(int key, keyState state){
    if (state.held && !state.pressed && !state.released) return;
    
    //SELECTION MOVEMENT
    if (begun && boardSelection && (key == VK_RIGHT || key == 'D') && state.pressed){
        xSel++;
        if (xSel > 8)
            xSel = 0;
    } else if (begun && boardSelection && (key == VK_LEFT || key == 'A') && state.pressed){
        xSel--;
        if (xSel < 0)
            xSel = 8;
    }
    if ((key == VK_DOWN || key == 'S') && state.pressed){
        if (!begun){
            mSel++;
            if (mSel > 2)
                mSel = (canContinue ? 0 : 1);
            DrawMenu();
        }
        else{
            if (boardSelection){
                ySel++;
                if (ySel > 8)
                    ySel = 0;
            }
            else{
                mSel++;
                if (mSel > 4)
                    mSel = 0;
                DrawLegend();
            }
        }
    } else if ((key == VK_UP || key == 'W') && state.pressed){
        if (!begun){
            mSel--;
            if (mSel < (canContinue ? 0 : 1))
                mSel = 2;
            DrawMenu();
        }
        else {
            if (boardSelection){
                ySel--;
                if (ySel < 0)
                    ySel = 8;
            } else{
                mSel--;
                if (mSel < 0)
                    mSel = 4;
                DrawLegend();
            }
        }
    } 
    
    //ESCAPE: toggle menu
    if (begun && key == VK_ESCAPE && state.pressed){
        boardSelection = !boardSelection;
        hintToggle = false;
        DrawLegend();
    }
    
    //ENTER/SPACE: Menu toggles
    if ((!begun || !boardSelection) && (key == VK_RETURN || key == VK_SPACE) && state.pressed){
        if (begun){
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
                {
                    hint = puzzle.RequestHint();
                    string hType;
                    switch (hint.type){
                        case SudokuPuzzle::mark:
                            hType = "Single";
                            break;
                        case SudokuPuzzle::pair:
                            hType = "Pair";
                            break;
                        case SudokuPuzzle::ERR:
                            hType = "ERR";
                            break;
                    }

                    Log::Debug.Info("Type: %s | x: %d, y: %d, z: %d, h: %d, i: %d, j: %d", hType.c_str(), hint.x, hint.y, hint.z, hint.h, hint.i, hint.j, hint.k);
                    hintToggle = true;
                }
                    break;
                case 4: //fill notes
                    puzzle.FillNotes();
                    break;
            }
            DrawLegend();
        } else{
            //TODO handle menu selection
            switch (mSel){
                case 0:
                    puzzle.Load();
                    begun = true;
                    DrawBorders();
                    DrawLegend();
                    break;
                case 1:
                    puzzle.createSudoku();
                    begun = true;
                    DrawBorders();
                    DrawLegend();
                    break;
                case 2:
                    exit(0);
                    break;
            }
        }
    }
    
    if (begun){
        //SHIFT: Show notes
        if (key == VK_SHIFT && state.pressed) {
            if (noteToggles) notesOn = !notesOn;
            else notesOn = true;
        } else if (key == VK_SHIFT && state.released && !noteToggles)
            notesOn = false;


        //0-9: Set note or value
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
    }
    
    //DETERMINE DRAW - we do this in HandleInput instead of Update to save a lot of draw cycles (drawing to Console is pretty expensive, and we only need to change these drawings in reaction to user input
    if (!begun){
        DrawMenu();
        return;
    }
    
    if (notesOn){
        DrawNotes();
    } else
        DrawBoard();
    if (hintToggle)
        DrawHints(hint);
}

bool View::Update(float deltaTime){
    if (begun){
        if (boardSelection) //don't increment time if we're paused //TODO if we keep this, we should hide the board while in the menu. Otherwise, we could pause to figure out the puzzle and play and enter it to cheat time record
            totalTime += deltaTime;

        //Draw time
        char s[16];
        snprintf(s, 16, "Time: %.0f s", totalTime);
        DrawString(40,0,s);
    }
    return true;
}

void View::DrawBorders(){
    Frame board = Frame(11,11);
    board.DrawBorder(' ', BG_WHITE);
    
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            DrawFrame(x*10, y*10, &board);
        }
    }
}

void View::DrawBoard(){
    Clear();
    for (int x = 0; x < 9; x++){
        for (int y = 0; y < 9; y++){
            SetBit(2+x*3+(x/3), 2+y*3+(y/3), (puzzle.Get(x,y) == 0?'_':puzzle.Get(x,y) + '0'), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + (puzzle.IsStarting(x,y)?FG_DARK_GRAY:FG_WHITE));
        }
    }
}

void View::DrawLegend(){
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

void View::DrawMenu(){
    menu->DrawString(0, 5, "Continue", (canContinue ? mSel == 0 ? BG_WHITE + FG_BLACK : BG_BLACK + FG_WHITE : BG_BLACK + FG_GRAY), false);
    menu->DrawString(0, 6, "New Game (Takes some time)", (mSel == 1 ? BG_WHITE + FG_BLACK : BG_BLACK + FG_WHITE), false);
    menu->DrawString(0, 7, "Exit Game", (mSel == 2 ? BG_WHITE + FG_BLACK : BG_BLACK + FG_WHITE), false);
    DrawFrame(0, 0, menu);
}

void View::Clear(){
    //Fill in the board, between borders, with empty space (BG_BLACK ' ')
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            Fill(1+x*10, 1+y*10,10+x*10,10+y*10, ' ', BG_BLACK);
        }
    }
}

void View::DrawNotes(){
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

void View::DrawHints(SudokuPuzzle::hint hint){
    switch(hint.type){
        case SudokuPuzzle::mark:
            SetBit(1+hint.x*3+(hint.x/3)+ hint.z%3, 1+hint.y*3+(hint.y/3) + hint.z/3, hint.z+'1', BG_GREEN + FG_BLACK);
            break;
        case SudokuPuzzle::pair:
        {
            //x and y for pair locations, set both hints hint.y, hint.z green
            
            SudokuPuzzle::coord c1 = puzzle.GetCoordFromHouseIndex(hint.h, hint.x, hint.i);
            SudokuPuzzle::coord c2 = puzzle.GetCoordFromHouseIndex(hint.h, hint.x, hint.j);
            int h1 = hint.y;
            int h2 = hint.z;
            
            int x1 = c1.x;
            int y1 = c1.y;
            int x2 = c2.x;
            int y2 = c2.y;
            
            SetBit(1 + x1 * 3 + (x1 / 3) + h1 % 3, 1 + y1 * 3 + (y1 / 3) + h1 / 3, h1 + '1', BG_GREEN + FG_BLACK);
            SetBit(1 + x1 * 3 + (x1 / 3) + h2 % 3, 1 + y1 * 3 + (y1 / 3) + h2 / 3, h2 + '1', BG_GREEN + FG_BLACK);
            SetBit(1 + x2 * 3 + (x2 / 3) + h1 % 3, 1 + y2 * 3 + (y2 / 3) + h1 / 3, h1 + '1', BG_GREEN + FG_BLACK);
            SetBit(1 + x2 * 3 + (x2 / 3) + h2 % 3, 1 + y2 * 3 + (y2 / 3) + h2 / 3, h2 + '1', BG_GREEN + FG_BLACK);
        }
            break;
        case SudokuPuzzle::ERR: //this is currently to determine if hint cannot resolve the puzzle //TODO remove this
            DrawString(0, 0, "ERROR", BG_RED+FG_BLACK, false);
            break;
            
    }
}

void View::Destroy(){    
    delete legend;
    delete debug;
}