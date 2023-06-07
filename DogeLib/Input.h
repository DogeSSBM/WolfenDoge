#ifndef INPUT_H
#define INPUT_H

struct{
    u8 key[SDL_NUM_SCANCODES];
    u8 prev[SDL_NUM_SCANCODES];
}keys = {0};

bool keyState(const Scancode key)
{
    return keys.key[key];
}

bool keyPressed(const Scancode key)
{
    return keys.key[key] && !keys.prev[key];
}

bool keyHeld(const Scancode key)
{
    return keys.key[key] && keys.prev[key];
}

bool keyChanged(const Scancode key)
{
    return keys.key[key] != keys.prev[key];
}

bool keyReleased(const Scancode key)
{
    return !keys.key[key] && keys.prev[key];
}

struct{
    Offset vec;
    Coord pos;
    u32 state;
    Coord wheel;
    struct{
        Offset vec;
        Coord pos;
        u32 state;
        Coord wheel;
    }prev;
}mouse = {0};

bool mouseBtnPressed(const u32 mouseBtn)
{
    return mouse.state&mouseBtn && !(mouse.prev.state&mouseBtn);
}

bool mouseBtnState(const u32 mouseBtn)
{
    return mouse.state&mouseBtn;
}

bool mouseBtnHeld(const u32 mouseBtn)
{
    return mouse.state&mouseBtn && mouse.prev.state&mouseBtn;
}

bool mouseBtnReleased(const u32 mouseBtn)
{
    return !(mouse.state&mouseBtn) && mouse.prev.state&mouseBtn;
}

bool mouseBtnChanged(const u32 mouseBtn)
{
    return (mouse.state&mouseBtn) != (mouse.prev.state&mouseBtn);
}

int mouseScrolledX(void)
{
    return mouse.wheel.x;
}

int mouseScrolledY(void)
{
    return mouse.wheel.y;
}

bool mouseMoving(void)
{
    return !coordSame(mouse.vec, (const Coord){0}) && !coordSame(mouse.prev.vec, (const Coord){0});
}

bool mouseMoveStart(void)
{
    return !coordSame(mouse.vec, (const Coord){0}) && coordSame(mouse.prev.vec, (const Coord){0});
}

bool mouseMoveStop(void)
{
    return !mouseMoving() && !coordSame(mouse.prev.vec, (const Coord){0});
}

Length mouseMovement(void)
{
    return coordSub(mouse.pos, mouse.prev.pos);
}

void input_init(void)
{
    mouse.pos = coordDivi(getWindowLen(), 2);
    mouse.prev.pos = coordDivi(getWindowLen(), 2);
}

#endif /* end of include guard: INPUT_H */
