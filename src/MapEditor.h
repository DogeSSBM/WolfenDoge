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

// iterates through all segments and or objects and their coords, sets bnearest to the nearest coord
// returns the segment containing the nearest coord
MapPiece posNearest(Map *map, const Coordf pos, const MapPieceType pieceType, Coordf **nearest)
{
    if(!map){
        nearest = NULL;
        return (MapPiece){.type = M_NONE};
    }
    MapPiece piece = {.type = M_SEG, .seg = NULL};
    float dst = 99999999.0f;
    if(pieceType == M_ANY || pieceType == M_SEG){
        for(SegType type = 0; type < S_N; type++){
            Seg *seg = map->seg[type];
            while(seg){
                float curDst = cfDist(pos, seg->a);
                if(curDst < dst){
                    piece.type = M_SEG;
                    piece.seg = seg;
                    dst = curDst;
                    *nearest = &(seg->a);
                }
                curDst = cfDist(pos, seg->b);
                if(curDst < dst){
                    piece.type = M_SEG;
                    piece.seg = seg;
                    dst = curDst;
                    *nearest = &(seg->b);
                }
                if(seg->type == S_TRIG){
                    curDst = cfDist(pos, seg->trig.c);
                    if(curDst < dst){
                        piece.type = M_SEG;
                        piece.seg = seg;
                        dst = curDst;
                        *nearest = &(seg->trig.c);
                    }
                    curDst = cfDist(pos, seg->trig.d);
                    if(curDst < dst){
                        piece.type = M_SEG;
                        piece.seg = seg;
                        dst = curDst;
                        *nearest = &(seg->trig.d);
                    }
                }
                seg = seg->next;
            }

        }
    }

    if(pieceType == M_ANY || pieceType == M_OBJ){
        for(ObjType type = 0; type < O_N; type++){
            Obj *obj = map->obj[type];
            while(obj){
                float curDst = cfDist(pos, obj->pos);
                if(curDst < dst){
                    piece.type = M_OBJ;
                    piece.obj = obj;
                    dst = curDst;
                    *nearest = &(obj->pos);
                }
                if(obj->type == O_MOB && (curDst = cfDist(pos, obj->mob.origin)) < dst){
                    piece.type = M_OBJ;
                    piece.obj = obj;
                    dst = curDst;
                    *nearest = &(obj->mob.origin);
                }
                obj = obj->next;
            }
        }
    }

    return piece;
}

// if *pos == NULL, sets it to first coord of piece
// sets pos to next coord in piece, if none remain, sets pos to first coord of next
// piece in list, if none remain, pos is unchanged and a piece of type M_NONE
MapPiece coordNext(MapPiece piece, Coordf **pos)
{
    if(piece.type >= M_ANY || !pos)
        return piece;
    if(piece.type == M_SEG){
        assertExpr(piece.seg);
        if(!*pos){
            *pos = &piece.seg->a;
            return piece;
        }
        const uint numCoords = piece.seg->type == S_TRIG ? 4 : 2;
        Coordf *coord[4] = {&piece.seg->a, &piece.seg->b};
        if(piece.seg->type == S_TRIG){
            coord[2] = &piece.seg->trig.c;
            coord[3] = &piece.seg->trig.d;
        }
        uint i = 0;
        for(i = 0; i < numCoords; i++)
            if(coord[i] == *pos)
                break;
        assertExpr(i < numCoords);
        if(i+1 < numCoords){
            *pos = coord[i+1];
            return piece;
        }
        if(!piece.seg->next)
            return (MapPiece){.type = M_NONE};
        *pos = &piece.seg->next->a;
        return (MapPiece){.type = M_SEG, .seg = piece.seg->next};
    }

    assertExpr(piece.type == M_OBJ && piece.obj);
    if(!*pos){
        *pos = &piece.obj->pos;
        return piece;
    }
    if(piece.obj->type == O_MOB && *pos == &piece.obj->pos){
        *pos = &piece.obj->mob.origin;
        return piece;
    }

    if(piece.obj->next){
        piece.obj = piece.obj->next;
        *pos = &piece.obj->pos;
        return piece;
    }

    piece.type = M_NONE;
    return piece;
}

