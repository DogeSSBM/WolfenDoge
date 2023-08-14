#ifndef DOGELIB_TEXT_H
#define DOGELIB_TEXT_H

char* strdup(char *str)
{
    char *dup = calloc(strlen(str)+1, sizeof(char));
    memcpy(dup, str, strlen(str));
    return dup;
}

bool strEndsWith(const char *str, const char *end)
{
    if(str == NULL || end == NULL)
        return false;
    const uint lenEnd = strlen(end);
    const uint lenStr = strlen(str);
    const char *pos = strstr(str, end);
    if(lenEnd > lenStr || strlen(pos) != strlen(end))
        return false;
    return true;
}

Img* textImg(const char *text)
{
    return TTF_RenderText_Solid(gfx.font, text, gfx.fontColor);
}

Texture* textTexture(const char *text)
{
    return imgTexture(textImg(text));
}

Length drawText(const char *text, const int x, const int y)
{
    Texture *t = textTexture(text);
    const Rect r = textureRect(t, iC(x,y));
    SDL_RenderCopy(gfx.renderer, t, NULL, &r);
    textureFree(t);
    return iC(r.w, r.h);
}

Length drawTextCoord(const char *text, const Coord pos)
{
    return drawText(text, pos.x, pos.y);
}

Length drawTextCentered(const char *text, const int x, const int y)
{
    Texture *t = textTexture(text);
    const Length len = drawTextureCentered(t, x, y);
    textureFree(t);
    return len;
}

Length drawTextCenteredCoord(const char *text, const Coord pos)
{
    return drawTextCentered(text, pos.x, pos.y);
}

Length getTextLength(const char *text)
{
    Length len = {0};
    TTF_SizeText(gfx.font, text, &len.x, &len.y);
    return len;
}

int getTextXLen(const char *text)
{
    Length len = {0};
    TTF_SizeText(gfx.font, text, &len.x, &len.y);
    return len.x;
}

int getTextYLen(const char *text)
{
    Length len = {0};
    TTF_SizeText(gfx.font, text, &len.x, &len.y);
    return len.y;
}

Coord* spanTextListCoords(Coord *coords, const Coord start, const Coord stop, const uint num)
{
    if(num == 0 || coords == NULL)
        return coords;
    const Length step = coordDivi(coordSub(stop, start), num>1?num-1:1);
    for(uint i = 0; i < num; i++)
        coords[i] = coordAdd(start, coordMuli(step, i));
    return coords;
}

Coord* spanTextListCoordsCentered(Coord *coords, const Coord start, const Coord stop, const uint num)
{
    if(num == 0 || coords == NULL)
        return coords;
    const Length step = coordDivi(coordSub(stop, start), num+1);
    for(uint i = 1; i < num+1; i++)
        coords[i-1] = coordAdd(start, coordMuli(step, i));
    return coords;
}

Rect* getTextListRect(const char **textList, Rect *const rect, const Coord start, const Coord stop, const uint num)
{
    if(num == 0 || rect == NULL)
        return rect;

    const Length step = coordDivi(coordSub(stop, start), num>1?num-1:1);
    for(uint i = 0; i < num; i++){
        const Length len = getTextLength(textList[i]);
        const Coord pos = coordSub(
            coordAdd(start, coordMuli(step, i)),
            coordDivi(len,2)
        );
        rect[i].x = pos.x;
        rect[i].y = pos.y;
        rect[i].w = len.x;
        rect[i].h = len.y;
    }
    return rect;
}

Rect* getTextListRectCentered(const char **textList, Rect *const rect, const Coord start, const Coord stop, const uint num)
{
    if(num == 0 || rect == NULL)
        return rect;

    const Length step = coordDivi(coordSub(stop, start), num+1);
    for(uint i = 1; i < num+1; i++){
        const Length len = getTextLength(textList[i-1]);
        const Coord pos = coordSub(
            coordAdd(start, coordMuli(step, i)),
            coordDivi(len,2)
        );
        rect[i-1].x = pos.x;
        rect[i-1].y = pos.y;
        rect[i-1].w = len.x;
        rect[i-1].h = len.y;
    }
    return rect;
}

int coordInRectList(const Coord coord, Rect *const rect, const int num)
{
    if(num < 1)
        return -1;
    for(int i = 0; i < num; i++)
        if(coordInRect(coord, rect[i]))
            return i;
    return -1;
}

int coordInTextList(const Coord coord, const Coord start, const Coord stop, const uint num, const char **textList)
{
    Rect r[num];
    return coordInRectArr(coord, getTextListRect(textList, r, start, stop, num), num);
}

// draws a collection of strings evenly spaced between 2 points
void spanTextList(const char **textList, const Coord start, const Coord stop, const uint num)
{
    if(num == 0)
        return;
    const Length step = coordDivi(coordSub(stop, start), num>1?num-1:1);
    for(uint i = 0; i < num; i++){
        drawTextCenteredCoord(textList[i], coordAdd(start, coordMuli(step, i)));
    }
}

void spanTextListCentered(const char **textList, const Coord start, const Coord stop, const uint num)
{
    if(num == 0)
        return;
    const Length step = coordDivi(coordSub(stop, start), num+1);
    for(uint i = 1; i < num+1; i++){
        drawTextCenteredCoord(textList[i-1], coordAdd(start, coordMuli(step, i)));
    }
}

void setTextSize(const uint size)
{
    if(size == gfx.fontSize)
        return;
    if(gfx.font != NULL)
        TTF_CloseFont(gfx.font);
    gfx.fontSize = size;
    gfx.font = TTF_OpenFont("./src/DogeLib/FiraCode.ttf", gfx.fontSize);
}

int getTextSize(void)
{
    return gfx.fontSize;
}

void setTextColor(const Color c)
{
    gfx.fontColor = c;
}

Color getTextColor(void)
{
    return gfx.fontColor;
}

void text_quit(void)
{
    if(gfx.font != NULL){
        TTF_CloseFont(gfx.font);
    }
    TTF_Quit();
}

void text_init(void)
{
    if(TTF_Init()){
        printf("SDL_ttf borked! Error: %s\n", TTF_GetError());
        exit(0);

    }
    gfx.fontColor = WHITE;
    setTextSize(32);
    if(!gfx.font){
        printf("Unable to open font or set font size! Error: %s\n", TTF_GetError());
        exit(0);
    }
    printf("Adding text_quit to atexit()\n");
    atexit(text_quit);
}

#endif /* end of include guard: DOGELIB_TEXT_H */
