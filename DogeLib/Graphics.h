#ifndef GRAPHICS_H
#define GRAPHICS_H

#define DEFAULT_WINDOW_XLEN    800
#define DEFAULT_WINDOW_YLEN    600

struct{
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    uint fontSize;
    Color fontColor;
    Color defaultColor;
    Length restoreLen;
    Length prvLen;
    Length winLen;
    bool outlined;
    u32 winFlags;
}gfx = {0};

void setWindowMode(const WindowMode mode)
{
    SDL_SetWindowFullscreen(gfx.window, mode);
}

void setWindowResizable(const bool resizable)
{
    SDL_SetWindowResizable(gfx.window, resizable);
}

bool getWindowResizable(void)
{
    return SDL_WINDOW_RESIZABLE & SDL_GetWindowFlags(gfx.window);
}

void setWindowLen(const Length len)
{
    SDL_SetWindowSize(gfx.window, len.x, len.y);
}

Length getWindowLen(void)
{
    Length ret = {0};
    SDL_GetWindowSize(gfx.window, &ret.x, &ret.y);
    return ret;
}

bool windowResized(void)
{
    return !coordSame(gfx.prvLen, gfx.winLen);
}

Length getWindowMid(void)
{
    Length ret = {0};
    SDL_GetWindowSize(gfx.window, &ret.x, &ret.y);
    return coordDivi(ret, 2);
}

bool coordInWindow(const Coord pos)
{
    const Length window = getWindowLen();
    return inBound(pos.x, 0, window.x) && inBound(pos.y, 0, window.y);
}

Length maximizeWindow(void)
{
    Length len = {0};
    gfx.restoreLen = getWindowLen();
    SDL_MaximizeWindow(gfx.window);
    SDL_GetWindowSize(gfx.window, &len.x, &len.y);
    return len;
}

Length restoreWindow(void)
{
    SDL_RestoreWindow(gfx.window);
    if(getWindowResizable())
    SDL_SetWindowSize(gfx.window, gfx.restoreLen.x, gfx.restoreLen.y);
    return gfx.restoreLen;
}

void setWindowSize(const uint x, const uint y)
{
    SDL_SetWindowSize(gfx.window, x, y);
}

void setBlend(const BlendMode mode)
{
    SDL_SetRenderDrawBlendMode(gfx.renderer, mode);
}

void drawPixel(const uint x, const uint y)
{
    SDL_RenderDrawPoint(gfx.renderer, x, y);
}

void drawPixelCoord(const Coord pos)
{
    SDL_RenderDrawPoint(gfx.renderer, pos.x, pos.y);
}

void drawLine(const uint x1, const uint y1, const uint x2, const uint y2)
{
    SDL_RenderDrawLine(gfx.renderer, x1, y1, x2, y2);
}

void drawLineCoords(const Coord pos1, const Coord pos2)
{
    SDL_RenderDrawLine(gfx.renderer, pos1.x, pos1.y, pos2.x, pos2.y);
}

void drawLineCoordPair(const CoordPair pair)
{
    SDL_RenderDrawLine(gfx.renderer, pair.pos1.x, pair.pos1.y, pair.pos2.x, pair.pos2.y);
}

void drawHLine(const uint x, const uint y, int len)
{
    SDL_RenderDrawLine(gfx.renderer, x, y, x+len, y);
}

void drawVLine(const uint x, const uint y, int len)
{
    SDL_RenderDrawLine(gfx.renderer, x, y, x, y+len);
}

void drawRect(const uint x, const uint y, const uint xlen, const uint ylen)
{
    SDL_RenderDrawRect(
        gfx.renderer,
        &(const Rect){.x = x, .y = y, .w = xlen, .h = ylen}
    );
}

void drawRectCentered(const uint x, const uint y, const uint xlen, const uint ylen)
{
    SDL_RenderDrawRect(
        gfx.renderer,
        &(const Rect){.x = x - xlen/2, .y = y - ylen/2, .w = xlen, .h = ylen}
    );
}

void fillRect(const uint x, const uint y, const uint xlen, const uint ylen)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){.x = x, .y = y, .w = xlen, .h = ylen}
    );
}

void fillRectCentered(const uint x, const uint y, const uint xlen, const uint ylen)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){.x = x - xlen/2, .y = y - ylen/2, .w = xlen, .h = ylen}
    );
}

void drawRectCoords(const Coord pos1, const Coord pos2)
{
    SDL_RenderDrawRect(
        gfx.renderer,
        &(const Rect){
            .x = pos1.x < pos2.x? pos1.x: pos2.x,
            .y = pos1.y < pos2.y? pos1.y: pos2.y,
            .w = abs(pos1.x-pos2.x),
            .h = abs(pos1.y-pos2.y)
        }
    );
}

