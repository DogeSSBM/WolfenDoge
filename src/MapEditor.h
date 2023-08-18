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

// updates camera on window resize
void editorInputResizeWindow(Camera *cam)
{
    if(windowResized()){
        cam->prv.wlen = cam->wlen;
        cam->wlen = getWindowLen();
        cam->prv.mlen = cam->mlen;
        cam->mlen = cfMulf(CCf(cam->wlen), cam->scale);
    }
}

// if there is a selection, pressing escape will clear it
void editorInputClearSelection(Selection *sel)
{
    if(sel->active && keyPressed(SC_ESCAPE)){
        sel->active = false;
        sel->pos = NULL;
    }
}

// sets mouse map / win positions
void editorInputMouseMove(const Camera cam, Mouse *mouse)
{
    mouse->win.pos = mousePos();
    mouse->map.pos = screenToMap(cam.off, cam.scale, mousePos());
}

// sets ldown / rdown map / win positions on left / right click
void editorInputMouseBtns(Mouse *mouse)
{
    if(mouseBtnPressed(MOUSE_L)){
        mouse->win.ldown = mouse->win.pos;
        mouse->map.ldown = mouse->map.pos;
    }
    if(mouseBtnPressed(MOUSE_R)){
        mouse->win.rdown = mouse->win.pos;
        mouse->map.rdown = mouse->map.pos;
    }
}

// performs selection on left click when current selection inactive
void editorInputSelect(Map *map, const Mouse mouse, Selection *sel)
{
    if(!sel->active && mouseBtnPressed(MOUSE_L)){
        const MapPiece piece = pieceNearest(map, mouse.map.pos, &sel->pos);
        sel->active = piece.type < M_ANY;
        if(sel->active)
            sel->fields = pieceFields(piece);
    }
}

// selects next available map piece with same coords as current selected pos
void editorInputNextSelection(Map *map, Selection *sel)
{
    if(sel->active && keyPressed(SC_N)){
        MapPiece cur = sel->fields.piece;
        cur = pieceNextSameCoord(map, cur, &sel->pos);
        sel->fields = pieceFields(cur);
    }
}

// while selection is active, changes the field currently highlighted by the cursor
void editorInputMoveCursor(Selection *sel)
{
    if(sel->active){
        sel->cursor = coordAdd(sel->cursor, arrowKeyPressedOffset());
        sel->cursor.y = wrap(sel->cursor.y, 0, sel->fields.numFields);
        sel->cursor.x = wrap(sel->cursor.x, 0, FieldTypeXlen[sel->fields.field[sel->cursor.y].type]);
    }
}

// zooms editor in or out focused on cursor
void editorInputZoom(Camera *cam, const Mouse mouse)
{
    if(!mouseScrolledY())
        return;
    cam->prv.scale = cam->scale;
    cam->scale = fclamp(cam->scale * (mouseScrolledY() > 0 ? 1.2f : .8f) , .01f, 100.0f);
    if(cam->prv.scale != cam->scale){
        const Coord ompos = mapToScreen(cam->off, cam->scale, mouse.map.pos);
        cam->off = coordAdd(cam->off, coordSub(mouse.win.pos, ompos));
    }
}

// pans the map on mouse movement while holding middle mouse / left shift
void editorInputPan(Offset *off)
{
    if(mouseBtnState(MOUSE_M) || keyState(SC_LSHIFT))
        *off = coordAdd(*off, mouseMovement());
}

// draws x / y axies through origin and snap grid if enabled
void editorDrawLines(const Snap snap, const Camera cam)
{
    if(snap.active)
        editorDrawGrid(cam.off, cam.wlen, cam.scale, snap.len);
    editorDrawOriginLines(cam.off, cam.wlen);
}

// main loop while in map editor. on save, map is saved to map->path
void mapEdit(Map *map)
{
    setRelativeMouse(false);
    EditorState state = {
        .cam = { .scale = 1.0f, .wlen = getWindowLen() },
        .snap = { .active = true, .len = 50.0f }
    };
    mapPrintFields(map);
    while(1){
        const uint t = frameStart();

        if(checkCtrlKey(SC_Q))
            exit(EXIT_SUCCESS);

        editorInputResizeWindow(&state.cam);
        editorInputClearSelection(&state.sel);
        editorInputMouseMove(state.cam, &state.mouse);
        editorInputMouseBtns(&state.mouse);
        editorInputSelect(map, state.mouse, &state.sel);
        editorInputNextSelection(map, &state.sel);
        editorInputMoveCursor(&state.sel);
        editorInputZoom(&state.cam, state.mouse);
        editorInputPan(&state.cam.off);

        editorDrawLines(state.snap, state.cam);
        editorDrawMap(map, state.cam.off, state.cam.scale, state.sel.pos);
        editorDrawPieceFields(state.sel);

        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITOR_H */
