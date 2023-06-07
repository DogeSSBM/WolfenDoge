#include "DogeLib/Includes.h"
#define WALLS 8
#define BOUNDS 750.0f

typedef struct{
    Coord pos;
    Coord mid;
    Length len;
}View;

void calcViews(View views[4])
{
    const Length hlen = getWindowMid();
    const Length qlen = coordDivi(hlen, 2);
    Coord pos = {0};
    for(uint i = 0; i < 4; i++){
        views[i].len = hlen;
        views[i].pos = pos;
        views[i].mid = coordAdd(pos, qlen);
        pos = coordShift(pos, dirROR(i), dirLR(dirROR(i))?hlen.x:hlen.y);
    }
}

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
    fillRectCoordLength(view.pos, iC(view.len.x,view.mid.y));
    setColor(GREY3);
    fillRectCoordLength(iC(view.pos.x,view.mid.y), iC(view.len.x,view.mid.y));

    const float hsec = (float)view.len.x/90;
    for(uint i = 0; i < 90; i++){
        int hpos = hsec+i*hsec;
        float dst = 1000.0f;
        Coordf pos = cfAdd(player.pos, degMagToCf((player.ang-45.0f)+i, 1000.0f));
        for(int w = 0; w < WALLS; w++){
            float cur = 0;
            if(lineIntersection(player.pos, pos, map[w].a, map[w].b, &pos)){
                if((cur = cfDist(player.pos, pos)) < dst){
                    dst = cur;
                    setColor(map[w].c);
                }
            }
        }
        fillRectCenteredCoordLength(
            iC(view.pos.x+hpos, view.pos.y+view.len.y/2),
            iC(hsec+1, view.len.y-(view.len.y*(dst/1000.0f)))
        );
    }

}

bool inWallIndexes(int *wallIndexes, const int index)
{
    for(int i = 0; i < WALLS && wallIndexes[i] != -1; i++)
        if(wallIndexes[i] == index)
            return true;
    return false;
}

void drawBv(const View view, const Wall map[WALLS], const Player player)
{
    setColor(GREY2);
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

Player playerMove(Player player)
{
    player.ang = degReduce(player.ang + keyState(SDL_SCANCODE_D) - keyState(SDL_SCANCODE_A));
    player.pos =  cfAdd(player.pos, degMagToCf(player.ang, 2*(float)(keyState(SDL_SCANCODE_W) - keyState(SDL_SCANCODE_S))));
    return player;
}

int main(void)
{
    init();
    gfx.outlined = false;
    setWindowResizable(false);
    setWindowLen(iC(800, 800));
    winSetPosCoord(coordAddi(coordDivi(getWinDisplayLen(), 2), -400));
    setColor(WHITE);
    View views[4] = {0};

    const Wall map[WALLS] = {
        { .c = GREEN,   .a={.x=  0.0f, .y=  0.0f}, .b={.x=750.0f, .y=  0.0f} },
        { .c = MAGENTA, .a={.x=750.0f, .y=  0.0f}, .b={.x=750.0f, .y=750.0f} },
        { .c = GREEN,   .a={.x=  0.0f, .y=  0.0f}, .b={.x=  0.0f, .y=750.0f} },
        { .c = MAGENTA, .a={.x=  0.0f, .y=750.0f}, .b={.x=750.0f, .y=750.0f} },

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

        calcViews(views);

        player = playerMove(player);

        drawFp(views[0], map, player);
        drawBv(views[1], map, player);

        frameEnd(t);
    }
    return 0;
}