void drawRectCenteredCoords(const Coord pos1, const Coord pos2)
{
    SDL_RenderDrawRect(
        gfx.renderer,
        &(const Rect){
            .x = (pos1.x < pos2.x? pos1.x: pos2.x) - abs(pos1.x-pos2.x)/2,
            .y = (pos1.y < pos2.y? pos1.y: pos2.y) - abs(pos1.y-pos2.y)/2,
            .w = abs(pos1.x-pos2.x),
            .h = abs(pos1.y-pos2.y)
        }
    );
}

void fillRectCoords(const Coord pos1, const Coord pos2)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){
            .x = pos1.x < pos2.x? pos1.x: pos2.x,
            .y = pos1.y < pos2.y? pos1.y: pos2.y,
            .w = abs(pos1.x-pos2.x),
            .h = abs(pos1.y-pos2.y)
        }
    );
}

void fillRectCenteredCoords(const Coord pos1, const Coord pos2)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){
            .x = (pos1.x < pos2.x? pos1.x: pos2.x) - abs(pos1.x-pos2.x)/2,
            .y = (pos1.y < pos2.y? pos1.y: pos2.y) - abs(pos1.y-pos2.y)/2,
            .w = abs(pos1.x-pos2.x),
            .h = abs(pos1.y-pos2.y)
        }
    );
}

void drawRectCoordLength(const Coord pos, const Length len)
{
    SDL_RenderDrawRect(gfx.renderer, &(const Rect){.x = pos.x, .y = pos.y, .w = len.x, .h = len.y});
}

void drawRectCenteredCoordLength(const Coord pos, const Length len)
{
    SDL_RenderDrawRect(
        gfx.renderer,
        &(const Rect){
            .x = (pos.x) - len.x/2,
            .y = (pos.y) - len.y/2,
            .w = len.x,
            .h = len.y
        }
    );
}

void fillRectCoordLength(const Coord pos, const Length len)
{
    SDL_RenderFillRect(gfx.renderer, &(const Rect){.x = pos.x, .y = pos.y, .w = len.x, .h = len.y});
}

void fillRectCenteredCoordLength(const Coord pos, const Length len)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){
            .x = (pos.x) - len.x/2,
            .y = (pos.y) - len.y/2,
            .w = len.x,
            .h = len.y
        }
    );
}

void drawRectRect(const Rect rect)
{
    SDL_RenderDrawRect(gfx.renderer, &rect);
}

void fillRectRect(const Rect rect)
{
    SDL_RenderFillRect(gfx.renderer, &rect);
}

void drawSquareCoord(const Coord pos, const uint len)
{
    SDL_RenderDrawLine(gfx.renderer, pos.x, pos.y, pos.x+len-1, pos.y);
    SDL_RenderDrawLine(gfx.renderer, pos.x, pos.y, pos.x, pos.y+len-1);
    SDL_RenderDrawLine(gfx.renderer, pos.x+len-1, pos.y, pos.x+len-1, pos.y+len-1);
    SDL_RenderDrawLine(gfx.renderer, pos.x, pos.y+len-1, pos.x+len-1, pos.y+len-1);
    // SDL_RenderDrawRect(
    //     gfx.renderer,
    //     &(const Rect){.x = pos.x, .y = pos.y, .w = len, .h = len}
    // );
}

void drawSquare(const uint x, const uint y, const uint len)
{
    SDL_RenderDrawLine(gfx.renderer, x, y, x+len-1, y);
    SDL_RenderDrawLine(gfx.renderer, x, y, x, y+len-1);
    SDL_RenderDrawLine(gfx.renderer, x+len-1, y, x+len-1, y+len-1);
    SDL_RenderDrawLine(gfx.renderer, x, y+len-1, x+len-1, y+len-1);
}

void fillSquare(const uint x, const uint y, const uint len)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){.x = x, .y = y, .w = len, .h = len}
    );
}

void fillSquareResize(const uint x, const uint y, const uint len, const int resize)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){
            .x = x-resize, .y = y-resize, .w = len+resize*2, .h = len+resize*2
        }
    );
}

void fillSquareCoord(const Coord pos, const uint len)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){.x = pos.x, .y = pos.y, .w = len, .h = len}
    );
}

void fillSquareCoordResize(const Coord pos, const uint len, const int resize)
{
    SDL_RenderFillRect(
        gfx.renderer,
        &(const Rect){
            .x = pos.x-resize, .y = pos.y-resize, .w = len+resize*2, .h = len+resize*2
        }
    );
}

