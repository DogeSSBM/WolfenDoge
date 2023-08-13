#ifndef DOGELIB_IMAGE_H
#define DOGELIB_IMAGE_H

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

void freeImg(Img *image)
{
    if(image)
        SDL_FreeSurface(image);
}

Texture* textureFree(Texture *t)
{
    if(t)
        SDL_DestroyTexture(t);
    return NULL;
}

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

Texture* imgTexture(Img *img)
{
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, img);
    freeImg(img);
    return t;
}

Texture* loadTexture(const char *imgFile)
{
    Img *img = IMG_Load(imgFile);
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, img);
    SDL_FreeSurface(img);
    return t;
}

void drawImg(Img *image)
{
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, image);
    SDL_RenderCopy(gfx.renderer, t, NULL, NULL);
    SDL_DestroyTexture(t);
}

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

Length textureLen(Texture *texture)
{
    Length len = {0};
    SDL_QueryTexture(texture, NULL, NULL, &len.x, &len.y);
    return len;
}

Rect textureRect(Texture *texture, const Coord pos)
{
    return rectify(pos, textureLen(texture));
}

void drawTextureRect(Texture *texture, const Rect rect, const int x, const int y)
{
    const Length len = textureLen(texture);
    SDL_RenderCopy(
        gfx.renderer,
        texture,
        &rect,
        &(const Rect){.x = x, .y = y, .w = len.x, .h = len.y}
    );
}

void drawTextureRectCoord(Texture *texture, const Rect rect, const Coord pos)
{
    drawTextureRect(texture, rect, pos.x, pos.y);
}

void drawTextureRectResize(Texture *texture, const Rect rect, const int x, const int y, const int xlen, const int ylen)
{
    SDL_RenderCopy(
        gfx.renderer,
        texture,
        &rect,
        &(const Rect){.x = x, .y = y, .w = xlen, .h = ylen}
    );
}

void drawTextureRectCoordResize(Texture *texture, const Rect rect, const Coord pos, const Length len)
{
    drawTextureRectResize(texture, rect, pos.x, pos.y, len.x, len.y);
}

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

Length drawTextureCoord(Texture *texture, const Coord pos)
{
    return drawTexture(texture, pos.x, pos.y);
}

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

Length drawTextureCenteredCoord(Texture *texture, const Coord pos)
{
    return drawTextureCentered(texture, pos.x, pos.y);
}

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

Length drawTextureCoordResize(Texture *texture, const Coord pos, const Length len)
{
    return drawTextureResize(texture, pos.x, pos.y, len.x, len.y);
}

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

Length drawTextureCenteredCoordResize(Texture *texture, const Coord pos, const Length len)
{
    return drawTextureCenteredResize(texture, pos.x, pos.y, len.x, len.y);
}

#endif /* end of include guard: DOGELIB_IMAGE_H */
