#ifndef DOGELIB_INPUT_H
#define DOGELIB_INPUT_H

struct{
    bool textInputState;
    bool textInputChange;
    u8 key[SDL_NUM_SCANCODES];
    u8 prev[SDL_NUM_SCANCODES];
    char *textInput;
    st textInputSize;
    st textInputPos;
    boolFunc textInputDone;
}keys = {0};

bool textInputState(void)
{
    return keys.textInputState;
}

bool textInputEnded(void)
{
    return !keys.textInputState && keys.textInputChange;
}

void textInputAppendText(char *text)
{
    const st maxlen = keys.textInputSize - keys.textInputPos;
    const st textlen = strlen(text);
    const st appendlen = imin(maxlen, textlen);
    if(appendlen < textlen)
        printf("Discarding text input: \"%s\"\n", text+appendlen);
    if(appendlen <= 0)
        return;
    memcpy(keys.textInput+keys.textInputPos, text, appendlen);
    keys.textInputPos += appendlen;
}

void textInputStop(void)
{
    if(!keys.textInputState)
        return;
    keys.textInputState = false;
    keys.textInputChange = true;
    keys.textInput = NULL;
    keys.textInputSize = 0;
    keys.textInputPos = 0;
}

bool defaultTextInputDone(void)
{
    return keyState(SC_ESCAPE);
}

void textInputStart(char *buf, const st bufsize, boolFunc textInputDone)
{
    if(!keys.textInputState)
        keys.textInputChange = true;
    keys.textInputState = true;
    keys.textInput = buf;
    keys.textInputSize = bufsize;
    keys.textInputPos = strlen(buf);
    keys.textInputDone = textInputDone == NULL ? defaultTextInputDone : textInputDone;
}

bool keyState(const Scancode key)
{
    return keys.key[key];
}

bool keyPressed(const Scancode key)
{
    return !keys.textInputState && (keys.key[key] && !keys.prev[key]);
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

bool isTextKey(const char *keyName)
{
    if(strlen(keyName) != 1)
        return false;
    return keyName[0] >= ' ' && keyName[0] <= '~';
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

Coord mousePos(void)
{
    return mouse.pos;
}

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

void setRelativeMouse(const bool state)
{
    SDL_SetRelativeMouseMode(state);
}

bool keyCtrlState(void)
{
    return keyState(SC_LCTRL) || keyState(SC_RCTRL);
}

bool keyShiftState(void)
{
    return keyState(SC_LSHIFT) || keyState(SC_RSHIFT);
}

bool keyComboPressed(const Scancode modkey, const Scancode key)
{
    return keyPressed(key) && keyState(modkey);
}

bool checkCtrlKey(const Scancode key)
{
    return keyPressed(key) && keyCtrlState();
}

void input_init(void)
{
    mouse.pos = coordDivi(getWindowLen(), 2);
    mouse.prev.pos = coordDivi(getWindowLen(), 2);
}

Offset wasdKeyStateOffset(void)
{
    return (const Offset){
        .x = keyState(SC_D) - keyState(SC_A),
        .y = keyState(SC_S) - keyState(SC_W)
    };
}

Offset arrowKeyStateOffset(void)
{
    return (const Offset){
        .x = keyState(SC_RIGHT) - keyState(SC_LEFT),
        .y = keyState(SC_DOWN) - keyState(SC_UP)
    };
}

Offset dirKeyStateOffset(void)
{
    return coordLeast(coordAdd(wasdKeyStateOffset(), arrowKeyStateOffset()), iC(1,1));
}

Offset wasdKeyPressedOffset(void)
{
    return (const Offset){
        .x = keyPressed(SC_D) - keyPressed(SC_A),
        .y = keyPressed(SC_S) - keyPressed(SC_W)
    };
}

Offset arrowKeyPressedOffset(void)
{
    return (const Offset){
        .x = keyPressed(SC_RIGHT) - keyPressed(SC_LEFT),
        .y = keyPressed(SC_DOWN) - keyPressed(SC_UP)
    };
}

Offset dirKeyPressedOffset(void)
{
    return coordLeast(coordAdd(wasdKeyPressedOffset(), arrowKeyPressedOffset()), iC(1,1));
}

#endif /* end of include guard: DOGELIB_INPUT_H */