void fillBorder(uint x, uint y, uint xlen, uint ylen, int b)
{
    fillRect(x-b, y-b, xlen+2*b, b);
    fillRect(x-b, y+ylen, xlen+2*b, b);
    fillRect(x-b, y, b, ylen);
    fillRect(x+xlen, y, b, ylen);
}

void fillBorderCoords(const Coord pos, const Length len, const int b)
{
    fillRect(pos.x-b,       pos.y-b,        len.x+2*b,  b);
    fillRect(pos.x-b,       pos.y+len.y,    len.x+2*b,  b);
    fillRect(pos.x-b,       pos.y,          b,          len.y);
    fillRect(pos.x+len.x,   pos.y,          b,          len.y);
}

void fillBorderCoordSquare(const Coord pos, const uint len, const int b)
{
    fillRect(pos.x-b,       pos.y-b,        len+2*b,    b);
    fillRect(pos.x-b,       pos.y+len,      len+2*b,    b);
    fillRect(pos.x-b,       pos.y,          b,          len);
    fillRect(pos.x+len,     pos.y,          b,          len);
}

void drawCircle(const uint x, const uint y, const uint radius)
{
    const double rsq = (double)(radius*radius);
    uint yoff = radius;
    for(uint xoff = 0; xoff <= yoff; xoff++){
        const double yc = sqrt(rsq - (xoff+1)*(xoff+1));
        const double ym = (double)yoff - 0.5;
        // 8 sections of circle
        drawPixel(x+xoff, y+yoff);        // 1
        drawPixel(x-yoff, y+xoff);        // 2
        drawPixel(x-xoff, y-yoff);        // 3
        drawPixel(x+yoff, y-xoff);        // 4

        drawPixel(x-xoff, y+yoff);        // 5
        drawPixel(x-yoff, y-xoff);        // 6
        drawPixel(x+xoff, y-yoff);        // 7
        drawPixel(x+yoff, y+xoff);        // 8
        yoff -= yc <= ym;
    }
}

void fillCircle(const uint x, const uint y, const uint radius)
{
    const double rsq = (double)(radius*radius);
    uint yoff = radius;
    for(uint xoff = 0; xoff <= yoff; xoff++){
        const double yc = sqrt(rsq - (xoff+1)*(xoff+1));
        const double ym = (double)yoff - 0.5;
        // connecting 8 sections of circle
        drawLine(x+xoff, y-yoff, x+xoff, y+yoff);        // 7 to 1
        drawLine(x-yoff, y+xoff, x+yoff, y+xoff);        // 2 to 8
        drawLine(x-xoff, y-yoff, x-xoff, y+yoff);        // 3 to 5
        drawLine(x-yoff, y-xoff, x+yoff, y-xoff);        // 6 to 4
        yoff -= yc <= ym;
    }
}

void drawCircleCoord(const Coord pos, const uint radius)
{
    const double rsq = (double)(radius*radius);
    uint yoff = radius;
    for(uint xoff = 0; xoff <= yoff; xoff++){
        const double yc = sqrt(rsq - (xoff+1)*(xoff+1));
        const double ym = (double)yoff - 0.5;
        // 8 sections of circle
        drawPixel(pos.x+xoff, pos.y+yoff);        // 1
        drawPixel(pos.x-yoff, pos.y+xoff);        // 2
        drawPixel(pos.x-xoff, pos.y-yoff);        // 3
        drawPixel(pos.x+yoff, pos.y-xoff);        // 4

        drawPixel(pos.x-xoff, pos.y+yoff);        // 5
        drawPixel(pos.x-yoff, pos.y-xoff);        // 6
        drawPixel(pos.x+xoff, pos.y-yoff);        // 7
        drawPixel(pos.x+yoff, pos.y+xoff);        // 8
        yoff -= yc <= ym;
    }
}

void fillCircleCoord(const Coord pos, const uint radius)
{
    const double rsq = (double)(radius*radius);
    uint yoff = radius;
    for(uint xoff = 0; xoff <= yoff; xoff++){
        const double yc = sqrt(rsq - (xoff+1)*(xoff+1));
        const double ym = (double)yoff - 0.5;
        // connecting 8 sections of circle
        drawLine(pos.x+xoff, pos.y-yoff, pos.x+xoff, pos.y+yoff);// 7 to 1
        drawLine(pos.x-yoff, pos.y+xoff, pos.x+yoff, pos.y+xoff);// 2 to 8
        drawLine(pos.x-xoff, pos.y-yoff, pos.x-xoff, pos.y+yoff);// 3 to 5
        drawLine(pos.x-yoff, pos.y-xoff, pos.x+yoff, pos.y-xoff);// 6 to 4
        yoff -= yc <= ym;
    }
}

