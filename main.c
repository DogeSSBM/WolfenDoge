#include "DogeLib/Includes.h"
Coordf fC(const float x, const float y)
{
    return (const Coordf){.x=x, .y=y};
}

float fmost(const float a, const float b)
{
    return a > b ? a : b;
}

float cfMax(const Coordf pos)
{
    return fmost(pos.x, pos.y);
}

Coord coordAbs(const Coord pos)
{
    return (const Coord){.x=pos.x<0?-pos.x:pos.x, .y=pos.y<0?-pos.y:pos.y};
}

bool checkCtrlKey(const Scancode key)
{
    return keyPressed(key) && (keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL));
}

typedef struct{
    Coord pos;
    Length len;
}View;

typedef struct Player{
    Coordf pos;
    float ang;
    float speed;
}Player;

typedef struct Wall{
    Color c;
    Coordf a;
    Coordf b;
    struct Wall *next;
}Wall;

typedef struct{
    Color c;
    Coordf a;
    Coordf b;
}WallPacked;

typedef struct{
    Wall *wall;
    float dst;
    Coordf pos;
}Ray;

Coord toView(const View view, const Coordf pos, const float scale)
{
    return coordAdd(view.pos, iC(pos.x*scale, pos.y*scale));
}

bool lineIntersection(const Coordf p0, const Coordf p1, const Coordf p2, const Coordf p3, Coordf *at)
{
    const Coordf s1 = {.x = p1.x - p0.x, .y = p1.y - p0.y};
    const Coordf s2 = {.x = p3.x - p2.x, .y = p3.y - p2.y};


    const float d = -s2.x * s1.y + s1.x * s2.y;

    const float s = d!=0 ? (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / d : 1000000.0f;
    const float t = d!=0 ? ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / d : 1000000.0f;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1){
        if(at){
            at->x = p0.x + (t * s1.x);
            at->y = p0.y + (t * s1.y);
        }
        return true;
    }
    return false;
}

Ray castRay(const Coordf origin, const Coordf distantPoint, Wall *map)
{
    Ray ray = {
        .wall = map,
        .dst = 6000.0f,
        .pos = {.x = 6000.0f, .y = 6000.0f}
    };
    while(map){
        float curDst = 0;
        Coordf curPos = {0};
        if(
            lineIntersection(origin, distantPoint, map->a, map->b, &curPos) &&
            (curDst = cfDist(origin, curPos)) < ray.dst
        ){
            ray.wall = map;
            ray.dst = curDst;
            ray.pos = curPos;
        }
        map = map->next;
    }
    return ray;
}

void drawFp(const View view, Wall *map, const Player player)
{
    setColor(GREY2);
    fillRectCoordLength(view.pos, iC(view.len.x,view.len.y/2));
    setColor(GREY3);
    fillRectCoordLength(iC(view.pos.x,view.len.y/2), iC(view.len.x,view.len.y/2));

    const Coordf startingPos = cfAdd(player.pos, cfRotateDeg((const Coordf){.x=1024.0f,.y=-1024.0f}, player.ang));
    const float scanAng = degReduce(player.ang+90.0f);

    const float hsec = (float)view.len.x/90;
    for(int i = 0; i < 90; i++){
        Ray ray = castRay(player.pos, cfAdd(startingPos, degMagToCf(scanAng, ((float)i/90.0f)*2048.0f)), map);
        const float viewTan = (0.5-i/90.0f) / 0.5;
        const int correctedDst = (int)(ray.dst/sqrtf(viewTan*viewTan+1.0f));
        Color c = ray.wall->c;
        c.r = clamp(c.r-((correctedDst/1000.0f)*255), 0, 256);
        c.g = clamp(c.g-((correctedDst/1000.0f)*255), 0, 256);
        c.b = clamp(c.b-((correctedDst/1000.0f)*255), 0, 256);
        setColor(c);
        fillRectCenteredCoordLength(
            iC(view.pos.x+hsec/2+i*hsec, view.pos.y+view.len.y/2),
            iC(hsec+1, imin(view.len.y, (view.len.y*120) / (correctedDst ? correctedDst : .01f)))
        );
    }
}

