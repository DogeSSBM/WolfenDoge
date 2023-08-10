#ifndef MAPEDITOR_H
#define MAPEDITOR_H

// Serializes the list of segments into an array
SegPacked mapPack(Seg *map)
{
    const uint len = segListLen(map);
    if(len == 0)
        return (SegPacked){0};
    SegPacked mapPacked = {
        .len = len,
        .seg = calloc(len, sizeof(Seg))
    };
    for(uint i = 0; i < len; i++){
        mapPacked.seg[i] = *map;
        map = map->next;
    }
    return mapPacked;
}

// De-serializes the map segment array into a list
Seg* mapUnpack(SegPacked mapPacked)
{
    if(mapPacked.len == 0 || !mapPacked.seg)
        return NULL;
    Seg *map = NULL;
    for(uint i = 0; i < mapPacked.len; i++){
        printf("copying seg %2u/%2u\n", i, mapPacked.len);
        Seg *seg = calloc(1, sizeof(Seg));
        memcpy(seg, &mapPacked.seg[i], sizeof(Seg));
        seg->next = NULL;
        map = segAppend(map, seg);
    }
    return map;
}

// takes pos, gets its position in terms of percentage of oldLen side lengths
// returns a new position offset by the same percentage of newLen's sides
Coord resizeTransform(const Length oldLen, const Length newLen, const Coord pos)
{
    return CfC(cfMul(CCf(newLen), cfDiv(CCf(pos), CCf(oldLen))));
}

// takes pos, gets its position in terms of percentage of oldLen
// returns a new position offset by the same percentage of newLen's sides
Coordf resizeTransformf(const Lengthf oldLen, const Lengthf newLen, const Coordf pos)
{
    return cfMul(newLen, cfDiv(pos, oldLen));
}

// iterates through all segments and their coords, sets *nearest = & the nearest coord
// returns the segment containing the nearest coord
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
        if(map->type == S_TRIG){
            curDst = cfDist(pos, map->trig.c);
            if(curDst < dst){
                dst = curDst;
                wall = map;
                *nearest = &(map->trig.c);
            }
            curDst = cfDist(pos, map->trig.d);
            if(curDst < dst){
                dst = curDst;
                wall = map;
                *nearest = &(map->trig.d);
            }
        }
        map = map->next;
    }
    return wall;
}

// returns the segment containing the next coord and sets pos to the next coord
// returns NULL if end is reached
Seg* coordNext(Seg *seg, Coordf **pos)
{
    if(!seg || !pos)
        return NULL;
    const uint numCoords = seg->type == S_TRIG ? 4 : 2;
    Coordf *coord[4] = {&seg->a, &seg->b};
    if(seg->type == S_TRIG){
        coord[2] = &seg->trig.c;
        coord[3] = &seg->trig.d;
    }
    uint i = 0;
    for(i = 0; i < numCoords; i++)
        if(coord[i] == *pos)
            break;
    assertExpr(i < numCoords);
    if(i+1 < numCoords){
        *pos = coord[i+1];
        return seg;
    }
    if(!seg->next)
        return NULL;
    *pos = &seg->next->a;
    return seg->next;
}

// returns next coord, wraps if end is reached
Seg* coordNextWrap(Seg *map, Seg *seg, Coordf **pos)
{
    if(!map)
        return NULL;
    Seg *next = coordNext(seg, pos);
    if(next)
        return next;
    *pos = &map->a;
    return map;
}

// iterates from seg sets *nextCoord = & the coord with same value as pos
// if none found, iterates from map up to seg
// returns the segment containing the nextCoord coord
Seg* posNext(Seg *map, Seg *seg, Coordf **pos)
{
    if(!map || !seg){
        return NULL;
    }
    Seg* next = seg;
    Coordf *nextCoord = *pos;
    do{
        seg = coordNextWrap(map, seg, &nextCoord);
        if(cfSame(*nextCoord, **pos)){
            *pos = nextCoord;
            return seg;
        }
    }while(nextCoord != *pos);
    return next;
}

