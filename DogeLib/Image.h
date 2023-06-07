#ifndef IMAGE_H
#define IMAGE_H

void img_quit(void)
{
    IMG_Quit();
}

void img_init(void)
{
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if((initted & flags) != flags) {
        printf("SDL_image borked! Error: %s\n", IMG_GetError());
        exit(0);
    }
    printf("Adding img_quit to atexit()\n");
    atexit(img_quit);
}

static inline
void freeImg(Img *image)
{
    if(image)
        SDL_FreeSurface(image);
}

static inline
void freeTexture(Texture *t)
{
    if(t)
        SDL_DestroyTexture(t);
}

static inline
Img* loadImg(const char *imgFile)
{
    Img *surface = IMG_Load(imgFile);
    if(surface == NULL){
        printf("Failed to load file %s! Error:%s\n",
            imgFile, IMG_GetError());
        exit(0);
    }
    return surface;
}

static inline
Texture* imgTexture(Img *img)
{
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, img);
    freeImg(img);
    return t;
}

static inline
Texture* loadTexture(const char *imgFile)
{
    Img *img = IMG_Load(imgFile);
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, img);
    SDL_FreeSurface(img);
    return t;
}

static inline
void drawImg(Img *image)
{
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, image);
    SDL_RenderCopy(gfx.renderer, t, NULL, NULL);
    SDL_DestroyTexture(t);
}

static inline
void loadDrawImg(const char *imgFile)
{
    Img *img = IMG_Load(imgFile);
    if(img == NULL){
        printf("Failed to load file %s! Error:%s\n",
            imgFile, IMG_GetError());
        exit(0);
    }
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, img);
    SDL_RenderCopy(gfx.renderer, t, NULL, NULL);
    SDL_DestroyTexture(t);
    SDL_FreeSurface(img);
}

static inline
Length textureLen(Texture *texture)
{
    Length len = {0};
    SDL_QueryTexture(texture, NULL, NULL, &len.x, &len.y);
    return len;
}

static inline
Rect textureRect(Texture *texture, const Coord pos)
{
    return rectify(pos, textureLen(texture));
}

static inline
Length drawTexture(Texture *texture, const int x, const int y)
{
    const Length len = textureLen(texture);
    SDL_RenderCopy(
        gfx.renderer,
        texture,
        &(const Rect){.w = len.x, .h = len.y},
        &(const Rect){.x = x, .y = y, .w = len.x, .h = len.y}
    );
    return len;
}

static inline
Length drawTextureCoord(Texture *texture, const Coord pos)
{
    return drawTexture(texture, pos.x, pos.y);
}

static inline
Length drawTextureCentered(Texture *texture, const int x, const int y)
{
    const Length len = textureLen(texture);
    SDL_RenderCopy(
        gfx.renderer,
        texture,
        &(const Rect){.w = len.x, .h = len.y},
        &(const Rect){.x = x - len.x/2, .y = y - len.y/2, .w = len.x, .h = len.y}
    );
    return len;
}

static inline
Length drawTextureCenteredCoord(Texture *texture, const Coord pos)
{
    return drawTextureCentered(texture, pos.x, pos.y);
}

static inline
Length drawTextureResize(Texture *texture, const int x, const int y, const int xlen, const int ylen)
{
    const Length len = textureLen(texture);
    SDL_RenderCopy(
        gfx.renderer,
        texture,
        &(const Rect){.w = len.x, .h = len.y},
        &(const Rect){.x = x, .y = y, .w = xlen, .h = ylen}
    );
    return len;
}

static inline
Length drawTextureCoordResize(Texture *texture, const Coord pos, const Length len)
{
    return drawTextureResize(texture, pos.x, pos.y, len.x, len.y);
}

static inline
Length drawTextureCenteredResize(Texture *texture, const int x, const int y, const int xlen, const int ylen)
{
    const Length len = textureLen(texture);
    SDL_RenderCopy(
        gfx.renderer,
        texture,
        &(const Rect){.w = len.x, .h = len.y},
        &(const Rect){.x = x - xlen/2, .y = y - ylen/2, .w = xlen, .h = ylen}
    );
    return len;
}

static inline
Length drawTextureCenteredCoordResize(Texture *texture, const Coord pos, const Length len)
{
    return drawTextureCenteredResize(texture, pos.x, pos.y, len.x, len.y);
}

#endif /* end of include guard: IMAGE_H */
