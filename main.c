#include "DogeLib/Includes.h"
#include "WolfenDoge.h"

int main(int argc, char **argv)
{
    assertExpr(argc <= 2);
    init();
    gfx.outlined = false;
    winSetPosCoord(coordAddi(coordDivi(getWinDisplayLen(), 2), -400));
    Wall *map = NULL;
    char *fileName;
    char defaultName[64] = {0};
    if(argc < 2){
        defaultMapFileName(defaultName);
        fileName = defaultName;
        map = mapDefault();
    }else{
        fileName = argv[1];
        map = mapLoad(fileName);
    }

    assertExpr(map);
    SDL_SetRelativeMouseMode(true);

    Player player = {.pos = {.x=125.0f, .y=125.0f}};

    while(1){
        const uint t = frameStart();

        if(keyPressed(SDL_SCANCODE_ESCAPE) || checkCtrlKey(SDL_SCANCODE_Q) || checkCtrlKey(SDL_SCANCODE_W)){
            return 0;
        }

        if(checkCtrlKey(SDL_SCANCODE_E)){
            map = mapEdit(map, fileName);
            SDL_SetRelativeMouseMode(true);
        }

        const Length wlen = getWindowLen();
        const View firstView = {.len = wlen};
        const View birdsView = {.len = iC(coordMin(wlen)/4,coordMin(wlen)/4), .pos = {.x=wlen.x-coordMin(wlen)/4}};

        player = playerMoveMouse(player);

        drawFp(firstView, map, player);
        drawBv(birdsView, map, player, coordMin(birdsView.len)/ cfMax(mapBounds(map)), fC(0,0));

        frameEnd(t);
    }
    return 0;
}