bool mapPieceEndOfList(const MapPiece piece)
{
    return piece.type >= M_ANY || (piece.type == M_SEG && piece.seg == NULL) || (piece.type == M_OBJ && piece.obj == NULL);
}

int mapPiecePieceType(const MapPiece piece)
{
    assertExpr(piece.type < M_ANY);
    if(piece.type == M_SEG)
        return piece.seg->type;
    return piece.obj->type;
}

// returns a MapPiece with type and its corrosponding list
MapPiece mapPieceFromListOfType(MapPieceType type, void *list)
{
    assertExpr(type < M_ANY && list);
    if(type == M_SEG)
        return (MapPiece){.type = type, .seg = list};
    return (MapPiece){.type = type, .obj = list};
}

// returns either a seg list or an obj list depending on type
// returns the list corrosponding to the respective mapPiecePieceType of index
void* mapGetPieceListOfTypeAtIndex(Map *map, const MapPieceType type, const int index)
{
    assertExpr(type < M_ANY);
    assertExpr(index < PieceTypeNum[type]);
    if(type == M_SEG)
        return map->seg[index];
    return map->obj[index];
}

// returns a MapPiece with type type, whos list is from map at the corrosponding map piece index
MapPiece mapPieceOfTypeAtIndex(Map *map, const MapPieceType type, const int index)
{
    return mapPieceFromListOfType(type, mapGetPieceListOfTypeAtIndex(map, type, index));
}

// returns next coord, in map piece type list of corrosponding index
// if end is reached, goes to the next index that contains any list elements
// if none are found, repeats process starting at lowest index of opposite MapPieceType
// that contains a list. if the end of this is reached, begins again starting
// with the first index containing a list of the original MapPieceType
// if the original coord and piece are then encountered, they are returned
// as if pos and piece were unmodified
MapPiece coordNextWrap(Map *map, MapPiece piece, Coordf **pos)
{
    if(!map || piece.type >= M_ANY)
        return piece;
    MapPiece next = coordNext(piece, pos);
    if(next.type < M_ANY)
        return next;
    int type = 0;
    for(type = mapPiecePieceType(piece)+1; type < PieceTypeNum[piece.type]; type++){
        void *list = mapGetPieceListOfTypeAtIndex(map, piece.type, type);
        if(list){
            *pos = NULL;
            return coordNext(mapPieceFromListOfType(piece.type, list), pos);
        }
    }

    assertExpr(type == PieceTypeNum[piece.type]);
    *pos = NULL;
    piece.type = (piece.type + 1) % 2;
    for(type = mapPiecePieceType(piece); type < PieceTypeNum[piece.type]; type++){
        void *list = mapGetPieceListOfTypeAtIndex(map, piece.type, type);
        if(list){
            *pos = NULL;
            return coordNext(mapPieceFromListOfType(piece.type, list), pos);
        }
    }
    assertExpr(type == PieceTypeNum[piece.type]);
    *pos = NULL;
    piece.type = (piece.type + 1) % 2;
    for(type = mapPiecePieceType(piece); type < PieceTypeNum[piece.type]; type++){
        void *list = mapGetPieceListOfTypeAtIndex(map, piece.type, type);
        if(list){
            *pos = NULL;
            return coordNext(mapPieceFromListOfType(piece.type, list), pos);
        }
    }
    // panic("good greif");
    return piece;
}

