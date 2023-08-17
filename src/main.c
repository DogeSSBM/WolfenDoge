// #define FOV_NUM_RAYS        90
#define FOV_NUM_RAYS        720
#include "Includes.h"

int main(int argc, char **argv)
{
    assertExpr(argc <= 2);
    init();
    // setBlend(BLEND_NONE);
    SDL_StopTextInput();
    gfx.outlined = false;
    winSetPosCoord(coordAddi(coordDivi(getWinDisplayLen(), 2), -400));

    Map map = mapLoad(argc == 2 ? argv[1] : NULL);
    bool focus = false;

    while(1){
        const uint t = frameStart();

        setRelativeMouse(winIsFocused());
        if(keyPressed(SC_ESCAPE) || checkCtrlKey(SC_Q) || checkCtrlKey(SC_W)){
            return 0;
        }
        const bool prvFocus = focus;
        focus = winIsFocused();
        if(prvFocus != focus)
            setRelativeMouse(focus);

        if(checkCtrlKey(SC_E)){
            printf("Editing map: '%s'\n", map.path);
            mapEdit(&map);
        }

        const Length wlen = getWindowLen();
        const View firstView = {.len = wlen};
        const Length bvlen = iiC(coordMin(coordDivi(wlen, 4)));
        View birdsView = {.len = bvlen, .pos = iC(wlen.x-bvlen.x, 0)};

        const Coordf oldPos = map.player.pos;
        playerMove(&map);
        if(!cfSame(oldPos, map.player.pos))
            mapApplyUpdates(&map, mapQueueUpdates(oldPos, map.player.pos, &map));
        mapUpdateDynamics(&map);

        drawFp(firstView, &map, map.player, wlen);
        drawBv(birdsView, &map, map.player, coordMin(birdsView.len) / cfMax(mapSegBoundLen(map.seg)), fC(0,0));

        frameEnd(t);
    }
    return 0;
}
