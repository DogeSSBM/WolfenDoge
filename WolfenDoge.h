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

typedef enum{W_WALL, W_WIND, W_TRIG, W_DOOR}WallType;
typedef struct Wall{
    Color color;
    Coordf a;
    Coordf b;
    WallType type;
    union{
        struct{
            Color topColor;
            float height;
            float top;
        }wind;
        struct{
            uint id;
            float pos;
            bool state;
            float speed;
            Direction closeDir;
        }door;
        struct{
            uint id;
            Coordf c;
            Coordf d;
        }trig;
    };
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

typedef struct{
    Coord spos;
    Coordf mpos;
    Coord sposd;
    Coordf mposd;
    Coord ssnap;
    Coordf msnap;
    Coord ssnapd;
    Coordf msnapd;
    bool drag;
}Minfo;

typedef struct Selection{
    Wall *wall;
    Coordf posOrig;
    Coordf *pos;
    struct Selection *next;
}Selection;

#include "Decls.h"
#include "Map.h"

Coord toView(const View view, const Coordf pos, const float scale)
{
    return coordAdd(view.pos, iC(pos.x*scale, pos.y*scale));
}

bool inView(const View view, const Coord pos)
{
    return inBound(pos.x, view.pos.x, view.pos.x+view.len.x) && inBound(pos.y, view.pos.y, view.pos.y+view.len.y);
}

Direction viewBoundIntersect(Wall bounds[4], const Coordf a, const Coordf b, Coordf *at)
{
    for(Direction d = 0; d < 4; d++){
        if(lineIntersection(bounds[d].a, bounds[d].b, a, b, at))
            return d;
    }
    return 4;
}

bool limitViewBounds(const View view, Coord *a, Coord *b)
{
    const Coordf fa = CCf(*a);
    const Coordf fb = CCf(*b);
    const Coordf vpos = CCf(view.pos);
    const Coordf vlen = CCf(view.len);
    Wall bounds[4] = {
        (Wall){.a = vpos,                       .b = fC(vpos.x+vlen.x, vpos.y)},
        (Wall){.a = fC(vpos.x+vlen.x, vpos.y),  .b = cfAdd(vpos, vlen)},
        (Wall){.a = fC(vpos.x, vpos.y+vlen.y),  .b = cfAdd(vpos, vlen)},
        (Wall){.a = vpos,                       .b = fC(vpos.x, vpos.y+vlen.y)}
    };

    if(!inView(view, *a)){
        Coordf na = fa;
        Direction da = viewBoundIntersect(bounds, fa, fb, &na);
        if(inView(view, *b)){
            *a = CfC(na);
            return true;
        }
        Coordf nb = fb;
        Direction db = viewBoundIntersect(bounds, fb, fa, &nb);
        if(da == 4 || db == 4)
            return false;
        *a = CfC(na);
        *b = CfC(nb);
        return true;
    }
    if(!inView(view, *b)){
        Coordf nb = fb;
        Direction db = viewBoundIntersect(bounds, fb, fa, &nb);
        if(inView(view, *a)){
            *b = CfC(nb);
            return true;
        }
        Coordf na = fa;
        Direction da = viewBoundIntersect(bounds, fa, fb, &na);
        if(da == 4 || db == 4)
            return false;
        *a = CfC(na);
        *b = CfC(nb);
        return true;
    }
    return true;
}

Ray castRayMin(const Coordf origin, const Coordf distantPoint, Wall *map, const bool solidOnly, const float min)
{
    Ray ray = {
        .wall = map,
        .dst = 60000.0f,
        .pos = distantPoint
    };
    while(map){
        if(solidOnly && (map->type == W_WIND || map->type == W_TRIG || (map->type == W_DOOR && map->door.pos < 1.0f))){
            map = map->next;
            continue;
        }
        float curDst = 0;
        Coordf curPos = {0};
        if(
            lineIntersection(origin, distantPoint, map->a, map->b, &curPos) &&
            (curDst = cfDist(origin, curPos)) > min && curDst < ray.dst
        ){
            ray.wall = map;
            ray.dst = curDst;
            ray.pos = curPos;
        }
        map = map->next;
    }
    if(ray.dst == 60000.0f)
        ray.wall = NULL;
    return ray;
}

Ray castRay(const Coordf origin, const Coordf distantPoint, Wall *map, const bool solidOnly)
{
    return castRayMin(origin, distantPoint, map, solidOnly, -1.0f);
}

void drawWall(const View view, const Ray ray, const int xpos, const int ymid, const int dst, const float hsec)
{
    const int height = (view.len.y*120) / fmax(dst, .01f);
    if(!ray.wall){
        setColor(BLACK);
        fillRectCenteredCoordLength(
            iC(xpos, ymid),
            iC(hsec+1, imax(view.len.y/64, 1))
        );
        return;
    }
    setColor((const Color){
        .r = clamp(ray.wall->color.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
        .g = clamp(ray.wall->color.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
        .b = clamp(ray.wall->color.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
    });
    if(ray.wall->type == W_WIND){
        const int boundL = xpos-hsec/2;
        const int boundR = boundL+hsec+1.0f;
        const int boundBotLower = ymid+height/2;
        const int boundBotUpper = boundBotLower - (int)(ray.wall->wind.height * (float)height);
        fillRectCoords(iC(boundL, boundBotLower), iC(boundR, boundBotUpper));

        setColor((const Color){
            .r = clamp(ray.wall->wind.topColor.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .g = clamp(ray.wall->wind.topColor.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .b = clamp(ray.wall->wind.topColor.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
        });
        const int boundTopUpper = ymid-height/2;
        const int boundTopLower = boundTopUpper + (int)(ray.wall->wind.height * (float)height);
        fillRectCoords(iC(boundL, boundTopUpper), iC(boundR, boundTopLower));
        return;
    }
    if(ray.wall->type == W_DOOR){
        if(ray.wall->door.pos == 0.0f)
            return;
        if(dirUD(ray.wall->door.closeDir)){
            const int boundL = xpos-hsec/2;
            const int boundR = boundL+hsec+1.0f;
            const int doorHeight = height * ray.wall->door.pos;
            const int boundTop = ymid-height/2 + (ray.wall->door.closeDir == DIR_U * (height - doorHeight));
            const int boundBot = boundTop + doorHeight;
            fillRectCoords(iC(boundL, boundTop), iC(boundR, boundBot));
        }
        return;
    }
    const int ypos = ymid;
    const int ylen = imin(view.len.y, height);
    fillRectCenteredCoordLength(
        iC(xpos, ypos),
        iC(hsec+1, ylen)
    );
}

void drawFp(const View view, Wall *map, const Player player, const Length wlen)
{
    setColor(GREY2);
    fillRectCoordLength(view.pos, iC(view.len.x,view.len.y/2));
    setColor(GREY3);
    fillRectCoordLength(iC(view.pos.x,view.len.y/2), iC(view.len.x,view.len.y/2));

    const Coordf startingPos = cfAdd(player.pos, cfRotateDeg((const Coordf){.x=2048.0f,.y=-2048.0f}, player.ang));
    const float scanAng = degReduce(player.ang+90.0f);
    const float hsec = (float)view.len.x/90;
    const int ymid = view.pos.y+view.len.y/2;
    for(int i = 0; i < 90; i++){
        const Coordf farpos = cfAdd(startingPos, degMagToCf(scanAng, ((float)i/90.0f)*4096.0f));
        const Ray ray = castRay(player.pos, farpos, map, true);
        const float viewTan = (0.5-i/90.0f) / 0.5;
        const int correctedDst = (int)(ray.dst/sqrtf(viewTan*viewTan+1.0f));
        const int xpos = view.pos.x+hsec/2+i*hsec;
        drawWall(view, ray, xpos, ymid, correctedDst, hsec);
        const Ray wray1 = castRay(player.pos, farpos, map, false);
        if(wray1.wall && wray1.wall->type != W_WALL){
            const Ray wray2 = castRayMin(player.pos, farpos, map, false, wray1.dst+1.0f);
            if(wray2.wall && wray2.wall->type != W_WALL)
                drawWall(view, wray2, xpos, ymid, (int)(wray2.dst/sqrtf(viewTan*viewTan+1.0f)), hsec);
            drawWall(view, wray1, xpos, ymid, (int)(wray1.dst/sqrtf(viewTan*viewTan+1.0f)), hsec);
        }
    }
    char buf[32] = {0};
    sprintf(buf, "%+14.6f, %+14.6f", player.pos.x, player.pos.y);
    const uint tscale = (wlen.x/3)/10;
    setTextSize(tscale);
    Texture *texture = textTexture(buf);
    setColor(BLACK);
    setTextColor(WHITE);
    const Coord pos = coordSub(wlen, textureLen(texture));
    fillRectCoordLength(pos, textureLen(texture));
    drawTextureCoord(texture, pos);
    freeTexture(texture);
}

void drawBv(const View view, Wall *map, const Player player, const float scale, const Coordf off)
{
    setColor(BLACK);
    fillRectCoordLength(view.pos, view.len);
    (void)off;
    Wall *cur = map;
    const Length hlen = coordDivi(view.len, 2);
    while(cur){
        Coord a = coordAdd(toView(view, cfSub(cur->a, player.pos), scale), hlen);
        Coord b = coordAdd(toView(view, cfSub(cur->b, player.pos), scale), hlen);
        setColor(cur->color);
        if(limitViewBounds(view, &a, &b))
            drawLineCoords(a, b);
        cur = cur->next;
    }

    Coord ppos = coordAdd(view.pos, hlen);
    Coord rpos = coordAdd(toView(view, cfSub(castRay(
        player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang + 45.0f), 6000.0f)),
        map, false
    ).pos, player.pos), scale), hlen);
    Coord lpos = coordAdd(toView(view, cfSub(castRay(
        player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang - 45.0f), 6000.0f)),
        map, false
    ).pos, player.pos), scale), hlen);
    limitViewBounds(view, &ppos, &rpos);
    limitViewBounds(view, &ppos, &lpos);
    setColor(YELLOW);
    drawLineCoords(ppos, rpos);
    drawLineCoords(ppos, lpos);
    fillCircleCoord(ppos, 2);
}

float triSign(const Coordf a, const Coordf b, const Coordf c)
{
    return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}

bool cfInTri(Coordf pos, Coordf a, Coordf b, Coordf c)
{
    const float s1 = triSign(pos, a, b);
    const float s2 = triSign(pos, b, c);
    const float s3 = triSign(pos, c, a);
    return !(((s1 < 0) || (s2 < 0) || (s3 < 0)) && ((s1 > 0) || (s2 > 0) || (s3 > 0)));
}

void mapUpdateIdState(Wall *map, const uint id, const bool state)
{
    while(map){
        if(map->type == W_DOOR && map->door.id == id)
            map->door.state = state;
        map = map->next;
    }
}

void mapUpdateTriggers(const Coordf oldPos, const Coordf newPos, Wall *map)
{
    while(map){
        if(map->type == W_TRIG){
            const bool oldState = cfInTri(oldPos, map->a, map->b, map->trig.d) || cfInTri(oldPos, map->b, map->trig.d, map->trig.c);
            const bool newState = cfInTri(newPos, map->a, map->b, map->trig.d) || cfInTri(newPos, map->b, map->trig.d, map->trig.c);
            if(oldState != newState)
                mapUpdateIdState(map, map->trig.id, newState);
        }
        map = map->next;
    }
}

void mapUpdateDynamics(Wall *map)
{
    while(map){
        if(map->type == W_DOOR){
            map->door.pos += map->door.state ? -map->door.speed : map->door.speed;
            if(map->door.pos < 0.0f)
                map->door.pos = 0.0f;
            if(map->door.pos > 1.0f)
                map->door.pos = 1.0f;
        }
        map = map->next;
    }
}

Player playerMoveMouse(Player player, Wall *map)
{
    const Coordf oldPos = player.pos;
    player.ang = degReduce(player.ang + (mouse.vec.x*2)/3);
    if(castRay(
        player.pos,
        cfAdd(player.pos, cfRotateDeg(cfMulf(CCf(wasdKeyStateOffset()), 10.0f), player.ang+90.0f)),
        map, true
    ).dst > 10.0f)
        player.pos = cfAdd(player.pos, cfRotateDeg(CCf(coordMuli(wasdKeyStateOffset(), 2)), player.ang+90.0f));
    if(!cfSame(oldPos, player.pos))
        mapUpdateTriggers(oldPos, player.pos, map);
    mapUpdateDynamics(map);
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
