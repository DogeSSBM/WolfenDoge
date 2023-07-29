#ifndef MAPEDITOR_H
#define MAPEDITOR_H

WallPacked* mapPack(Seg *map)
{
    const uint len = segListLen(map);
    if(len == 0)
        return NULL;
    WallPacked *mapPacked = calloc(len, sizeof(WallPacked));
    for(uint i = 0; i < len; i++){
        mapPacked[i].c = map->color;
        mapPacked[i].a = map->a;
        mapPacked[i].b = map->b;
        map = map->next;
    }
    return mapPacked;
}

Seg* mapUnpack(WallPacked *mapPacked, const uint len)
{
    if(len == 0)
        return NULL;
    Seg *map = NULL;
    for(uint i = 0; i < len; i++)
        map = segAppend(map, wallNew(mapPacked[i].c, mapPacked[i].a, mapPacked[i].b));
    return map;
}

Coord resizeTransform(const Length oldLen, const Length newLen, const Coord pos)
{
    return CfC(cfMul(CCf(newLen), cfDiv(CCf(pos), CCf(oldLen))));
}

Coordf resizeTransformf(const Lengthf oldLen, const Lengthf newLen, const Coordf pos)
{
    return cfMul(newLen, cfDiv(pos, oldLen));
}

Seg* posNearest(Seg *map, const Coordf pos, Coordf **nearest)
{
    if(!map){
        nearest = NULL;
        return NULL;
    }
    Seg *wall = map;
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

void mapSave(Seg *map, char *fileName)
{
    if(!map){
        printf("Map is empty, skipping save\n");
        return;
    }
    File *file = NULL;
    if(!fileName){
        char defaultName[64] = {0};
        defaultMapFileName(defaultName);
        fileName = defaultName;
    }
    char path[256] = {0};
    sprintf(path, "./Maps/%s", fileName);
    file = fopen(path, "wb");
    const uint len = segListLen(map);
    WallPacked *mapPacked = mapPack(map);
    fwrite(&len, sizeof(uint), 1, file);
    fwrite(mapPacked, sizeof(WallPacked), len, file);
    fclose(file);
    free(mapPacked);
}

Seg* wallNew(const Color c, const Coordf a, const Coordf b)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = W_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    return w;
}

Seg* windNew(const Color c, const Color topColor, const Coordf a, const Coordf b, const float height, const float top)
{
    Seg *w = wallNew(c, a, b);
    w->type = W_WIND;
    w->wind.topColor = topColor;
    w->wind.height = height;
    w->wind.top = top;
    return w;
}

Seg* doorNew(const Color c, const Coordf a, const Coordf b, const uint id, const float pos, const bool state, const float speed, const Direction closeDir)
{
    Seg *w = wallNew(c, a, b);
    w->type = W_DOOR;
    w->door.id = id;
    w->door.pos = pos;
    w->door.state = state;
    w->door.speed = speed;
    w->door.closeDir = closeDir;
    return w;
}

Seg* trigNew(const Color color, const Coordf a, const Coordf b, const uint id, const Coordf c, const Coordf d)
{
    Seg *w = wallNew(color, a, b);
    w->type = W_TRIG;
    w->trig.id = id;
    w->trig.c = c;
    w->trig.d = d;
    return w;
}

Seg* segAppend(Seg *head, Seg *tail)
{
    if(!head)
        return tail;
    Seg *cur = head;
    while(cur->next)
        cur = cur->next;
    cur->next = tail;
    return head;
}

Seg* segDelete(Seg *map, Seg *del)
{
    if(!del)
        return map;
    if(!map)
        return NULL;
    if(del == map){
        Seg *next = map->next;
        free(map);
        return next;
    }
    Seg *cur = map;
    while(cur && cur->next != del)
        cur = cur->next;
    if(!cur)
        return map;
    Seg *next = cur->next->next;
    free(cur->next);
    cur->next = next;
    return map;
}

uint segListLen(Seg *map)
{
    uint len = 0;
    while(map){
        len++;
        map = map->next;
    }
    return len;
}

void segFreeList(Seg *list)
{
    while(list){
        Seg *next = list->next;
        free(list);
        list = next;
    }
}