// attempts to open map.bork then map(n).bork with n starting at 1 and increasing
// once a file name that doesnt exist is found returns n
uint newMapFileNum(void)
{
    uint n = 0;
    File *file = NULL;
    char path[256] = "./Maps/map.bork";
    while((file = fopen(path, "rb")) != NULL){
        fclose(file);
        n++;
        sprintf(path, "./Maps/map(%u).bork", n);
    }
    return n;
}

// saves map to path
void mapSave(Seg *map, char *path)
{
    if(!map){
        printf("Map is empty, skipping save\n");
        return;
    }
    assertExpr(path);
    File *file = NULL;
    file = fopen(path, "wb");
    SegPacked mapPacked = mapPack(map);
    printf("Writing map of length: %u to \"%s\"\n", mapPacked.len, path);
    fwrite(&mapPacked.len, sizeof(uint), 1, file);
    fwrite(mapPacked.seg, sizeof(Seg), mapPacked.len, file);
    fclose(file);
    free(mapPacked.seg);
}

// searches for wall with texture path, if found returns the texture else returns
// newly allocated texture
Texture* txtrQryLoad(Seg *map, char *path)
{
    while(map){
        if(map->type == S_WALL && map->wall.path && map->wall.texture && !strcmp(map->wall.path, path))
            return map->wall.texture;
        map = map->next;
    }
    printf("Loading texture: \"%s\"\n", path);
    return loadTexture(path);
}

// creates a new segment with type S_WALL
Seg* wallNew(const Color c, const Coordf a, const Coordf b)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    return w;
}

// creates a new segment with type S_WALL that has a texture
Seg* txtrNew(Seg *map, const Color c, const Coordf a, const Coordf b, char *path)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    w->wall.path = path;
    w->wall.texture = txtrQryLoad(map, path);
    return w;
}

// creates a new segment with type S_WIND
Seg* windNew(const Color c, const Color topColor, const Coordf a, const Coordf b, const float height, const float top)
{
    Seg *w = wallNew(c, a, b);
    w->type = S_WIND;
    w->wind.topColor = topColor;
    w->wind.height = height;
    w->wind.top = top;
    return w;
}

// creates a new segment with type S_DOOR
Seg* doorNew(const Color c, const Coordf a, const Coordf b, const uint id, const float pos, const bool state, const float speed, const Direction closeDir)
{
    Seg *w = wallNew(c, a, b);
    w->type = S_DOOR;
    w->door.id = id;
    w->door.pos = pos;
    w->door.state = state;
    w->door.speed = speed;
    w->door.closeDir = closeDir;
    return w;
}

// creates a new segment with type S_TRIG
Seg* trigNew(const Color color, const Coordf a, const Coordf b, const uint id, const Coordf c, const Coordf d)
{
    Seg *w = wallNew(color, a, b);
    w->type = S_TRIG;
    w->trig.id = id;
    w->trig.c = c;
    w->trig.d = d;
    return w;
}

// creates a new segment with type S_CONV
Seg* convNew(const Color c, const Coordf a, const Coordf b, const uint idA, const uint idB)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_CONV;
    w->color = c;
    w->a = a;
    w->b = b;
    w->conv.idA = idA;
    w->conv.idB = idB;
    return w;
}

// appends tail to the end of the list (head)
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

// searches for del in map list, removes and frees it
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

// returns the number of segments in the map list
uint segListLen(Seg *map)
{
    uint len = 0;
    while(map){
        len++;
        map = map->next;
    }
    return len;
}

// frees all segments in the list
void segFreeList(Seg *list)
{
    while(list){
        Seg *next = list->next;
        free(list);
        list = next;
    }
}

