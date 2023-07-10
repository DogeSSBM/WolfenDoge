#ifndef MAPEDITOR_H
#define MAPEDITOR_H

WallPacked* mapPack(Wall *map)
{
    const uint len = wallListLen(map);
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

Wall* mapUnpack(WallPacked *mapPacked, const uint len)
{
    if(len == 0)
        return NULL;
    Wall *map = NULL;
    for(uint i = 0; i < len; i++)
        map = wallAppend(map, wallNew(mapPacked[i].c, mapPacked[i].a, mapPacked[i].b));
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
    w->type = W_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    return w;
}

Wall* windNew(const Color c, const Color topColor, const Coordf a, const Coordf b, const float height, const float top)
{
    Wall *w = wallNew(c, a, b);
    w->type = W_WIND;
    w->wind.topColor = topColor;
    w->wind.height = height;
    w->wind.top = top;
    return w;
}

Wall* doorNew(const Color c, const Coordf a, const Coordf b, const uint id, const float pos, const bool state, const float speed, const Direction closeDir)
{
    Wall *w = wallNew(c, a, b);
    w->type = W_DOOR;
    w->door.id = id;
    w->door.pos = pos;
    w->door.state = state;
    w->door.speed = speed;
    w->door.closeDir = closeDir;
    return w;
}

Wall* trigNew(const Color color, const Coordf a, const Coordf b, const uint id, const Coordf c, const Coordf d)
{
    Wall *w = wallNew(color, a, b);
    w->type = W_TRIG;
    w->trig.id = id;
    w->trig.c = c;
    w->trig.d = d;
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
    Wall *map =           wallNew(
        GREEN,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=  0.0f}
    );
    map = wallAppend(map, wallNew(
        MAGENTA,
        (const Coordf){.x=750.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = wallAppend(map, wallNew(
        MAGENTA,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=  0.0f, .y=750.0f}
    ));
    map = wallAppend(map, wallNew(
        GREEN,
        (const Coordf){.x=  0.0f, .y=750.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = wallAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=250.0f, .y=500.0f},
        .25f, .25f
    ));
    map = wallAppend(map, windNew(BLUE, RED,
        (const Coordf){.x=250.0f, .y=500.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        .25f, .25f
    ));
    map = wallAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=500.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        .25f, .25f
    ));
    map = wallAppend(map, windNew(BLUE, RED,
        (const Coordf){.x=250.0f, .y=500.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        .25f, .25f
    ));
    map = wallAppend(map, doorNew(YELLOW,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=250.0f},
        0, 0.0f, false, 0.01f, DIR_U
    ));
    map = wallAppend(map, trigNew(YELLOW,
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

int editColor(Color *c, int ci, Wall *selectedWall)
{
    static int nums[3] = {0};
    ci = wrap(ci + keyPressed(SC_RIGHT) - keyPressed(SC_LEFT), 0, 3);
    u8* b = colorIndex(c, ci);
    const int change = (keyPressed(SC_UP) - keyPressed(SC_DOWN))*(keyCtrlState()?10:1);
    *b = clamp((int)(*b) + change, 0, 255);
    if(change){
        nums[2] = (*(colorIndex(c, ci))) / 100;
        nums[2] = ((*(colorIndex(c, ci)) / 10)%10);
        nums[0] = (*(colorIndex(c, ci)) %10);
    }
    const int num = numKeyPressed();
    if(num != -1){
        nums[2] = nums[1];
        nums[1] = nums[0];
        nums[0] = num;
        *b = clamp(nums[2]*100+nums[1]*10+nums[0], 0, 255);
    }

    if(keyPressed(SC_C) && selectedWall)
        selectedWall->color = *c;

    return ci;
}

bool checkEditorExit(void)
{
    return checkCtrlKey(SC_Q) || checkCtrlKey(SC_W);
}

bool checkKeyS(Wall *map, char *fileName, bool snap, const float snaplen)
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

void drawOriginLines(const Offset off, const Length wlen)
{
    setColor(WHITE);
    if(inBound(off.y, 0, wlen.y))
        drawHLine(0, off.y, wlen.x);
    if(inBound(off.x, 0, wlen.x))
        drawVLine(off.x, 0, wlen.y);
}

void drawGrid(const Offset off, const Length wlen, const float scale, const bool snap, const float snaplen)
{
    if(!snap)
        return;
    setColor(GREY);
    Coordf mpos = cfSnap(screenToMap(off, scale, iC(0,0)), snaplen);
    Coord spos = mapToScreen(off, scale, mpos);
    while(spos.x < wlen.x || spos.y < wlen.y){
        drawVLine(spos.x, 0, wlen.y);
        drawHLine(0, spos.y, wlen.x);
        mpos = cfAddf(mpos, snaplen);
        spos = mapToScreen(off, scale, mpos);
    }
}

void drawColor(const Length wlen, Color c, const int ci)
{
    const uint tscale = (wlen.x/3)/10;
    setTextSize(tscale);
    Coord pos = {.x = wlen.x-(wlen.x/3+tscale*2), .y = wlen.y-(tscale + tscale/4)};
    setColor(c);
    fillSquareCoord(iC(pos.x - tscale*2, pos.y), tscale);
    char letter[3] = {'R', 'G', 'B'};
    const Color indexcolor[3] = {RED, GREEN, BLUE};
    for(int i = 0; i < 3; i++){
        char buf[16] = {0};
        sprintf(buf, "%c: %3u", letter[i], *(colorIndex(&c, i)));
        setTextColor(i == ci ? indexcolor[i] : WHITE);
        drawTextCoord(buf, pos);
        pos.x += 4*tscale;
    }
}

void drawEditorMap(Wall *map, const Offset off, const float scale)
{
    Wall *cur = map;
    while(cur){
        const Coord a = mapToScreen(off, scale, cur->a);
        const Coord b = mapToScreen(off, scale, cur->b);
        setColor(cur->color);
        drawLineCoords(a, b);
        cur = cur->next;
    }
}

void drawSel(const Selection sel, const Offset off, const float scale)
{
    if(sel.pos)
        fillCircleCoord(mapToScreen(off, scale, *sel.pos), 8);
    if(sel.wall)
        drawCircleCoord(mapToScreen(off, scale, &(sel.wall->a) == sel.pos ? sel.wall->b : sel.wall->a), 8);
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

Minfo mlUpdate(Minfo ml, Selection *sel, Wall *map, const float scale, const bool snap, const float snaplen)
{
    if(!sel->wall && mouseBtnReleased(MOUSE_L)){
        sel->wall = posNearest(map, ml.mpos, &(sel->pos));
    }

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

Minfo mrUpdate(Minfo mr, Selection *sel, Wall **map, const Color c, const bool snap)
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
        Wall *newWall = wallNew(c, snap ? mr.msnapd : mr.mposd, snap ? mr.msnap : mr.mpos);
        *map = wallAppend(*map, newWall);
        sel->wall = newWall;
        sel->pos = &(newWall->a);
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

Wall* updateDel(Wall *map, Selection *sel)
{
    if((keyPressed(SC_DELETE) || keyPressed(SC_BACKSPACE)) && sel->wall){
        map = wallDelete(map, sel->wall);
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

Wall* mapEdit(Wall *map, char *fileName)
{
    SDL_SetRelativeMouseMode(false);
    float scale = 1.0f;
    bool snap = true;
    float snaplen = 50.0f;
    Coord off = {0};
    Length wlen = getWindowLen();
    Minfo ml = {0};
    Minfo mr = {0};
    Selection sel = {0};
    Color c = MAGENTA;
    int ci = 0;

    while(1){
        const uint t = frameStart();
        if(checkEditorExit())
            return map;

        snap = checkKeyS(map, fileName, snap, snaplen);
        ci = editColor(&c, ci, sel.wall);
        mlrUpdate(&ml, &mr, &sel, off, scale, snaplen);
        wlen = updateResize(wlen, &off);
        map = updateDel(map, &sel);
        checkScroll(&off, ml.mpos, snap, &snaplen, &scale);
        off = updatePan(off, &ml, &mr);
        ml = mlUpdate(ml, &sel, map, scale, snap, snaplen);
        mr = mrUpdate(mr, &sel, &map, c, snap);
        sel = selCheckRev(sel);

        drawGrid(off, wlen, scale, snap, snaplen);
        drawOriginLines(off, wlen);
        drawEditorMap(map, off, scale);
        drawSel(sel, off, scale);
        drawColor(wlen, c, ci);

        frameEnd(t);
    }
}


#endif /* end of include guard: MAPEDITOR_H */
