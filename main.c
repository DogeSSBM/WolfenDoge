#include "Includes.h"

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
        const Length bvlen = iiC(coordMin(coordDivi(wlen, 4)));
        View birdsView = {.len = bvlen, .pos = iC(wlen.x-bvlen.x, 0)};

        player = playerMoveMouse(player, map);

        drawFp(firstView, map, player, wlen);
        drawBv(birdsView, map, player, coordMin(birdsView.len) / cfMax(mapBounds(map)), fC(0,0));

        frameEnd(t);
    }
    return 0;
}
