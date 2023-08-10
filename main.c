#include "Includes.h"

int main(int argc, char **argv)
{
    assertExpr(argc <= 2);
    init();
    gfx.outlined = false;
    winSetPosCoord(coordAddi(coordDivi(getWinDisplayLen(), 2), -400));
    Seg *map = NULL;
    char mapFilePath[128] = "./Maps/map.bork";
    if(argc < 2){
        const uint mapNum = newMapFileNum();
        if(mapNum != 0)
            sprintf(mapFilePath, "./Maps/map(%u).bork", mapNum);
        map = mapDefault();
    }else{
        assertExpr(argc == 2);
        assertExpr(strlen(argv[1]) < 128);
        sprintf(mapFilePath, "%s", argv[1]);
        printf("mapFilePath: \"%s\"\n", mapFilePath);
        map = mapLoad(mapFilePath);
    }

    assertExpr(map);
    setRelativeMouse(true);

    Player player = {.pos = {.x=125.0f, .y=125.0f}};
    while(1){
        const uint t = frameStart();

        if(keyPressed(SC_ESCAPE) || checkCtrlKey(SC_Q) || checkCtrlKey(SC_W)){
            return 0;
        }
        setRelativeMouse(winIsFocused());

        if(checkCtrlKey(SC_E)){
            printf("Editing map: '%s'\n", mapFilePath);
            map = mapEdit(map, mapFilePath);
        }

        const Length wlen = getWindowLen();
        const View firstView = {.len = wlen};
        const Length bvlen = iiC(coordMin(coordDivi(wlen, 4)));
        View birdsView = {.len = bvlen, .pos = iC(wlen.x-bvlen.x, 0)};

        const Coordf oldPos = player.pos;
        player = playerMove(player, map);
        if(!cfSame(oldPos, player.pos))
            map = mapApplyUpdates(map, mapQueueUpdates(oldPos, player.pos, map));
        mapUpdateDynamics(map);

        drawFp(firstView, map, player, wlen);
        drawBv(birdsView, map, player, coordMin(birdsView.len) / cfMax(mapLength(map)), fC(0,0));

        frameEnd(t);
    }
    return 0;
}
