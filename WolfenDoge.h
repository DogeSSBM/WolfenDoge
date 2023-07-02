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

typedef enum{W_WALL, W_WIND}WallType;
typedef struct Wall{
    Color c;
    Coordf a;
    Coordf b;
    WallType type;
    union{
        struct{
            Color ctop;
            float height;
            float top;
        }wind;
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

Coordf fC(const float x, const float y)
{
    return (const Coordf){.x=x, .y=y};
}

Coord iiC(const int i)
{
    return (const Coord){.x=i, .y=i};
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

Coordf cfSnapMid(const Coordf pos, const float scale)
{
    return cfSnap(cfAddf(pos, scale/2), scale);
}

u8* colorIndex(Color *c, const int i)
{
    if(iabs(i)%3 == 0)
        return &(c->r);
    if(iabs(i)%3 == 1)
        return &(c->g);
    return &(c->b);
}

bool keyCtrlState(void)
{
    return keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL);
}

bool keyShiftState(void)
{
    return keyState(SDL_SCANCODE_LSHIFT) || keyState(SDL_SCANCODE_RSHIFT);
}

bool checkCtrlKey(const Scancode key)
{
    return keyPressed(key) && keyCtrlState();
}

Coord toView(const View view, const Coordf pos, const float scale)
{
    return coordAdd(view.pos, iC(pos.x*scale, pos.y*scale));
}

bool inView(const View view, const Coord pos)
{
    return inBound(pos.x, view.pos.x, view.pos.x+view.len.x) && inBound(pos.y, view.pos.y, view.pos.y+view.len.y);
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

Offset wasdKeyStateOffset(void)
{
    return (const Offset){
        .x = keyState(SDL_SCANCODE_D) - keyState(SDL_SCANCODE_A),
        .y = keyState(SDL_SCANCODE_S) - keyState(SDL_SCANCODE_W)
    };
}

Offset arrowKeyStateOffset(void)
{
    return (const Offset){
        .x = keyState(SDL_SCANCODE_RIGHT) - keyState(SDL_SCANCODE_LEFT),
        .y = keyState(SDL_SCANCODE_DOWN) - keyState(SDL_SCANCODE_UP)
    };
}

Offset dirKeyStateOffset(void)
{
    return coordLeast(coordAdd(wasdKeyStateOffset(), arrowKeyStateOffset()), iC(1,1));
}

Offset wasdKeyPressedOffset(void)
{
    return (const Offset){
        .x = keyPressed(SDL_SCANCODE_D) - keyPressed(SDL_SCANCODE_A),
        .y = keyPressed(SDL_SCANCODE_S) - keyPressed(SDL_SCANCODE_W)
    };
}

Offset arrowKeyPressedOffset(void)
{
    return (const Offset){
        .x = keyPressed(SDL_SCANCODE_RIGHT) - keyPressed(SDL_SCANCODE_LEFT),
        .y = keyPressed(SDL_SCANCODE_DOWN) - keyPressed(SDL_SCANCODE_UP)
    };
}

Offset dirKeyPressedOffset(void)
{
    return coordLeast(coordAdd(wasdKeyPressedOffset(), arrowKeyPressedOffset()), iC(1,1));
}

Ray castRayMin(const Coordf origin, const Coordf distantPoint, Wall *map, const bool ignoreWind, const float min)
{
    Ray ray = {
        .wall = map,
        .dst = 60000.0f,
        .pos = distantPoint
    };
    while(map){
        if(ignoreWind && map->type == W_WIND){
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
    return ray;
}

Ray castRay(const Coordf origin, const Coordf distantPoint, Wall *map, const bool ignoreWind)
{
    return castRayMin(origin, distantPoint, map, ignoreWind, -1.0f);
}

void drawWall(const View view, const Ray ray, const int xpos, const int ymid, const int dst, const float hsec)
{
    const int height = (view.len.y*120) / fmax(dst, .01f);
    if(ray.wall){
        setColor((const Color){
            .r = clamp(ray.wall->c.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .g = clamp(ray.wall->c.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .b = clamp(ray.wall->c.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
        });
        if(ray.wall->type == W_WIND){
            const int xc = xpos-hsec/2;
            const int xr = xc+hsec+1.0f;
            int oldbot = ymid+height/2;
            const int bot = oldbot - (int)(ray.wall->wind.height * (float)height);
            fillRectCoords(iC(xc, oldbot), iC(xr, bot));

            setColor((const Color){
                .r = clamp(ray.wall->wind.ctop.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
                .g = clamp(ray.wall->wind.ctop.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
                .b = clamp(ray.wall->wind.ctop.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
            });
            int oldtop = ymid-height/2;
            const int top = oldtop + (int)(ray.wall->wind.height * (float)height);
            fillRectCoords(iC(xc, oldtop), iC(xr, top));
            return;
        }
        const int ypos = ymid;
        const int ylen = imin(view.len.y, height);
        fillRectCenteredCoordLength(
            iC(xpos, ypos),
            iC(hsec+1, ylen)
        );
        return;
    }else{
        setColor(BLACK);
        fillRectCenteredCoordLength(
            iC(xpos, ymid),
            iC(hsec+1, imax(view.len.y/64, 1))
        );
    }
    return;
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
        if(wray1.wall && wray1.wall->type == W_WIND){
            const Ray wray2 = castRayMin(player.pos, farpos, map, false, wray1.dst+1.0f);
            if(wray2.wall && wray2.wall->type == W_WIND)
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
        setColor(cur->c);
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

Player playerMoveMouse(Player player, Wall *map)
{
    player.ang = degReduce(player.ang + (mouse.vec.x*2)/3);
    if(castRay(
        player.pos,
        cfAdd(player.pos, cfRotateDeg(cfMulf(CCf(wasdKeyStateOffset()), 10.0f), player.ang+90.0f)),
        map, false
    ).dst < 10.0f)
        return player;
    player.pos = cfAdd(player.pos, cfRotateDeg(CCf(coordMuli(wasdKeyStateOffset(), 2)), player.ang+90.0f));
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
