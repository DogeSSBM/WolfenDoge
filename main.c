#include "DogeLib/Includes.h"
#define WALLS 8
#define BOUNDS 750.0f

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
}Wall;

Coord toView(const View view, const Coordf pos, const float scale)
{
    return coordAdd(view.pos, iC(pos.x*scale, pos.y*scale));
}

bool lineIntersection(const Coordf p0, const Coordf p1, const Coordf p2, const Coordf p3, Coordf *at)
{
    const Coordf s1 = {.x = p1.x - p0.x, .y = p1.y - p0.y};
    const Coordf s2 = {.x = p3.x - p2.x, .y = p3.y - p2.y};

    const float s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
    const float t = ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1){
        if(at){
            at->x = p0.x + (t * s1.x);
            at->y = p0.y + (t * s1.y);
        }
        return true;
    }
    return false;
}

void drawFp(const View view, const Wall map[WALLS], const Player player)
{
    setColor(GREY2);
    fillRectCoordLength(view.pos, iC(view.len.x,view.len.y/2));
    setColor(GREY3);
    fillRectCoordLength(iC(view.pos.x,view.len.y/2), iC(view.len.x,view.len.y/2));

    const Coordf startingPos = cfAdd(player.pos, cfRotateDeg((const Coordf){.x=1024.0f,.y=-1024.0f}, player.ang));
    const float scanAng = degReduce(player.ang+90.0f);
    const float hsec = (float)view.len.x/90;
    for(int i = 0; i < 90; i++){
        float dst = 6000.0f;
        Coordf pos = cfAdd(startingPos, degMagToCf(scanAng, ((float)i/90.0f)*2048.0f));
        for(int w = 0; w < WALLS; w++){
            float cur = 0;
            if(
                lineIntersection(player.pos, pos, map[w].a, map[w].b, &pos) &&
                (cur = cfDist(player.pos, pos)) < dst
            ){
                dst = cur;
                Color c = map[w].c;
                c.r = clamp(c.r-((dst/1000.0f)*255), 0, 256);
                c.g = clamp(c.g-((dst/1000.0f)*255), 0, 256);
                c.b = clamp(c.b-((dst/1000.0f)*255), 0, 256);
                setColor(c);
            }
        }
        
        const float height = view.len.y-(view.len.y*(dst/1000.0f));
        fillRectCenteredCoordLength(
            iC(view.pos.x+hsec/2+i*hsec, view.pos.y+view.len.y/2),
            iC(hsec+1, (int)height)
        );
    }
}

void drawBv(const View view, const Wall map[WALLS], const Player player)
{
    setColor(BLACK);
    fillRectCoordLength(view.pos, view.len);
    const float scale = (float)coordMin(view.len) / BOUNDS;

    for(uint i = 0; i < WALLS; i++){
        const Coord a = toView(view, map[i].a, scale);
        const Coord b = toView(view, map[i].b, scale);
        setColor(map[i].c);
        drawLineCoords(a, b);
    }
    const Coordf posl = cfAdd(player.pos, degMagToCf(degReduce(player.ang - 45.0f), 1000.0f));
    const Coordf posr = cfAdd(player.pos, degMagToCf(degReduce(player.ang + 45.0f), 1000.0f));
    Coordf minl = {.x=1000.0f, .y=0};
    float dstl = 1000.0f;
    Coordf minr = {.x=1000.0f, .y=0};
    float dstr = 1000.0f;

    for(uint i = 0; i < WALLS; i++){
        Coordf pos = {0};
        float dst = 0;
        if(lineIntersection(player.pos, posl, map[i].a, map[i].b, &pos)){
            if((dst = cfDist(player.pos, pos)) < dstl){
                dstl = dst;
                minl = pos;
            }
        }
        if(lineIntersection(player.pos, posr, map[i].a, map[i].b, &pos)){
            if((dst = cfDist(player.pos, pos)) < dstr){
                dstr = dst;
                minr = pos;
            }
        }
    }
    const Coord ppos = toView(view, player.pos, scale);
    setColor(YELLOW);
    drawLineCoords(ppos, toView(view, minr, scale));
    drawLineCoords(ppos, toView(view, minl, scale));
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

int main(void)
{
    init();
    gfx.outlined = false;
    winSetPosCoord(coordAddi(coordDivi(getWinDisplayLen(), 2), -400));
    setColor(WHITE);
    SDL_SetRelativeMouseMode(true);
    const Wall map[WALLS] = {
        { .c = GREEN,   .a={.x=  0.0f, .y=  0.0f}, .b={.x=750.0f, .y=  0.0f} },
        { .c = MAGENTA, .a={.x=750.0f, .y=  0.0f}, .b={.x=750.0f, .y=750.0f} },
        { .c = MAGENTA, .a={.x=  0.0f, .y=  0.0f}, .b={.x=  0.0f, .y=750.0f} },
        { .c = GREEN,   .a={.x=  0.0f, .y=750.0f}, .b={.x=750.0f, .y=750.0f} },

        { .c = BLUE,    .a={.x=250.0f, .y=250.0f}, .b={.x=500.0f, .y=250.0f} },
        { .c = BLUE,    .a={.x=500.0f, .y=250.0f}, .b={.x=500.0f, .y=500.0f} },
        { .c = BLUE,    .a={.x=250.0f, .y=250.0f}, .b={.x=250.0f, .y=500.0f} },
        { .c = BLUE,    .a={.x=250.0f, .y=500.0f}, .b={.x=500.0f, .y=500.0f} }
    };

    Player player = {.pos = {.x=125.0f, .y=125.0f}};

    while(1){
        const uint t = frameStart();

        if(keyPressed(SDL_SCANCODE_ESCAPE))
            return 0;

        const Length wlen = getWindowLen();
        player = playerMoveMouse(player);

        drawFp((View){.len = wlen}, map, player);
        drawBv((View){.len = coordDivi(wlen, 4), .pos = {.x=wlen.x/4*3}}, map, player);

        frameEnd(t);
    }
    return 0;
}