void drawBv(const View view, Wall *map, const Player player, const float scale, const Coordf off)
{
    setColor(BLACK);
    fillRectCoordLength(view.pos, view.len);
    (void)off;

    Wall *cur = map;
    while(cur){
        const Coord a = toView(view, cur->a, scale);
        const Coord b = toView(view, cur->b, scale);
        setColor(cur->c);
        drawLineCoords(a, b);
        cur = cur->next;
    }

    Ray rayl = castRay(player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang - 45.0f), 1000.0f)), map);
    Ray rayr = castRay(player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang + 45.0f), 1000.0f)), map);

    const Coord ppos = toView(view, player.pos, scale);
    setColor(YELLOW);
    drawLineCoords(ppos, toView(view, rayr.pos, scale));
    drawLineCoords(ppos, toView(view, rayl.pos, scale));
    fillCircleCoord(ppos, 2);
}

Player playerMoveMouse(Player player)
{
    const Scancode dirkey[4] = {SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S};
    player.ang = degReduce(player.ang + (mouse.vec.x*2)/3);
    Coord strafe = {0} ;
    for(uint i = 0; i < 4; i++)
        strafe = coordShift(strafe, i, 2*keyState(dirkey[i]));

    player.pos = cfAdd(player.pos, cfRotateDeg(CCf(strafe), player.ang));
    return player;
}

Player playerMoveKeys(Player player)
{
    player.ang = degReduce(player.ang + keyState(SDL_SCANCODE_D) - keyState(SDL_SCANCODE_A));
    player.pos = cfAdd(
        player.pos,
        degMagToCf(player.ang, 2*(float)(keyState(SDL_SCANCODE_W) - keyState(SDL_SCANCODE_S)))
    );
    return player;
}

Wall* wallNew(const Color c, const Coordf a, const Coordf b)
{
    Wall *w = calloc(1, sizeof(Wall));
    w->c = c;
    w->a = a;
    w->b = b;
    return w;
}

Wall* wallAppend(Wall *head, Wall *tail)
{
    if(!head)
        return tail;
    Wall *cur = head;
    while(cur->next)
        cur = cur->next;
    cur->next = tail;
    return head;
}

uint wallListLen(Wall *map)
{
    uint len = 0;
    while(map){
        len++;
        map = map->next;
    }
    return len;
}

void wallFreeList(Wall *list)
{
    while(list){
        Wall *next = list->next;
        free(list);
        list = next;
    }
}

WallPacked* mapPack(Wall *map)
{
    const uint len = wallListLen(map);
    if(len == 0)
        return NULL;
    WallPacked *mapPacked = calloc(len, sizeof(WallPacked));
    for(uint i = 0; i < len; i++){
        mapPacked[i].c = map->c;
        mapPacked[i].a = map->a;
        mapPacked[i].b = map->b;
        map = map->next;
    }
    return mapPacked;
}

Wall* mapUnpack(WallPacked *mapPacked, const uint len)
{
    if(len == 0)
        return NULL;
    Wall *map = NULL;
    for(uint i = 0; i < len; i++)
        map = wallAppend(map, wallNew(mapPacked[i].c, mapPacked[i].a, mapPacked[i].b));
    return map;
}

Coordf mapBounds(Wall *map)
{
    Coordf bound = {0};
    while(map)
    {
        bound.x = fmost(bound.x, map->a.x);
        bound.x = fmost(bound.x, map->b.x);
        bound.y = fmost(bound.y, map->a.y);
        bound.y = fmost(bound.y, map->b.y);
        map = map->next;
    }
    return bound;
}

Coord resizeTransform(const Length oldLen, const Length newLen, const Coord pos)
{
    return CfC(cfMul(CCf(newLen), cfDiv(CCf(pos), CCf(oldLen))));
}

// Coord rescaleTransform(const )

Coordf screenToMap(const Coord off, const float scale, const Coord pos)
{
    return cfMulf(CCf(coordSub(pos, off)), scale);
}

Coord mapToScreen(const Coord off, const float scale, const Coordf pos)
{
    return coordAdd(CfC(cfDivf(pos, scale)), off);
}

