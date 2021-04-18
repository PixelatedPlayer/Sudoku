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
    
    //DETERMINE MENU SELECTION STARTING POINT
    if (!controller.CanContinue())
        mSel = 1;
    
    //CREATE THE BOARD, which is only a section of the full board, redrawn to
    //create the complete board
    Frame board = Frame(11,11);
    board.DrawBorder(' ', BG_WHITE);
    
    //DRAW THE BOARD
    for (int x = 0; x < 3; x++){
        for (int y = 0; y < 3; y++){
            DrawFrame(x*10, y*10, &board);
        }
    }
    
    //CREATE AND FILL THE MAIN MENU
    loadingMenu = new Frame(40, 80);
    loadingMenu->DrawString(0,0, "Sudoku!");
    loadingMenu->DrawString(0,1, "By Chris Schlenker,");
    loadingMenu->DrawString(0,2, "Mark Welch and Braxton Mott");
    
    
    //CREATE AND FILL THE GAME MENU
    gameMenu = new Frame(30, 28);
    gameMenu->DrawString(0,2,"WASD/Arrow Keys to move");
    gameMenu->DrawString(0,3,"selection. Shift to view notes");
    gameMenu->DrawString(0,4,"1-9 to write (0 clears)");
    gameMenu->DrawString(0,5,"SHIFT + 1-9 to edit notes");
    gameMenu->DrawString(0,6,"ESC to edit options");
    gameMenu->DrawString(0,9,"GAME OPTIONS");
    
    
    //CREATE THE DEBUG MENU
    debug = new Frame(100, 7);
    
    //DRAW THE MAIN MENU TO START
    DrawMenu();
    return true;
}

void View::HandleInput(int key, keyState state){
    if (state.held && !state.pressed && !state.released) return;
    
    //SELECTION MOVEMENT, determines if we're in the main menu, game menu, or board
    if (controller.Begun() && boardSelection && (key == VK_RIGHT || key == 'D') && state.pressed){
        xSel++;
        if (xSel > 8)
            xSel = 0;
    } else if (controller.Begun() && boardSelection && (key == VK_LEFT || key == 'A') && state.pressed){
        xSel--;
        if (xSel < 0)
            xSel = 8;
    }
    if ((key == VK_DOWN || key == 'S') && state.pressed){
        if (!controller.Begun()){
            mSel++;
            if (mSel > 2)
                mSel = (controller.CanContinue() ? 0 : 1);
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
                DrawGameMenu();
            }
        }
    } else if ((key == VK_UP || key == 'W') && state.pressed){
        if (!controller.Begun()){
            mSel--;
            if (mSel < (controller.CanContinue() ? 0 : 1))
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
                DrawGameMenu();
            }
        }
    } 
    
    //ESCAPE: toggle menu
    if (controller.Begun() && key == VK_ESCAPE && state.pressed){
        boardSelection = !boardSelection;
        hintToggle = false;
        DrawGameMenu();
    }
    
    //ENTER/SPACE: Enact selection
    if ((!controller.Begun() || !boardSelection) && (key == VK_RETURN || key == VK_SPACE) && state.pressed){
        if (controller.Begun()){
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
                    hint = controller.RequestHint();
                    std::string hType;
                    switch (hint.type){
                        case mark:
                            hType = "Single";
                            break;
                        case pair:
                            hType = "Pair";
                            break;
                        case ERR:
                            hType = "ERR";
                            break;
                    }

                    Log::Debug.Info("Type: %s | x: %d, y: %d, z: %d, h: %d, i: %d, j: %d", hType.c_str(), hint.x, hint.y, hint.z, hint.h, hint.i, hint.j, hint.k);
                    hintToggle = true;
                }
                    break;
                case 4: //fill notes
                    controller.FillNotes();
                    break;
            }
            DrawGameMenu();
        } else{
            //TODO handle menu selection
            switch (mSel){
                case 0:
                    controller.Load();
                    DrawBorders();
                    DrawGameMenu();
                    break;
                case 1:
                    DrawBorders();
                    DrawGameMenu();
                    break;
                case 2:
                    exit(0);
                    break;
            }
        }
    }
    
    //SHIFT: Show notes
    if (controller.Begun()){
        if (key == VK_SHIFT && state.pressed) {
            if (noteToggles) notesOn = !notesOn;
            else notesOn = true;
        } else if (key == VK_SHIFT && state.released && !noteToggles)
            notesOn = false;


        //0-9: Set note or value
        if (key >= '0' && key <= '9' && !controller.IsStarting(xSel, ySel)){ //we are modifying note
            if (notesOn){
                if (key == '0'){ //clear note
                    for (int i = 0; i < 9; i++)
                        controller.SetNote(xSel,ySel,i,false);
                } else if (state.pressed)//toggle note
                    controller.SetNote(xSel,ySel,key-'1', !controller.GetNote(xSel, ySel, key-'1'));
            }
            else{
                if (key != '0'){
                    std::vector<coord> obstructions = controller.MarkErrors(xSel, ySel,key-'0');
                    for (int i = 0; i < obstructions.size(); i++){
                        int tx = 2+obstructions[i].x*3+(obstructions[i].x/3);
                        int ty = 2+obstructions[i].y*3+(obstructions[i].y/3);
                        SetBit(tx, ty, GetBit(tx,ty).glyph, BG_RED + FG_BLACK);
                    }
                    if (obstructions.size() > 0) return;
                }
                controller.Set(xSel, ySel, (key=='0'||key==controller.Get(xSel,ySel)?0:key-'0'));
            }
        }
    }
    
    //DETERMINE DRAW - we do this in HandleInput instead of Update to save a lot of draw cycles (drawing to Console is pretty expensive, and we only need to change these drawings in reaction to user input
    if (!controller.Begun()){
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
    if (controller.Begun()){
        if (boardSelection) //don't increment time if we're paused //TODO if we keep this, we should hide the board while in the menu. Otherwise, we could pause to figure out the puzzle and play and enter it to cheat time record
            controller.IncrementTime(deltaTime);

        //Draw time
        char s[16];
        snprintf(s, 16, "Time: %.0f s", controller.GetTime());
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
            SetBit(2+x*3+(x/3), 2+y*3+(y/3), (controller.Get(x,y) == 0?'_':controller.Get(x,y) + '0'), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + (controller.IsStarting(x,y)?FG_DARK_GRAY:FG_WHITE));
        }
    }
}

