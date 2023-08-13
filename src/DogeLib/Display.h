#ifndef DOGELIB_DISPLAY_H
#define DOGELIB_DISPLAY_H

int getWinDisplayIndex(void)
{
    return SDL_GetWindowDisplayIndex(gfx.window);
}

Rect getWinDisplayRect(void)
{
    Rect rect = {0};
    SDL_GetDisplayBounds(getWinDisplayIndex(), &rect);
    return rect;
}

Length getWinDisplayLen(void)
{
    Rect rect = {0};
    assert(SDL_GetDisplayBounds(getWinDisplayIndex(), &rect) >= 0);
    return irL(rect);
}

Length getDisplayIndexLen(const uint i)
{
    Rect rect = {0};
    assert(SDL_GetDisplayBounds(i, &rect) >= 0);
    return irL(rect);
}

Rect getDisplayIndexRect(const uint i)
{
    Rect rect = {0};
    assert(SDL_GetDisplayBounds(i, &rect) >= 0);
    return rect;
}

Coord getWinDisplayOffset(void)
{
    Rect rect = {0};
    SDL_GetDisplayBounds(getWinDisplayIndex(), &rect);
    return irC(rect);
}

Coord getDisplayIndexOffset(const uint i)
{
    Rect rect = {0};
    SDL_GetDisplayBounds(i, &rect);
    return irC(rect);
}

uint getDisplayNum(void)
{
    const int ret = SDL_GetNumVideoDisplays();
    assert(ret > 0);
    return ret;
}

Length getDisplayTotalLen(void)
{
    const uint numDisplay = getDisplayNum();
    Length ret = {0};
    for(uint i = 0; i < numDisplay; i++){
        ret = coordMost(ret, unrectify(getDisplayIndexRect(i)));
    }
    return ret;
}

#endif /* end of include guard: DOGELIB_DISPLAY_H */