Wall* mapDefault(void)
{
    Wall *map =           wallNew(GREEN,    (const Coordf){.x=  0.0f, .y=  0.0f},   (const Coordf){.x=750.0f, .y=  0.0f});
    map = wallAppend(map, wallNew(MAGENTA,  (const Coordf){.x=750.0f, .y=  0.0f},   (const Coordf){.x=750.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(MAGENTA,  (const Coordf){.x=  0.0f, .y=  0.0f},   (const Coordf){.x=  0.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(GREEN,    (const Coordf){.x=  0.0f, .y=750.0f},   (const Coordf){.x=750.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=250.0f},   (const Coordf){.x=500.0f, .y=250.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=500.0f, .y=250.0f},   (const Coordf){.x=500.0f, .y=500.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=250.0f},   (const Coordf){.x=250.0f, .y=500.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=500.0f},   (const Coordf){.x=500.0f, .y=500.0f}));
    return map;
}

Wall* mapLoad(char *fileName)
{
    File *file = NULL;
    assertExpr(fileName);
    if((file = fopen(fileName, "rb")) == NULL){
        printf("Couldn't open file \"%s\"\n", fileName);
        return NULL;
    }
    uint len = 0;
    fread(&len, sizeof(uint), 1, file);
    if(feof(file) || len == 0){
        printf("Error reading len of map in file \"%s\"\n", fileName);
        fclose(file);
        return NULL;
    }
    WallPacked *mapPacked = calloc(len, sizeof(WallPacked));
    fread(mapPacked, sizeof(WallPacked), len, file);
    if(fgetc(file) != EOF || !feof(file))
        printf("Not at end of file after reading expected len (%u)\n", len);
    Wall* map = mapUnpack(mapPacked, len);
    free(mapPacked);
    return map;
}

void defaultMapFileName(char *fileName)
{
    uint i = 0;
    File *file = NULL;
    sprintf(fileName, "map.bork");
    while((file = fopen(fileName, "rb")) != NULL){
        fclose(file);
        i++;
        sprintf(fileName, "map(%u).bork", i);
    }
}

void mapSave(Wall *map, char *fileName)
{
    if(!map){
        printf("Map is empty, skipping save\n");
        return;
    }
    File *file = NULL;
    char defaultName[64] = {0};
    if(!fileName){
        defaultMapFileName(defaultName);
        fileName = defaultName;
    }
    file = fopen(fileName, "wb");
    const uint len = wallListLen(map);
    WallPacked *mapPacked = mapPack(map);
    fwrite(&len, sizeof(uint), 1, file);
    fwrite(mapPacked, sizeof(WallPacked), len, file);
    fclose(file);
    free(mapPacked);
}

Wall* mapEdit(Wall *map, char *fileName)
{
    float scale = 1.0f;
    // float snaplen = 24.0f;
    Coord off = {0};
    SDL_SetRelativeMouseMode(false);
    Length wlen = getWindowLen();
    bool rdrag = false;
    Coord mrd = {0};

    Color c = MAGENTA;
    while(1){
        const uint t = frameStart();
        if(checkCtrlKey(SDL_SCANCODE_Q) || checkCtrlKey(SDL_SCANCODE_W)){
            return map;
        }

        if(checkCtrlKey(SDL_SCANCODE_S)){
            mapSave(map, fileName);
            return map;
        }

        if(windowResized()){
            const Length wlenOld = wlen;
            wlen = getWindowLen();
            off = resizeTransform(wlenOld, wlen, off);
        }

        if(mouseScrolledY()){
            const float oldScale = scale;
            scale = fclamp(scale * (mouseScrolledY() > 0 ? 1.2f : .8f) , .01f, 100.0f);
            if(oldScale != scale){

            }
        }

        if(mouseBtnState(MOUSE_M) || keyState(SDL_SCANCODE_LSHIFT)){
            off = coordAdd(off, mouseMovement());
            mrd = coordAdd(mrd, mouseMovement());
        }

        if(mouseBtnPressed(MOUSE_R)){
            mrd = mouse.pos;
            rdrag = true;
        }

        if(rdrag && keyPressed(SDL_SCANCODE_ESCAPE))
            rdrag = false;

        if(rdrag && mouseBtnReleased(MOUSE_R)){
            rdrag = false;
            map = wallAppend(map, wallNew(c, screenToMap(off, scale, mrd), screenToMap(off, scale, mouse.pos)));
        }
        if(rdrag){
            setColor(c);
            drawLineCoords(mrd, mouse.pos);
            setColor(YELLOW);
            fillCircleCoord(mouse.pos, 8);
            setColor(GREEN);
            fillCircleCoord(mrd, 8);
        }

        Wall *cur = map;
        while(cur){
            const Coord a = mapToScreen(off, scale, cur->a);
            const Coord b = mapToScreen(off, scale, cur->b);
            setColor(cur->c);
            drawLineCoords(a, b);
            cur = cur->next;
        }

        setColor(RED);
        drawHLine(0, off.y, wlen.x);
        drawVLine(off.x, 0, wlen.y);

        frameEnd(t);
    }
}

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

        if(keyPressed(SDL_SCANCODE_ESCAPE))
            return 0;

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