// returns next coord, in map piece type list of corrosponding index
// if end is reached, goes to the next index that contains any list elements
// if none are found, repeats process starting at lowest index of opposite MapPieceType
// that contains a list. if the end of this is reached, begins again starting
// with the first index containing a list of the original MapPieceType
// if the original coord and piece are then encountered, they are returned
// as if pos and piece were unmodified
MapPiece posNext(Map *map, MapPiece piece, Coordf **pos)
{
    if(!map || piece.type >= M_ANY || (piece.type == M_SEG && !piece.seg) || (piece.type == M_OBJ && !piece.obj)){
        return (MapPiece){.type = M_NONE};
    }

    MapPiece next = piece;
    Coordf *nextCoord = *pos;
    do{
        piece = coordNextWrap(map, piece, &nextCoord);
        if(cfSame(*nextCoord, **pos)){
            *pos = nextCoord;
            return piece;
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
bool checkKeyS(Map *map, const bool snap, const float snaplen)
{
    if(checkCtrlKey(SC_S)){
        mapSave(map);
        printf("Saved \"%s\": to \"%s\"\n", map->name ? map->name : "", map->path);
    }else if(keyPressed(SC_S)){
        printf("Snap (%4.0f) %s\n", snaplen, !snap ? "On" : "Off");
        return !snap;
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

// main loop while in map editor. on save, map is saved to map->path
void mapEdit(Map *map)
{
    setRelativeMouse(false);
    float scale = 1.0f;
    bool snap = true;
    float snaplen = 50.0f;
    Coord off = {0};
    Length wlen = getWindowLen();
    Minfo ml = {0};
    Minfo mr = {0};
    Selection sel = {.piece.type = M_NONE, .showInfo = true, .cursor = iC(0,3)};
    Color c = MAGENTA;
    while(1){
        const uint t = frameStart();
        if(checkEditorExit())
            return;

        snap = checkKeyS(map, snap, snaplen);
        sel = updateSelCursor(sel);
        if(!sel.piece.seg)
            sel.newSegType = wrap(sel.newSegType + keyPressed(SC_RIGHT) - keyPressed(SC_LEFT), 0, S_N);
        else if((sel.cursor.y == 3 || (sel.piece.seg->type == S_WIND && sel.cursor.y == 4)))
            sel.cursor = editColor(sel.cursor, sel.cursor.y == 3 ? &sel.piece.seg->color : &sel.piece.seg->wind.topColor);
        else if(sel.piece.seg->type == S_WALL && sel.cursor.y == 4){
            if(keyPressed(SC_RETURN))
                textInputStart(sel.piece.seg->wall.path, 128, NULL);
            if(textInputEnded()){
                map->seg[S_WALL] = wallListTxtrCleanup(map->seg[S_WALL], sel.piece.seg);
                sel.piece.seg->wall.texture = wallListTxtrQryLoad(map->seg[S_WALL], sel.piece.seg->wall.path);
            }
        }else if(sel.piece.seg->type == S_WIND){
            if(sel.cursor.y == 5)
                sel.piece.seg->wind.height = editFloat(sel.piece.seg->wind.height);
            else if(sel.cursor.y == 6)
                sel.piece.seg->wind.top = editFloat(sel.piece.seg->wind.top);
        }else if(sel.piece.seg->type == S_DOOR){
            if(sel.cursor.y == 4)
                sel.piece.seg->door.id = editUint(sel.piece.seg->door.id);
            else if(sel.cursor.y == 5)
                sel.piece.seg->door.pos = editFloat(sel.piece.seg->door.pos);
            else if(sel.cursor.y == 7)
                sel.piece.seg->door.speed = editFloat(sel.piece.seg->door.speed);
        }else if(sel.piece.seg->type == S_TRIG){
            if(sel.cursor.y == 4)
                sel.piece.seg->trig.id = editUint(sel.piece.seg->trig.id);
        }else if(sel.piece.seg->type == S_CONV){
            if(sel.cursor.y == 4)
                sel.piece.seg->conv.idA = editUint(sel.piece.seg->conv.idA);
            else if(sel.cursor.y == 5)
                sel.piece.seg->conv.idB = editUint(sel.piece.seg->conv.idB);
        }
        updateMouseCommon(&ml, &mr, &sel, off, scale, snaplen);
        wlen = updateResize(wlen, &off);
        if(updateDel(map, &sel))
            printf("Deleted\n");
        checkScroll(&off, ml.mpos, snap, &snaplen, &scale);
        off = updatePan(off, &ml, &mr);
        ml = updateMouseL(ml, &sel, map, scale, snap, snaplen);
        mr = updateMouseR(mr, &sel, map, c, snap);
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
