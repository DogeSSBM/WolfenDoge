#ifndef DOGELIB_WINDOW_H
#define DOGELIB_WINDOW_H

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

void winSetPos(const int x, const int y)
{
    SDL_SetWindowPosition(gfx.window, x, y);
}

void winSetPosCoord(const Coord pos)
{
    SDL_SetWindowPosition(gfx.window, pos.x, pos.y);
}

bool winIsMaximized(void)
{
    return SDL_WINDOW_MINIMIZED & gfx.winFlags;
}

bool winIsMinimized(void)
{
    return SDL_WINDOW_MINIMIZED & gfx.winFlags;
}

void winSetGrab(const bool isGrabbed)
{
    SDL_SetWindowGrab(gfx.window, isGrabbed);
}

bool winIsGrabbed(void)
{
    return SDL_GetWindowGrab(gfx.window);
}

Window* winGetFocused(void)
{
    return SDL_GetMouseFocus();
}

bool winIsFocused(void)
{
    return winGetFocused() == gfx.window;
}

#endif /* end of include guard: DOGELIB_WINDOW_H */
