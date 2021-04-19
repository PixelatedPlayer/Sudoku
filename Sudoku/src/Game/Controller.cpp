#include "Controller.h"

Controller::Controller(){
    srand(time(NULL));
    if (puzzle.LoadFileExists(gameFileName))
        canContinue = true;
}

void Controller::Load(){
    puzzle.Load(gameFileName);
    begun = true;
}

void Controller::NewGame(Difficulty difficulty){
    puzzle.CreateSudoku(difficulty);
    begun = true;
}

void Controller::Set(int x, int y, int v){
    puzzle.Set(x, y, v);
    if (v != 0);
        puzzle.ClearNotesInHouses(x, y, v - 1);
}


void Controller::SetNote(int x, int y, int z, bool v){
    puzzle.SetNote(x, y, z, v);
}

void Controller::Victory() {
    //delete the save game (so we can't continue it from completion)
    std::remove(gameFileName);
    
    //load the scoreboard
    std::vector<ScoreboardNode> scoreboard = LoadScoreboard();
    int n = scoreboard.size();

    //now add to the scoreboard, via insertion sort
    for (int i = 0; i < n; i++){
        if (puzzle.GetTime() < scoreboard[i].time) //quicker than this one, insert before it
            scoreboard.insert(scoreboard.begin()+i, ScoreboardNode(puzzle.GetDifficulty(), puzzle.GetTime()));
    }
    if (scoreboard.size() == n) //we didn't insert (it was worst time yet; or first entry)
        scoreboard.push_back(ScoreboardNode(puzzle.GetDifficulty(), puzzle.GetTime()));
    
    //now we re-write the scoreboard
    n+=1;
    std::ofstream f(scoreboardFileName, std::fstream::binary | std::fstream::out);
    f.write(reinterpret_cast<char*>(&n), sizeof(n));
    for (int i = 0; i < n; i++){
        f.write(reinterpret_cast<char*>(&scoreboard[i]), sizeof(ScoreboardNode));
    }
    f.close();
}

std::vector<Controller::ScoreboardNode> Controller::LoadScoreboard(){
    std::ifstream f(scoreboardFileName, std::fstream::binary | std::fstream::in);
    
    int n = 0;
    std::vector<ScoreboardNode> scoreboard;
    
    if (f){ //we have an existing scoreboard
        //load scoreboard into data
        f.read(reinterpret_cast<char*>(&n), sizeof(n));
        ScoreboardNode node;
        for (int i = 0; i < n; i++){
            f.read(reinterpret_cast<char*>(&node), sizeof(node));
            scoreboard.push_back(node);
        }
    }
    
    f.close();
    return scoreboard;
}

void Controller::IncrementTime(float deltaTime) {
    puzzle.IncrementTime(deltaTime);
    saveTimer += deltaTime;
    if (saveTimer >= 1.0f){
        puzzle.Save(gameFileName);
        saveTimer = 0.0f;
    }
}