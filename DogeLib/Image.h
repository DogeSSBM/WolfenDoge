#ifndef DOGELIB_IMAGE_H
#define DOGELIB_IMAGE_H

void freeImg(Img *image)
{
    if(image)
        SDL_FreeSurface(image);
}

void freeTexture(Texture *t)
{
    if(t)
        SDL_DestroyTexture(t);
}

Texture* imgTexture(Img *img)
{
    Texture *t = SDL_CreateTextureFromSurface(gfx.renderer, img);
    freeImg(img);
    return t;
}

Length textureLen(Texture *texture)
{
    Length len = {0};
    SDL_QueryTexture(texture, NULL, NULL, &len.x, &len.y);
    return len;
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

Rect textureRect(Texture *texture, const Coord pos)
{
    return rectify(pos, textureLen(texture));
}

#endif /* end of include guard: DOGELIB_IMAGE_H */