Color getColor(void)
{
    Color c = {0};
    SDL_GetRenderDrawColor(gfx.renderer, &c.r, &c.g, &c.b, &c.a);
    return c;
}

void setColor(const Color c)
{
    SDL_SetRenderDrawColor(gfx.renderer, c.r, c.g, c.b, c.a);
}

u32 colorToU32(const Color c)
{
    return *((u32*)&c);
}

Color u32ToColor(const u32 u)
{
    return (const Color){.r = u&0xff, .g = (u>>8)&0xff, .b = (u>>16)&0xff, 0xff};
}

void drawTri(const Coord pos1, const Coord pos2, const Coord pos3)
{
    trigonColor(
        gfx.renderer,
        pos1.x, pos1.y,
        pos2.x, pos2.y,
        pos3.x, pos3.y,
        colorToU32(getColor())
    );
}

void fillTri(const Coord pos1, const Coord pos2, const Coord pos3)
{
    filledTrigonColor(
        gfx.renderer,
        pos1.x, pos1.y,
        pos2.x, pos2.y,
        pos3.x, pos3.y,
        colorToU32(getColor())
    );
}

void drawPoly(Coord *pos, const uint num)
{
    i16 posx[num];
    i16 posy[num];
    for(uint i = 0; i < num; i++){
    posx[i] = pos[i].x;
    posy[i] = pos[i].y;
    }
    polygonColor(gfx.renderer, posx, posy, num, colorToU32(getColor()));
}

void fillPoly(Coord *pos, const uint num)
{
    i16 posx[num];
    i16 posy[num];
    for(uint i = 0; i < num; i++){
        posx[i] = pos[i].x;
        posy[i] = pos[i].y;
    }
    filledPolygonColor(gfx.renderer, posx, posy, num, colorToU32(getColor()));
}

void bezier(const Coord *pos, const uint numc, const uint nums)
{
    i16 posx[numc];
    i16 posy[numc];
    for(uint i = 0; i < numc; i++){
        posx[i] = pos[i].x;
        posy[i] = pos[i].y;
    }
    bezierColor(gfx.renderer, posx, posy, numc, nums, colorToU32(getColor()));
}

void setRGB(const u8 r, const u8 g, const u8 b)
{
    SDL_SetRenderDrawColor(gfx.renderer, r, g, b, 255);
}

void setRGBA(const u8 r, const u8 g, const u8 b, const u8 a)
{
    SDL_SetRenderDrawColor(gfx.renderer, r, g, b, a);
}

void fillScreen(void)
{
    SDL_RenderClear(gfx.renderer);
}

void clear(void)
{
    const Color c = getColor();
    setColor(gfx.defaultColor);
    SDL_RenderClear(gfx.renderer);
    setColor(c);
}

void outlineWindow(void)
{
    const Color c = getColor();
    setColor(WHITE);
    fillBorderCoords(iC(0,0), getWindowLen(), -2);
    setColor(c);
}

void draw(void)
{

    if(gfx.outlined)
        outlineWindow();
    SDL_RenderPresent(gfx.renderer);
}

uint frameStart(void)
{
    clear();
    return getTicks();
}

void frameEnd(const uint t)
{
    draw();
    events(t+TPF);
}

void gfx_quit(void)
{
    SDL_DestroyRenderer(gfx.renderer);
    SDL_DestroyWindow(gfx.window);
    SDL_Quit();
}

void gfx_init(void)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("SDL borked! Error: %s\n", SDL_GetError());
        exit(0);
    }

    gfx.window = SDL_CreateWindow(
        "main.out",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DEFAULT_WINDOW_XLEN,
        DEFAULT_WINDOW_YLEN,
        SDL_WINDOW_RESIZABLE
    );

    gfx.renderer = SDL_CreateRenderer(gfx.window, -1, SDL_RENDERER_ACCELERATED);
    printf("Adding gfx_quit to atexit()\n");
    atexit(gfx_quit);

    gfx.defaultColor = BLACK;
    gfx.outlined = true;
    SDL_SetRenderDrawBlendMode(gfx.renderer, BLEND_NONE);
    gfx.restoreLen.x = DEFAULT_WINDOW_XLEN;
    gfx.restoreLen.y = DEFAULT_WINDOW_YLEN;
    setWindowResizable(true);
    gfx.prvLen.x = DEFAULT_WINDOW_XLEN;
    gfx.prvLen.y = DEFAULT_WINDOW_YLEN;

    clear();
    draw();
    clear();
}

#endif /* end of include guard: GRAPHICS_H */
