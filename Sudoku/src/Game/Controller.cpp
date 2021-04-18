#include "Controller.h"

Controller::Controller(){
    srand(time(NULL));
    if (puzzle.LoadFileExists(fileName))
        canContinue = true;
}

void Controller::Load(){
    puzzle.Load(fileName);
    begun = true;
}

void Controller::NewGame(){
    puzzle.CreateSudoku();
    begun = true;
}

void Controller::Set(int x, int y, int v){
    puzzle.Set(x, y, v);
    if (v != 0);
        puzzle.ClearNotesInHouses(x, y, v - 1);
    puzzle.Save(fileName);
}


void Controller::SetNote(int x, int y, int z, bool v){
    puzzle.SetNote(x, y, z, v);
    puzzle.Save(fileName);
}

void Controller::DeleteSave() {
    std::remove(fileName);
}