#ifndef EVENTS_H
#define EVENTS_H

void events(const uint endOfFrame)
{
    i32 ticksLeft = imax(1, endOfFrame - getTicks());
    mouse.prev.wheel = mouse.wheel;
    mouse.wheel = (const Coord){.x = 0, .y = 0};
    bool e = false;
    gfx.winFlags = SDL_GetWindowFlags(gfx.window);
    gfx.prvLen = gfx.winLen;
    do{
        Event event;
        if(ticksLeft > 0)
            e = SDL_WaitEventTimeout(&event, ticksLeft);
        else
            e = SDL_PollEvent(&event);
        if(e){
            switch(event.type)
            {
            case SDL_QUIT:
                printf("Quitting now!\n");
                exit(0);
                break;
            case SDL_MOUSEWHEEL:
                mouse.wheel.x += event.wheel.x;
                mouse.wheel.y += event.wheel.y;
                break;
            // case SDL_WINDOWEVENT_SIZE_CHANGED:
            // case SDL_WINDOWEVENT:
                // break;
            default:
                break;
            }
        }
        ticksLeft = imax(0, endOfFrame - getTicks());
    }while(ticksLeft > 0 || e);
    gfx.winLen = getWindowLen();
    memcpy(keys.prev, keys.key, SDL_NUM_SCANCODES);
    memcpy(keys.key, SDL_GetKeyboardState(NULL), SDL_NUM_SCANCODES);
    mouse.prev.state = mouse.state;
    mouse.prev.pos = mouse.pos;
    mouse.prev.vec = mouse.vec;
    mouse.state = SDL_GetMouseState(&mouse.pos.x, &mouse.pos.y);
    SDL_GetRelativeMouseState(&mouse.vec.x, &mouse.vec.y);
}

#endif /* end of include guard: EVENTS_H */
