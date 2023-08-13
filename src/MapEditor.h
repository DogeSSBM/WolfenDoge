#ifndef MAPEDITOR_H
#define MAPEDITOR_H

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

// toggles snap if s is pressed and returns snap state
// saves map on ctrl + s
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

// main loop while in map editor. segment list map is edited and returned when exiting from map editor
// on save, map is saved to ../Maps/fileName or a default map name if fileName is NULL
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
        sel = updateSelCursor(sel);
        if(!sel.wall)
            sel.newSegType = wrap(sel.newSegType + keyPressed(SC_RIGHT) - keyPressed(SC_LEFT), 0, S_N);
        else if((sel.cursor.y == 3 || (sel.wall->type == S_WIND && sel.cursor.y == 4)))
            sel.cursor = editColor(sel.cursor, sel.cursor.y == 3 ? &sel.wall->color : &sel.wall->wind.topColor);
        else if(sel.wall->type == S_WALL && sel.cursor.y == 4){
            if(keyPressed(SC_RETURN))
                textInputStart(sel.wall->wall.path, 128, NULL);
            if(textInputEnded()){
                map = txtrCleanup(map, sel.wall);
                sel.wall->wall.texture = txtrQryLoad(map, sel.wall->wall.path);
            }
        }else if(sel.wall->type == S_WIND){
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
        updateMouseCommon(&ml, &mr, &sel, off, scale, snaplen);
        wlen = updateResize(wlen, &off);
        map = updateDel(map, &sel);
        checkScroll(&off, ml.mpos, snap, &snaplen, &scale);
        off = updatePan(off, &ml, &mr);
        ml = updateMouseL(ml, &sel, map, scale, snap, snaplen);
        mr = updateMouseR(mr, &sel, &map, c, snap);
        if(keyPressed(SC_I))
            sel.showInfo = !sel.showInfo;
        sel = updateSelNext(sel, map);
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
