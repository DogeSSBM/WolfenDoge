#ifndef WOLFENDOGE_H
#define WOLFENDOGE_H

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
    Wall *wall;
    float dst;
    Coordf pos;
}Ray;

typedef struct{
    Color c;
    Coordf a;
    Coordf b;
}WallPacked;

#include "Decls.h"
#include "Map.h"

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

bool cfSame(const Coordf a, const Coordf b)
{
    return a.x == b.x && a.y == b.y;
}

Coordf cfModf(const Coordf pos, const float mod)
{
    return (const Coordf){
        .x = fmod(pos.x, mod),
        .y = fmod(pos.y, mod)
    };
}

Coordf cfSub(const Coordf a, const Coordf b)
{
    return (const Coordf){
        .x = a.x-b.x,
        .y = a.y-b.y
    };
}

Coordf cfAddf(const Coordf pos, const float f)
{
    return (const Coordf){
        .x=pos.x+f,
        .y=pos.y+f
    };
}

Coordf cfSnap(const Coordf pos, const float scale)
{
    return cfSub(pos, cfModf(pos, scale));
}

u8* colorIndex(Color *c, const int i)
{
    if(iabs(i)%3 == 0)
        return &(c->r);
    if(iabs(i)%3 == 1)
        return &(c->g);
    return &(c->b);
}

bool checkCtrlKey(const Scancode key)
{
    return keyPressed(key) && (keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL));
}

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
        .dst = 60000.0f,
        .pos = {.x = 60000.0f, .y = 60000.0f}
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

    const Coordf startingPos = cfAdd(player.pos, cfRotateDeg((const Coordf){.x=2048.0f,.y=-2048.0f}, player.ang));
    const float scanAng = degReduce(player.ang+90.0f);

    const float hsec = (float)view.len.x/90;
    for(int i = 0; i < 90; i++){
        Ray ray = castRay(player.pos, cfAdd(startingPos, degMagToCf(scanAng, ((float)i/90.0f)*4096.0f)), map);
        const float viewTan = (0.5-i/90.0f) / 0.5;
        const int correctedDst = (int)(ray.dst/sqrtf(viewTan*viewTan+1.0f));
        if(ray.wall){
            Color c = ray.wall->c;
            c.r = clamp(c.r-(((correctedDst*1.2f)/2000.0f)*255), 0, 256);
            c.g = clamp(c.g-(((correctedDst*1.2f)/2000.0f)*255), 0, 256);
            c.b = clamp(c.b-(((correctedDst*1.2f)/2000.0f)*255), 0, 256);
            setColor(c);
        }else{
            setColor(BLACK);
        }
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

#endif /* end of include guard: WOLFENDOGE_H */
