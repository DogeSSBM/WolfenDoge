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

void editMap(Wall *map)
{
    Player player = {.pos = {.x=125.0f, .y=125.0f}};
    Coordf off = fC(0,0);
    SDL_SetRelativeMouseMode(false);

    while(1){
        const uint t = frameStart();

        const Length wlen = getWindowLen();
        const View birdsView = {.len = wlen};
        const View firstView = {.len = coordDivi(wlen, 4), .pos = {.x=wlen.x-wlen.x/4}};
        const Coordf bound = mapBounds(map);
        float scale = (float)coordMin(birdsView.len) / cfMax(bound);
        if(keyPressed(SDL_SCANCODE_ESCAPE))
            return;

        player = playerMoveKeys(player);

        if(mouseBtnState(MOUSE_M))
            off = cfAdd(off, cfMulf(CCf(mouseMovement()), scale));
        // Coordf mouseMapPos = toView()

        drawBv(birdsView, map, player, scale, off);
        drawFp(firstView, map, player);

        frameEnd(t);
    }
}

int main(void)
{
    init();
    gfx.outlined = false;
    winSetPosCoord(coordAddi(coordDivi(getWinDisplayLen(), 2), -400));
    setColor(WHITE);
    Wall *map =           wallNew(GREEN,    (const Coordf){.x=  0.0f, .y=  0.0f},   (const Coordf){.x=750.0f, .y=  0.0f});
    map = wallAppend(map, wallNew(MAGENTA,  (const Coordf){.x=750.0f, .y=  0.0f},   (const Coordf){.x=750.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(MAGENTA,  (const Coordf){.x=  0.0f, .y=  0.0f},   (const Coordf){.x=  0.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(GREEN,    (const Coordf){.x=  0.0f, .y=750.0f},   (const Coordf){.x=750.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=250.0f},   (const Coordf){.x=500.0f, .y=250.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=500.0f, .y=250.0f},   (const Coordf){.x=500.0f, .y=500.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=250.0f},   (const Coordf){.x=250.0f, .y=500.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=500.0f},   (const Coordf){.x=500.0f, .y=500.0f}));

    editMap(map);
    SDL_SetRelativeMouseMode(true);
    
    Player player = {.pos = {.x=125.0f, .y=125.0f}};

    while(1){
        const uint t = frameStart();

        if(keyPressed(SDL_SCANCODE_ESCAPE))
            return 0;

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
