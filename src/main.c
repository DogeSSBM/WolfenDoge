// #define FOV_NUM_RAYS        90
#define FOV_NUM_RAYS        720
#include "Includes.h"

void checkQuit(Map *map){
    if(keyPressed(SC_ESCAPE) || checkCtrlKey(SC_Q) || checkCtrlKey(SC_W)){
        mapFree(map);
        exit(EXIT_SUCCESS);
    }
}

EditorState checkEdit(Map *map, EditorState editor)
{
    if(checkCtrlKey(SC_E)){
        printf("Editing map -\n\tName:\"%s\"\n\tPath: \"%s\"\n", map->name, map->path);
        editor = mapEdit(map, editor);
        printf("\tDone!\n");
        setRelativeMouse(true);
    }
    return editor;
}

int main(int argc, char **argv)
{
    assertExpr(argc <= 2);
    init();
    Map map = mapLoad(argc == 2 ? argv[1] : NULL);
    EditorState editor = editorInitState();
    setRelativeMouse(true);
    while(1){
        const uint t = frameStart();

        checkQuit(&map);
        editor = checkEdit(&map, editor);
        const Length wlen = getWindowLen();
        const View firstView = {.len = wlen};
        const Length bvlen = iiC(coordMin(coordDivi(wlen, 4)));
        const View birdsView = {.len = bvlen, .pos = iC(wlen.x-bvlen.x, 0)};

        playerMove(&map);
        mapUpdateDynamics(&map);

        drawFp(firstView, &map, map.player);
        drawBv(birdsView, &map, map.player, coordMin(birdsView.len) / cfMax(mapSegBoundLen(map.seg)), fC(0,0));

        frameEnd(t);
    }
    return 0;
}
