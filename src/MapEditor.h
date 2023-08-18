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

// pans the map offset
Offset editorInputPan(const Offset off)
{
    if(mouseBtnState(MOUSE_M) || keyState(SC_LSHIFT))
        return coordAdd(off, mouseMovement());
    return off;
}

// zooms editor in or out focused on cursor
// returns true if zoom occured
bool editorInputZoom(float *scale, float *oldScale, Coord *off, const Coordf mouseMapPos)
{
    if(!mouseScrolledY())
        return false;
    *oldScale = *scale;
    *scale = fclamp(*scale * (mouseScrolledY() > 0 ? 1.2f : .8f) , .01f, 100.0f);
    if(*oldScale != *scale){
        const Coord ompos = mapToScreen(*off, *scale, mouseMapPos);
        const Coord diff = coordSub(mouse.pos, ompos);
        *off = coordAdd(*off, diff);
        return true;
    }
    return false;
}

// sets left mouse button press window / map location
// returns true on left mouse button press
bool editorInputLmouse(const Coord off, const float scale, Coord *ldown, Coordf *ldownf)
{
    if(mouseBtnPressed(MOUSE_L)){
        *ldown = mouse.pos;
        *ldownf = screenToMap(off, scale, mouse.pos);
        return true;
    }
    return false;
}

// main loop while in map editor. on save, map is saved to map->path
void mapEdit(Map *map)
{
    setRelativeMouse(false);
    EditorState state = {
        .scale = 1.0f,
        .wlen = getWindowLen(),
        .snap = { .enabled = true, .len = 50.0f },
        .mouse = { .win = {.pos = mouse.pos} }
    };
    mapPrintFields(map);
    while(1){
        const uint t = frameStart();

        if(checkCtrlKey(SC_Q))
            exit(EXIT_SUCCESS);

        state.mouse.win.pos = mouse.pos;
        state.mouse.map.pos = screenToMap(state.off, state.scale, state.mouse.win.pos);

        if(windowResized()){
            state.prv.wlen = state.wlen;
            state.wlen = getWindowLen();
            state.prv.mlen = state.mlen;
            state.mlen = cfMulf(CCf(state.wlen), state.scale);
        }
        if(state.selection && keyPressed(SC_ESCAPE)){
            state.selection = false;
            state.selectedPos = NULL;
        }
        editorInputLmouse(state.off, state.scale, &state.mouse.win.ldown, &state.mouse.map.ldown);
        if(!state.selection && mouseBtnReleased(MOUSE_L)){
            state.selection = true;
            state.fields = pieceFields(pieceNearest(map, state.mouse.map.pos, &state.selectedPos));
        }
        if(state.selection && keyPressed(SC_N)){
            MapPiece cur = state.fields.piece;
            cur = pieceNextSameCoord(map, cur, &state.selectedPos);
            state.fields = pieceFields(cur);
        }
        if(state.selection){
            state.cursor = coordAdd(state.cursor, arrowKeyPressedOffset());
            state.cursor.y = wrap(state.cursor.y, 0, state.fields.numFields);
            state.cursor.x = wrap(state.cursor.x, 0, FieldTypeXlen[state.fields.field[state.cursor.y].type]);
        }

        editorInputZoom(&state.scale, &state.prv.scale, &state.off, state.mouse.map.pos);
        state.off = editorInputPan(state.off);
        if(state.snap.enabled)
            editorDrawGrid(state.off, state.wlen, state.scale, state.snap.len);
        editorDrawOriginLines(state.off, state.wlen);
        setTextSize((getWindowLen().y/3)/12);
        editorDrawMap(map, state.off, state.scale, state.selectedPos);
        if(state.selection)
            drawPieceFields(state.fields, state.cursor, iiC(0));

        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITOR_H */
