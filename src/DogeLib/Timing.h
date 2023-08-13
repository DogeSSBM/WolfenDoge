#ifndef DOGELIB_TIMING_H
#define DOGELIB_TIMING_H

#define TPS     1000
#define FPS     60
#define TPF     (TPS/FPS)

static inline
uint getTicks(void)
{
    return SDL_GetTicks();
}

#endif /* end of include guard: DOGELIB_TIMING_H */
