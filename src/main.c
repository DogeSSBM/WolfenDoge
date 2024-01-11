// #define FOV_NUM_RAYS        90
#define FOV_NUM_RAYS        720
#include "Includes.h"

void checkQuit(Map *map){
    if(keyPressed(SC_ESCAPE) || checkCtrlKey(SC_Q) || checkCtrlKey(SC_W)){
        mapFree(map);
        exit(EXIT_SUCCESS);
    }
}

void checkEdit(Map *map)
{
    if(checkCtrlKey(SC_E)){
        printf("Editing map -\n\tName:\"%s\"\n\tPath: \"%s\"\n", map->name, map->path);
        mapEdit(map);
        printf("\tDone!\n");
        setRelativeMouse(true);
    }
}

int main(int argc, char **argv)
{
    assertExpr(argc <= 2);
    init();
    SDL_StopTextInput();
    gfx.outlined = false;
    winSetPosCoord(coordAddi(coordDivi(getWinDisplayLen(), 2), -400));
    maximizeWindow();
    Map map = mapLoad(argc == 2 ? argv[1] : NULL);

    setRelativeMouse(true);
    while(1){
        const uint t = frameStart();

        checkQuit(&map);
        checkEdit(&map);
        const Length wlen = getWindowLen();
        const View firstView = {.len = wlen};
        const Length bvlen = iiC(coordMin(coordDivi(wlen, 4)));
        const View birdsView = {.len = bvlen, .pos = iC(wlen.x-bvlen.x, 0)};

        // const Coordf oldPos = map.player.pos;
        playerMove(&map);
        // if(!cfSame(oldPos, map.player.pos))
            // mapApplyUpdates(&map, mapQueueUpdates(oldPos, map.player.pos, &map));
        // const bool oldb = map.seg[S_TRIG]->next->trig.state;
        mapUpdateDynamics(&map);

        drawFp(firstView, &map, map.player);
        drawBv(birdsView, &map, map.player, coordMin(birdsView.len) / cfMax(mapSegBoundLen(map.seg)), fC(0,0));

        frameEnd(t);
    }
    return 0;
}
