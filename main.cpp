
#include <iostream>
#include <memory>
#include <fstream>

#include "GameFolder/GameEngine.h"


bool suppressTraceFlag = false;
bool suppressWarningFlag = false;

int main()
{
    

    GameEngine game("");
    game.run();

    return 0;
}