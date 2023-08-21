#ifndef MAPEDITORINPUT_H
#define MAPEDITORINPUT_H

// returns true if escape pressed with no active selection
bool editorInputExit(Map *map, Selection *sel)
{
    if(checkCtrlKey(SC_Q)){
        mapFree(map);
        exit(EXIT_SUCCESS);
    }
    return !sel && keyPressed(SC_ESCAPE);
}

// updates camera on window resize
void editorInputResizeWindow(Camera *cam)
{
    if(!coordSame(cam->prv.wlen, cam->wlen) || windowResized()){
        cam->prv.wlen = cam->wlen;
        cam->wlen = getWindowLen();
        cam->prv.mlen = cam->mlen;
        cam->mlen = cfMulf(CCf(cam->wlen), cam->scale);
        setTextSize((cam->wlen.y/3)/12);
    }
}

// if there is a selection, pressing escape will clear it
void editorInputClearSelection(Selection **sel)
{
    if(*sel && keyPressed(SC_ESCAPE))
        *sel = selFreeList(*sel);
}

// sets mouse map / win positions
void editorInputMouseMove(const Camera cam, Mouse *mouse, Snap *snap)
{
    mouse->map.prv.pos = mouse->map.pos;
    mouse->win.prv.pos = mouse->win.pos;
    mouse->win.pos = mousePos();
    mouse->map.pos = screenToMap(cam.off, cam.scale, mouse->win.pos);
    snap->mouse.pos = cfSnapMid(mouse->map.pos, snap->len);
    setColor(CYAN);
    fillCircleCoord(mapToScreen(cam.off, cam.scale, snap->mouse.pos), 4);
}

// sets ldown / rdown map / win positions on left / right click
void editorInputMouseBtns(Mouse *mouse, Snap *snap)
{
    if(mouseBtnPressed(MOUSE_L)){
        mouse->map.ldown = mouse->map.pos;
        mouse->win.ldown = mouse->win.pos;
        mouse->map.prv.ldown = mouse->map.ldown;
        mouse->win.prv.ldown = mouse->win.ldown;
        snap->mouse.prv.ldown = snap->mouse.ldown;
        snap->mouse.ldown = snap->mouse.pos;
    }
    if(mouseBtnPressed(MOUSE_R)){
        mouse->map.rdown = mouse->map.pos;
        mouse->win.rdown = mouse->win.pos;
        mouse->map.prv.rdown = mouse->map.rdown;
        mouse->win.prv.rdown = mouse->win.rdown;
        snap->mouse.prv.rdown = snap->mouse.rdown;
        snap->mouse.rdown = snap->mouse.pos;
    }
}

// performs single selection on left click when current selection inactive
// left click while holding ctrl for multiple selections
void editorInputSelect(Map *map, const Coordf pos, Coord *cursor, Selection **sel)
{
    assertExpr(map && sel && cursor);
    if(!mouseBtnPressed(MOUSE_L) || (*sel && !keyCtrlState()))
        return;
    Selection *addSel = selPosNearest(map, cursor, pos);
    if(!addSel)
        return;
    if(selPosSelected(*sel, addSel->pos)){
        const MapPiece pieceStart = addSel->fields.piece;
        Coordf *posStart = addSel->pos;
        MapPiece npiece = pieceStart;
        Coordf *npos = posStart;
        do{
            npiece = pieceNextSameCoord(map, npiece, &npos);
            addSel->pos = npos;
            if(!selPosSelected(*sel, addSel->pos)){
                *sel = selAppend(*sel, addSel);
                return;
            }
        }while(npos != posStart);
        selFree(addSel);
        return;
    }
    *sel = selAppend(*sel, addSel);
}

// changes most recent selection to next available map piece with same coords as current selected pos
void editorInputNextSelection(Map *map, Selection *sel)
{
    if(sel && keyPressed(SC_N)){
        sel = selLast(sel);
        MapPiece cur = sel->fields.piece;
        cur = pieceNextSameCoord(map, cur, &sel->pos);
        sel->fields = pieceFields(cur);
    }
}

// while selection is active, changes the field currently highlighted by the cursor
void editorInputMoveCursor(Selection *sel)
{
    if(sel && !sel->next){
        *sel->cursor = coordAdd(*sel->cursor, arrowKeyPressedOffset());
        sel->cursor->y = wrap(sel->cursor->y, 0, sel->fields.numFields);
        sel->cursor->x = wrap(sel->cursor->x, 0, FieldTypeXlen[sel->fields.field[sel->cursor->y].type]);
    }
}

// saves map when ctrl s is pressed
void editorInputSave(Map *map)
{
    if(checkCtrlKey(SC_S))
        mapSave(map);
}


// editorInputNewPiece(&state.pieceInfo)


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

#endif /* end of include guard: MAPEDITORINPUT_H */
