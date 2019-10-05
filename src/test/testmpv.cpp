#include "videoplayermpv.h"
int main(int argc, char *argv[])
{
    VideoPlayerMPV player;
    player.LoadVideo("../raw/sample.mp4");
    return 0;
}