Seg* mapDefault(void)
{
    Seg *map =           wallNew(
        GREEN,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=  0.0f}
    );
    map = segAppend(map, wallNew(
        MAGENTA,
        (const Coordf){.x=750.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = segAppend(map, wallNew(
        MAGENTA,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=  0.0f, .y=750.0f}
    ));
    map = segAppend(map, wallNew(
        GREEN,
        (const Coordf){.x=  0.0f, .y=750.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = segAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=250.0f, .y=500.0f},
        .25f, .25f
    ));
    map = segAppend(map, doorNew(YELLOW,
        (const Coordf){.x=500.0f, .y=0.0f},
        (const Coordf){.x=500.0f, .y=250.0f},
        0, 0.0f, true, 0.01f, DIR_D
    ));
    map = segAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=500.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        .25f, .25f
    ));
    map = segAppend(map, windNew(BLUE, RED,
        (const Coordf){.x=250.0f, .y=500.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        .10, .25f
    ));
    map = segAppend(map, doorNew(YELLOW,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=250.0f},
        0, 0.0f, false, 0.01f, DIR_D
    ));
    map = segAppend(map, trigNew(YELLOW,
        (const Coordf){.x=250.0f, .y=0.0f},
        (const Coordf){.x=500.0f, .y=0.0f},
        0,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=250.0f}
    ));
    return map;
}

int numKeyPressed(void)
{
    const Scancode key[10] = {
        SC_0,
        SC_1,
        SC_2,
        SC_3,
        SC_4,
        SC_5,
        SC_6,
        SC_7,
        SC_8,
        SC_9
    };
    for(int i = 0; i < 10; i++){
        if(keyPressed(key[i]))
            return i;
    }
    return -1;
}

Coord editColor(Coord cursor, Color *color)
{
    static int nums[3] = {0};
    cursor.x = wrap(cursor.x + keyPressed(SC_RIGHT) - keyPressed(SC_LEFT), 0, 3);
    u8* b = colorIndex(color, cursor.x);
    const int num = numKeyPressed();
    if(num != -1){
        nums[2] = nums[1];
        nums[1] = nums[0];
        nums[0] = num;
        *b = clamp(nums[2]*100+nums[1]*10+nums[0], 0, 255);
    }

    return cursor;
}

bool checkEditorExit(void)
{
    return checkCtrlKey(SC_Q) || checkCtrlKey(SC_W);
}

bool checkKeyS(Seg *map, char *fileName, bool snap, const float snaplen)
{
    if(checkCtrlKey(SC_S)){
        mapSave(map, fileName);
        printf("Saved \"%s\"\n", fileName);
    }else if(keyPressed(SC_S)){
        snap = !snap;
        printf("Snap (%4.0f) %s\n", snaplen, snap?"On":"Off");
    }
    return snap;
}

void checkScroll(Offset *off, const Coordf mmpos, const bool snap, float *snaplen, float *scale)
{
    float oldSnaplen = *snaplen;
    if(mouseScrolledY()){
        if(keyState(SC_LCTRL) || keyState(SC_RCTRL)){
            *snaplen = (float)imax(1, (int)*snaplen + mouseScrolledY());
        }else{
            const float oldScale = *scale;
            *scale = fclamp(*scale * (mouseScrolledY() > 0 ? 1.2f : .8f) , .01f, 100.0f);
            if(oldScale != *scale){
                const Coord ompos = mapToScreen(*off, *scale, mmpos);
                const Coord diff = coordSub(mouse.pos, ompos);
                *off = coordAdd(*off, diff);
            }
        }
    }
    *snaplen = (float)imax(1, (int)*snaplen + checkCtrlKey(SC_EQUALS)-checkCtrlKey(SC_MINUS));
    if(*snaplen != oldSnaplen)
        printf("Snap %4.0f (%s)\n", *snaplen, snap?"On":"Off");
}

void mlrUpdate(Minfo *ml, Minfo *mr, Selection *sel, const Offset off, const float scale, const float snaplen)
{
    ml->spos = mouse.pos;
    mr->spos = ml->spos;
    ml->mpos = screenToMap(off, scale, ml->spos);
    mr->mpos = ml->mpos;
    ml->msnap = cfSnapMid(ml->mpos, snaplen);
    mr->msnap = ml->msnap;
    ml->ssnap = mapToScreen(off, scale, ml->msnap);
    mr->ssnap = ml->ssnap;
    if(keyPressed(SC_ESCAPE)){
        sel->wall = NULL;
        sel->pos = NULL;
        mr->drag = false;
        ml->drag = false;
    }
}

Minfo mlUpdate(Minfo ml, Selection *sel, Seg *map, const float scale, const bool snap, const float snaplen)
{
    if(!sel->wall && mouseBtnReleased(MOUSE_L))
        sel->wall = posNearest(map, ml.mpos, &(sel->pos));

    if(mouseBtnPressed(MOUSE_L) && sel->pos){
        ml.drag = true;
        ml.sposd = ml.spos;
        ml.ssnapd = ml.ssnap;
        ml.mposd = ml.mpos;
        ml.msnapd = ml.msnap;
        sel->posOrig = *(sel->pos);
        if(snap)
            sel->posOrig = cfSnapMid(*(sel->pos), snaplen);
    }

    if(ml.drag && mouseBtnReleased(MOUSE_L)){
        ml.drag = false;
    }

    if(ml.drag && sel->pos){
        if(snap)
            *(sel->pos) = cfAdd(sel->posOrig, cfSnapMid(cfSub(ml.mpos, ml.mposd), snaplen));
        else
            *(sel->pos) = cfAdd(*(sel->pos), cfMulf(CCf(mouseMovement()), scale));
    }

    return ml;
}

