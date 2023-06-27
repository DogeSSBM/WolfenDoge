#ifndef MAP_H
#define MAP_H

WallPacked* mapPack(Wall *map)
{
    const uint len = wallListLen(map);
    if(len == 0)
        return NULL;
    WallPacked *mapPacked = calloc(len, sizeof(WallPacked));
    for(uint i = 0; i < len; i++){
        mapPacked[i].c = map->c;
        mapPacked[i].a = map->a;
        mapPacked[i].b = map->b;
        map = map->next;
    }
    return mapPacked;
}

Wall* mapUnpack(WallPacked *mapPacked, const uint len)
{
    if(len == 0)
        return NULL;
    Wall *map = NULL;
    for(uint i = 0; i < len; i++)
        map = wallAppend(map, wallNew(mapPacked[i].c, mapPacked[i].a, mapPacked[i].b));
    return map;
}

Coordf mapBounds(Wall *map)
{
    Coordf bound = {0};
    while(map)
    {
        bound.x = fmost(bound.x, map->a.x);
        bound.x = fmost(bound.x, map->b.x);
        bound.y = fmost(bound.y, map->a.y);
        bound.y = fmost(bound.y, map->b.y);
        map = map->next;
    }
    return bound;
}

Coord resizeTransform(const Length oldLen, const Length newLen, const Coord pos)
{
    return CfC(cfMul(CCf(newLen), cfDiv(CCf(pos), CCf(oldLen))));
}

Coordf resizeTransformf(const Lengthf oldLen, const Lengthf newLen, const Coordf pos)
{
    return cfMul(newLen, cfDiv(pos, oldLen));
}

Coordf screenToMap(const Coord off, const float scale, const Coord pos)
{
    return cfMulf(CCf(coordSub(pos, off)), scale);
}

Coord mapToScreen(const Coord off, const float scale, const Coordf pos)
{
    return coordAdd(CfC(cfDivf(pos, scale)), off);
}

Coordf absOff(const Coord len, const Coord pos)
{
    return cfDiv(CCf(pos), CCf(len));
}

Wall* posNearest(Wall *map, const Coordf pos, Coordf **nearest)
{
    if(!map){
        nearest = NULL;
        return NULL;
    }
    Wall *wall = map;
    *nearest = &(map->a);
    float dst = cfDist(pos, map->a);
    while(map){
        float curDst = cfDist(pos, map->a);
        if(curDst < dst){
            dst = curDst;
            wall = map;
            *nearest = &(map->a);
        }
        curDst = cfDist(pos, map->b);
        if(curDst < dst){
            dst = curDst;
            wall = map;
            *nearest = &(map->b);
        }
        map = map->next;
    }
    return wall;
}

void defaultMapFileName(char *fileName)
{
    uint i = 0;
    File *file = NULL;
    sprintf(fileName, "map.bork");
    while((file = fopen(fileName, "rb")) != NULL){
        fclose(file);
        i++;
        sprintf(fileName, "map(%u).bork", i);
    }
}

void mapSave(Wall *map, char *fileName)
{
    if(!map){
        printf("Map is empty, skipping save\n");
        return;
    }
    File *file = NULL;
    char defaultName[64] = {0};
    if(!fileName){
        defaultMapFileName(defaultName);
        fileName = defaultName;
    }
    file = fopen(fileName, "wb");
    const uint len = wallListLen(map);
    WallPacked *mapPacked = mapPack(map);
    fwrite(&len, sizeof(uint), 1, file);
    fwrite(mapPacked, sizeof(WallPacked), len, file);
    fclose(file);
    free(mapPacked);
}

Wall* wallNew(const Color c, const Coordf a, const Coordf b)
{
    Wall *w = calloc(1, sizeof(Wall));
    w->c = c;
    w->a = a;
    w->b = b;
    return w;
}

Wall* wallAppend(Wall *head, Wall *tail)
{
    if(!head)
        return tail;
    Wall *cur = head;
    while(cur->next)
        cur = cur->next;
    cur->next = tail;
    return head;
}

Wall* wallDelete(Wall *map, Wall *del)
{
    if(!del)
        return map;
    if(!map)
        return NULL;
    if(del == map){
        Wall *next = map->next;
        free(map);
        return next;
    }
    Wall *cur = map;
    while(cur && cur->next != del)
        cur = cur->next;
    if(!cur)
        return map;
    Wall *next = cur->next->next;
    free(cur->next);
    cur->next = next;
    return map;
}

uint wallListLen(Wall *map)
{
    uint len = 0;
    while(map){
        len++;
        map = map->next;
    }
    return len;
}

void wallFreeList(Wall *list)
{
    while(list){
        Wall *next = list->next;
        free(list);
        list = next;
    }
}

