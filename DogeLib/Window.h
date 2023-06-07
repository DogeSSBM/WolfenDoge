#ifndef WINDOW_H
#define WINDOW_H

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

#endif /* end of include guard: WINDOW_H */