void View::DrawGameMenu(){
    gameMenu->DrawString(0,10,"Shift Toggle: ", (!boardSelection && mSel == 0 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    gameMenu->DrawString(0,11,"Checkerboard: ", (!boardSelection && mSel == 1 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    gameMenu->DrawString(0,12,"Invert notes: ", (!boardSelection && mSel == 2 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    gameMenu->DrawString(0,14, "Hint (cheat - Does not", (!boardSelection && mSel == 3 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    gameMenu->DrawString(0,15, "validate your notes)", (!boardSelection && mSel == 3 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    gameMenu->DrawString(0,16, "Fill notes (cheat)", (!boardSelection && mSel == 4 ? BG_GRAY + FG_BLACK : BG_BLACK + FG_WHITE), false);
    gameMenu->SetBit(15,10, noteToggles?'X':' ', BG_GRAY + FG_BLACK);
    gameMenu->SetBit(15,11, checkerboard?'X':' ', BG_GRAY + FG_BLACK);
    gameMenu->SetBit(15,12, invertNotes?'X':' ', BG_GRAY + FG_BLACK);
    DrawFrame(40, 0, gameMenu);
    Log::Debug.Display(debug, logINFO, 0, 1, false);
    DrawFrame(0, 32, debug);
}

void View::DrawMenu(){
    loadingMenu->DrawString(0, 5, "Continue", (controller.CanContinue() ? mSel == 0 ? BG_WHITE + FG_BLACK : BG_BLACK + FG_WHITE : BG_BLACK + FG_GRAY), false);
    loadingMenu->DrawString(0, 6, "New Game (Takes some time)", (mSel == 1 ? BG_WHITE + FG_BLACK : BG_BLACK + FG_WHITE), false);
    loadingMenu->DrawString(0, 7, "Exit Game", (mSel == 2 ? BG_WHITE + FG_BLACK : BG_BLACK + FG_WHITE), false);
    DrawFrame(0, 0, loadingMenu);
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
            if (controller.Get(x, y) != 0)
                SetBit(2+x*3+(x/3), 2+y*3+(y/3), (controller.Get(x,y) == 0?'_':controller.Get(x,y)+'0'), (x==xSel && y==ySel?BG_GRAY:BG_BLACK) + (controller.IsStarting(x,y)?FG_DARK_GRAY:FG_WHITE));
            else{
                for (int z = 0; z < 9; z++){
                    SetBit(1+x*3+(x/3)+ z%3, 1+y*3+(y/3) + z/3, controller.GetNote(x,y,z)?z+'1':' ', (x==xSel&&y==ySel?BG_WHITE + FG_BLACK:checkerboard?c?BG_DARK_GREEN+FG_WHITE:BG_DARK_BLUE+FG_WHITE:BG_BLACK+FG_WHITE));
                }
            }
            c = !c;
        }
    }
}

void View::DrawHints(Sudoku::hint hint){
    switch(hint.type){
        case mark:
            SetBit(1+hint.x*3+(hint.x/3)+ hint.z%3, 1+hint.y*3+(hint.y/3) + hint.z/3, hint.z+'1', BG_GREEN + FG_BLACK);
            break;
        case pair:
        {
            //x and y for pair locations, set both hints hint.y, hint.z green
            
            coord c1 = controller.GetCoordFromHouseIndex(hint.h, hint.x, hint.i);
            coord c2 = controller.GetCoordFromHouseIndex(hint.h, hint.x, hint.j);
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
        case ERR: //this is currently to determine if hint cannot resolve the puzzle //TODO remove this
            DrawString(0, 0, "ERROR", BG_RED+FG_BLACK, false);
            break;
            
    }
}

View::~View(){    
    delete gameMenu;
    delete debug;
    delete loadingMenu;
}