Wall* mapDefault(void)
{
    Wall *map =           wallNew(GREEN,    (const Coordf){.x=  0.0f, .y=  0.0f},   (const Coordf){.x=750.0f, .y=  0.0f});
    map = wallAppend(map, wallNew(MAGENTA,  (const Coordf){.x=750.0f, .y=  0.0f},   (const Coordf){.x=750.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(MAGENTA,  (const Coordf){.x=  0.0f, .y=  0.0f},   (const Coordf){.x=  0.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(GREEN,    (const Coordf){.x=  0.0f, .y=750.0f},   (const Coordf){.x=750.0f, .y=750.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=250.0f},   (const Coordf){.x=500.0f, .y=250.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=500.0f, .y=250.0f},   (const Coordf){.x=500.0f, .y=500.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=250.0f},   (const Coordf){.x=250.0f, .y=500.0f}));
    map = wallAppend(map, wallNew(BLUE,     (const Coordf){.x=250.0f, .y=500.0f},   (const Coordf){.x=500.0f, .y=500.0f}));
    return map;
}

Wall* mapEdit(Wall *map, char *fileName)
{
    float scale = 1.0f;
    bool snap = true;
    uint snaplen = 24;
    Coordf mmpos = {0};
    Coord off = {0};
    SDL_SetRelativeMouseMode(false);
    Length wlen = getWindowLen();
    bool rdrag = false;
    Coord mrd = {0};
    Coord mld = {0};
    bool ldrag = false;

    Wall *selectedWall = NULL;
    Coordf *selectedPos = NULL;

    Color c = MAGENTA;
    int ci = 0;
    while(1){
        const uint t = frameStart();
        if(checkCtrlKey(SDL_SCANCODE_Q) || checkCtrlKey(SDL_SCANCODE_W)){
            return map;
        }

        if(checkCtrlKey(SDL_SCANCODE_S)){
            mapSave(map, fileName);
            return map;
        }else if(keyPressed(SDL_SCANCODE_S)){
            snap = !snap;
        }

        ci = wrap(ci + keyPressed(SDL_SCANCODE_RIGHT) - keyPressed(SDL_SCANCODE_LEFT), 0, 3);
        u8* b = colorIndex(&c, ci);
        int change = (keyPressed(SDL_SCANCODE_UP) - keyPressed(SDL_SCANCODE_DOWN));
        change *= (1+(keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL)))*10;
        *b = clamp((int)(*b) + change, 0, 255);

        mmpos = screenToMap(off, scale, mouse.pos);

        if(keyPressed(SDL_SCANCODE_ESCAPE)){
            selectedWall = NULL;
            selectedPos = NULL;
            rdrag = false;
            ldrag = false;
        }

        if(windowResized()){
            const Length wlenOld = wlen;
            wlen = getWindowLen();
            off = resizeTransform(wlenOld, wlen, off);
        }

        if((keyPressed(SDL_SCANCODE_DELETE) || keyPressed(SDL_SCANCODE_BACKSPACE)) && selectedWall){
            map = wallDelete(map, selectedWall);
            selectedWall = NULL;
            selectedPos = NULL;
        }

        if(mouseScrolledY()){
            if(keyState(SDL_SCANCODE_LCTRL) || keyState(SDL_SCANCODE_RCTRL)){
                snaplen = imax(1, snaplen + mouseScrolledY());
            }else{
                const float oldScale = scale;
                scale = fclamp(scale * (mouseScrolledY() > 0 ? 1.2f : .8f) , .01f, 100.0f);
                if(oldScale != scale){
                    const Coord ompos = mapToScreen(off, scale, mmpos);
                    const Coord diff = coordSub(mouse.pos, ompos);
                    off = coordAdd(off, diff);
                }
            }
        }

        if(mouseBtnState(MOUSE_M) || keyState(SDL_SCANCODE_LSHIFT)){
            off = coordAdd(off, mouseMovement());
            mrd = coordAdd(mrd, mouseMovement());
            mld = coordAdd(mld, mouseMovement());
        }

        if(!selectedWall && mouseBtnReleased(MOUSE_L)){
            selectedWall = posNearest(map, mmpos, &selectedPos);
        }

        if(mouseBtnPressed(MOUSE_L) && selectedPos){
            ldrag = true;
            mld = mouse.pos;
        }

        if(ldrag && mouseBtnReleased(MOUSE_L)){
            ldrag = false;
        }

        if(ldrag && selectedPos){
            // *selectedPos = screenToMap(off, scale, mld);
            *selectedPos = cfAdd(*selectedPos, cfMulf(CCf(mouseMovement()), scale));

            // *selectedPos = CCf(coordMuli(coordDivi(CfC(*selectedPos), (int)scale), (int)scale));
        }

        if(mouseBtnPressed(MOUSE_R)){
            mrd = mouse.pos;
            rdrag = true;
        }

        if(rdrag && mouseBtnReleased(MOUSE_R)){
            rdrag = false;
            map = wallAppend(map, wallNew(c, screenToMap(off, scale, mrd), screenToMap(off, scale, mouse.pos)));
        }

        if(rdrag){
            setColor(c);
            drawLineCoords(mrd, mouse.pos);
            setColor(YELLOW);
            fillCircleCoord(mouse.pos, 8);
            setColor(GREEN);
            fillCircleCoord(mrd, 8);
        }

        Wall *cur = map;
        while(cur){
            const Coord a = mapToScreen(off, scale, cur->a);
            const Coord b = mapToScreen(off, scale, cur->b);
            setColor(cur->c);
            drawLineCoords(a, b);
            cur = cur->next;
        }

        setColor(RED);
        drawHLine(0, off.y, wlen.x);
        drawVLine(off.x, 0, wlen.y);

        // if(snap){
        //
        //         drawVLine(spos.x, 0, wlen.y);
        //         drawHLine(0, spos.y, wlen.x);
        //         ;
        //     }
        // }

        if(selectedPos)
            fillCircleCoord(mapToScreen(off, scale, *selectedPos), 8);
        if(selectedWall)
            drawCircleCoord(mapToScreen(off, scale, &(selectedWall->a) == selectedPos ? selectedWall->b : selectedWall->a), 8);

        setColor(c);
        fillCircleCoord(wlen, 8);

        frameEnd(t);
    }
}

#endif /* end of include guard: MAP_H */
