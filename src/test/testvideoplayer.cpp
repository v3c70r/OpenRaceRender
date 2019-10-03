#include "videoplayer.h"
#include <iostream>
#include <cassert>
int main(int argc, char** argv)
{
    VideoPlayer player;

    assert(player.LoadFile("../raw/sample.mp4"));

    std::cout<<player.GetDebugStr()<<std::endl;

    player.ShutDown();

    return 0;
    
}