Minfo mrUpdate(Minfo mr, Selection *sel, Seg **map, const Color c, const bool snap)
{
    if(mouseBtnPressed(MOUSE_R)){
        mr.sposd = mr.spos;
        mr.ssnapd = mr.ssnap;
        mr.mposd = mr.mpos;
        mr.msnapd = mr.msnap;
        mr.drag = true;
    }

    if(mr.drag && mouseBtnReleased(MOUSE_R)){
        mr.drag = false;
        Seg *newSeg = wallNew(c, snap ? mr.msnapd : mr.mposd, snap ? mr.msnap : mr.mpos);
        *map = segAppend(*map, newSeg);
        sel->wall = newSeg;
        sel->pos = &(newSeg->a);
    }

    if(mr.drag){
        setColor(c);
        drawLineCoords(snap ? mr.ssnapd : mr.sposd, snap ? mr.ssnap : mr.spos);
        setColor(YELLOW);
        fillCircleCoord(snap ? mr.ssnap : mr.spos, 8);
        setColor(GREEN);
        fillCircleCoord(snap ? mr.ssnapd : mr.sposd, 8);
    }

    return mr;
}

Selection selCheckRev(Selection sel)
{
    if(sel.wall && sel.pos && keyPressed(SC_R))
        sel.pos = sel.pos == &(sel.wall->a) ? &(sel.wall->b) : &(sel.wall->a);
    return sel;
}

Selection selUpdateCursor(Selection sel)
{
    if(!sel.wall)
        return sel;
    sel.cursor.y = wrap(sel.cursor.y + keyPressed(SC_DOWN) - keyPressed(SC_UP), 0, sel.wall->type == W_DOOR ? 9 : 7);
    return sel;
}

Seg* updateDel(Seg *map, Selection *sel)
{
    if((keyPressed(SC_DELETE) || keyPressed(SC_BACKSPACE)) && sel->wall){
        map = segDelete(map, sel->wall);
        sel->wall = NULL;
        sel->pos = NULL;
    }
    return map;
}

Length updateResize(Length wlen, Offset *off)
{
    if(windowResized()){
        const Length wlenOld = wlen;
        wlen = getWindowLen();
        *off = resizeTransform(wlenOld, wlen, *off);
    }
    return wlen;
}

Offset updatePan(Offset off, Minfo *ml, Minfo *mr)
{
    if(mouseBtnState(MOUSE_M) || keyState(SC_LSHIFT)){
        off = coordAdd(off, mouseMovement());
        mr->sposd = mr->spos;
        ml->sposd = ml->spos;
    }
    return off;
}

Seg* mapEdit(Seg *map, char *fileName)
{
    setRelativeMouse(false);
    float scale = 1.0f;
    bool snap = true;
    float snaplen = 50.0f;
    Coord off = {0};
    Length wlen = getWindowLen();
    Minfo ml = {0};
    Minfo mr = {0};
    Selection sel = {.showInfo = true, .cursor = iC(0,3)};
    Color c = MAGENTA;

    while(1){
        const uint t = frameStart();
        if(checkEditorExit())
            return map;

        snap = checkKeyS(map, fileName, snap, snaplen);
        sel = selUpdateCursor(sel);
        if(sel.wall && (sel.cursor.y == 3 || (sel.wall->type == W_WIND && sel.cursor.y == 4)))
            sel.cursor = editColor(sel.cursor, sel.cursor.y == 3 ? &sel.wall->color : &sel.wall->wind.topColor);
        mlrUpdate(&ml, &mr, &sel, off, scale, snaplen);
        wlen = updateResize(wlen, &off);
        map = updateDel(map, &sel);
        checkScroll(&off, ml.mpos, snap, &snaplen, &scale);
        off = updatePan(off, &ml, &mr);
        ml = mlUpdate(ml, &sel, map, scale, snap, snaplen);
        mr = mrUpdate(mr, &sel, &map, c, snap);
        sel = selCheckRev(sel);
        sel.showInfo = keyPressed(SC_I) ? !sel.showInfo : sel.showInfo;
        sel.tscale = (wlen.y/3)/12;
        setTextSize(sel.tscale);

        drawGrid(off, wlen, scale, snap, snaplen);
        drawOriginLines(off, wlen);
        drawEditorMap(map, sel, off, scale);
        drawSel(sel, off, scale);

        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITOR_H */