// allocates and returns the default map
Seg* mapDefault(void)
{
    Seg *map =           wallNew(GREEN,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=  0.0f}
    );
    map = segAppend(map, wallNew(MAGENTA,
        (const Coordf){.x=750.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = segAppend(map, wallNew(MAGENTA,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=  0.0f, .y=750.0f}
    ));
    map = segAppend(map, wallNew(GREEN,
        (const Coordf){.x=  0.0f, .y=750.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = segAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=250.0f, .y=500.0f},
        .25f, .25f
    ));
    map = segAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=500.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        .25f, .25f
    ));
    map = segAppend(map, txtrNew(map, WHITE,
        (const Coordf){.x=250.0f, .y=500.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        "./Assets/Bricks64x64.png"
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
    map = segAppend(map, trigNew(YELLOW,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=250.0f},
        0,
        (const Coordf){.x=250.0f, .y=500.0f},
        (const Coordf){.x=500.0f, .y=500.0f}
    ));
    return map;
}

// returns the int corrosponding to the number key pressed
// -1 if no number keys were pressed
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

// edits color component (r, g, or b) according to the cursor position.
// changes cursor.x if left or right arrow key is pressed
// returns cursor position
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

// if a number key was pressed, shifts the digits of u left inserting the
// new number in the ones place
uint editUint(uint u)
{
    const int num = numKeyPressed();
    if(num == -1)
        return u;
    u = u*10 + num;
    return u%1000;
}

// if a number key was pressed, shifts the digits of u left inserting the
// new number in the thousands place (clamps f such that 1.0 >= f >= 0)
float editFloat(float f)
{
    static int nums[4] = {0};
    const int num = numKeyPressed();
    if(num != -1){
        nums[3] = nums[2];
        nums[2] = nums[1];
        nums[1] = nums[0];
        nums[0] = num;
        f = nums[3] + nums[2]/10.0f + nums[1]/100.0f + nums[0]/1000.0f;
        if(f < 0)
            f = 0;
        if(f > 1.0f)
            f = 1.0f;
    }
    return f;
}

// returns true if ctrl + q or ctrl + w is pressed
bool checkEditorExit(void)
{
    return checkCtrlKey(SC_Q) || checkCtrlKey(SC_W);
}

// toggles snap if s is pressed and returns snap
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

// checks if the mouse wheel is scrolled.
// if scrolled while holding ctrl, updates *snaplen
// otherwise if scrolled updates *scale and *offset such that mmpos is at the same
// screen position after scaling
// also updates *snaplen if ctrl + - or ctrl + + is pressed
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

// updates left and right mouse button info
// clears selection wall / pos  and drag info if escape is pressed
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

// updates left mouse button info
// updates selection if left mouse is pressed, dragged, or released
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

// updates right mouse button info
// updates selection if right mouse is pressed, dragged, or released
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
        Seg *newSeg = NULL;
        const Coordf a = snap ? mr.msnapd : mr.mposd;
        const Coordf b = snap ? mr.msnap : mr.mpos;
        switch(sel->newSegType){
            case S_WALL:
                newSeg = wallNew(c, a, b);
                break;
            case S_WIND:
                newSeg = windNew(c, c, a, b, .25f, .25f);
                break;
            case S_DOOR:
                newSeg = doorNew(c, a, b, 0, 0.0f, false, 0.01f, DIR_D);
                break;
            case S_TRIG:
                newSeg = trigNew(c, a, b, 0, cfAddf(a, 100.0f), cfAddf(b, 100.0f));
                break;
            case S_CONV:
                newSeg = convNew(c, a, b, 0, 0);
                break;
            default:
                panic("uh oh");
                break;
        }
        assertExpr(newSeg);
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

// updates selection cursor.y if up or down arrow keys are pressed
// sets cursor.x to 0 if cursor.y changed to a selection that only has a single option
Selection selUpdateCursor(Selection sel)
{
    if(!sel.wall)
        return sel;
    sel.cursor.y = wrap(sel.cursor.y + keyPressed(SC_DOWN) - keyPressed(SC_UP), 0, SegTypeNumFields[sel.wall->type]);
    if(!(
        sel.cursor.y == 3 ||
        (sel.wall->type == S_WIND && sel.cursor.y == 4) ||
        (sel.wall->type == S_DOOR && sel.cursor.y == 8)
    ))
        sel.cursor.x = 0;
    return sel;
}

// updates selection to next seg / coord that overlaps
Selection selUpdateNext(Selection sel, Seg *map)
{
    if(!keyPressed(SC_N))
        return sel;
    sel.wall = posNext(map, sel.wall, &sel.pos);
    return sel;
}

// if a segment is selected, deletes it from map segment list and updates *sel
Seg* updateDel(Seg *map, Selection *sel)
{
    if((keyPressed(SC_DELETE) || keyPressed(SC_BACKSPACE)) && sel->wall){
        map = segDelete(map, sel->wall);
        sel->wall = NULL;
        sel->pos = NULL;
    }
    return map;
}

// if window is resized updates it preserving off relative to new window lengths
// eg off will be in the same place in terms of percentage offset relative to old / new window lengths
Length updateResize(Length wlen, Offset *off)
{
    if(windowResized()){
        const Length wlenOld = wlen;
        wlen = getWindowLen();
        *off = resizeTransform(wlenOld, wlen, *off);
    }
    return wlen;
}

// pans map while middle mouse or left shift is held
Offset updatePan(Offset off, Minfo *ml, Minfo *mr)
{
    if(mouseBtnState(MOUSE_M) || keyState(SC_LSHIFT)){
        off = coordAdd(off, mouseMovement());
        mr->sposd = mr->spos;
        ml->sposd = ml->spos;
    }
    return off;
}

// main loop while in map editor. segment list map is edited and returned when exiting from map editor
// on save, map is saved to ./Maps/fileName or a default map name if fileName is NULL
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
        if(!sel.wall)
            sel.newSegType = wrap(sel.newSegType + keyPressed(SC_RIGHT) - keyPressed(SC_LEFT), 0, S_N);
        else if((sel.cursor.y == 3 || (sel.wall->type == S_WIND && sel.cursor.y == 4)))
            sel.cursor = editColor(sel.cursor, sel.cursor.y == 3 ? &sel.wall->color : &sel.wall->wind.topColor);
        else if(sel.wall->type == S_WIND){
            if(sel.cursor.y == 5)
                sel.wall->wind.height = editFloat(sel.wall->wind.height);
            else if(sel.cursor.y == 6)
                sel.wall->wind.top = editFloat(sel.wall->wind.top);
        }else if(sel.wall->type == S_DOOR){
            if(sel.cursor.y == 4)
                sel.wall->door.id = editUint(sel.wall->door.id);
            else if(sel.cursor.y == 5)
                sel.wall->door.pos = editFloat(sel.wall->door.pos);
            else if(sel.cursor.y == 7)
                sel.wall->door.speed = editFloat(sel.wall->door.speed);
        }else if(sel.wall->type == S_TRIG){
            if(sel.cursor.y == 4)
                sel.wall->trig.id = editUint(sel.wall->trig.id);
        }else if(sel.wall->type == S_CONV){
            if(sel.cursor.y == 4)
                sel.wall->conv.idA = editUint(sel.wall->conv.idA);
            else if(sel.cursor.y == 5)
                sel.wall->conv.idB = editUint(sel.wall->conv.idB);
        }
        mlrUpdate(&ml, &mr, &sel, off, scale, snaplen);
        wlen = updateResize(wlen, &off);
        map = updateDel(map, &sel);
        checkScroll(&off, ml.mpos, snap, &snaplen, &scale);
        off = updatePan(off, &ml, &mr);
        ml = mlUpdate(ml, &sel, map, scale, snap, snaplen);
        mr = mrUpdate(mr, &sel, &map, c, snap);
        if(keyPressed(SC_I))
            sel.showInfo = !sel.showInfo;
        sel = selUpdateNext(sel, map);
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
