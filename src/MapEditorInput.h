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

// toggles snap on or off
// on ctrl + scroll, changes snap len
void editorInputSnap(Snap *snap)
{
    const bool oldSnapActive = snap->active;
    const float oldSnapLen = snap->len;
    if(keyPressed(SC_S) && !keyCtrlState())
        snap->active = !snap->active;
    if(keyCtrlState() && mouseScrolledY()){
        snap->len = (float)imax(1, (int)snap->len + mouseScrolledY());
    }
    snap->len = (float)imax(1, (int)snap->len + checkCtrlKey(SC_EQUALS)-checkCtrlKey(SC_MINUS));
    if(snap->len != oldSnapLen || snap->active != oldSnapActive)
        printf("Snap %4.0f (%s)\n", snap->len, snap->active?"On":"Off");
}

// sets mouse map / win positions
void editorInputMouseMove(const Camera cam, Mouse *mouse, Snap *snap)
{
    mouse->map.prv.pos = mouse->map.pos;
    mouse->win.prv.pos = mouse->win.pos;
    mouse->win.pos = mousePos();
    mouse->map.pos = screenToMap(cam.off, cam.scale, mouse->win.pos);
    snap->mouse.map.prv.pos = snap->mouse.map.pos;
    snap->mouse.win.prv.pos = snap->mouse.win.pos;
    if(snap->active){
        snap->mouse.map.pos = cfSnapMid(mouse->map.pos, snap->len);
        snap->mouse.win.pos = mapToScreen(cam.off, cam.scale, snap->mouse.map.pos);
        mouse->map.pos = snap->mouse.map.pos;
        mouse->win.pos = snap->mouse.win.pos;
    }else{
        snap->mouse.map.pos = mouse->map.pos;
        snap->mouse.win.pos = mouse->win.pos;
    }
}

// sets ldown / rdown map / win positions on left / right click
void editorInputMouseBtns(Mouse *mouse, Snap *snap)
{
    if(mouseBtnPressed(MOUSE_L)){
        mouse->map.prv.ldown = mouse->map.ldown;
        mouse->win.prv.ldown = mouse->win.ldown;
        snap->mouse.map.prv.ldown = snap->mouse.map.ldown;
        snap->mouse.win.prv.ldown = snap->mouse.win.ldown;
        mouse->map.ldown = snap->active ? snap->mouse.map.pos : mouse->map.pos;
        mouse->win.ldown = snap->active ? snap->mouse.win.pos : mouse->win.pos;
        snap->mouse.map.ldown = snap->mouse.map.pos;
        snap->mouse.win.ldown = snap->mouse.win.pos;
    }
    if(mouseBtnPressed(MOUSE_R)){
        mouse->map.prv.rdown = mouse->map.rdown;
        mouse->win.prv.rdown = mouse->win.rdown;
        snap->mouse.map.prv.rdown = snap->mouse.map.rdown;
        snap->mouse.win.prv.rdown = snap->mouse.win.rdown;
        mouse->map.rdown = snap->active ? snap->mouse.map.pos : mouse->map.pos;
        mouse->win.rdown = snap->active ? snap->mouse.win.pos : mouse->win.pos;
        snap->mouse.map.rdown = snap->mouse.map.pos;
        snap->mouse.win.rdown = snap->mouse.win.pos;
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

// Changed the type of new piece
void editorInputNewPiece(Selection *sel, NewPieceInfo *pieceInfo)
{
    if(sel)
        return;
    assertExpr(pieceInfo->pieceType == M_SEG || pieceInfo->pieceType == M_OBJ);
    if(pieceInfo->pieceType == M_SEG){
        const int pt = pieceInfo->segType - keyPressed(SC_LEFT) + keyPressed(SC_RIGHT);
        if(pt < 0){
            pieceInfo->pieceType = M_OBJ;
            pieceInfo->objType = O_N + pt;
            return;
        }else if(pt >= S_N){
            pieceInfo->pieceType = M_OBJ;
            pieceInfo->objType = pt-S_N;
            return;
        }
        pieceInfo->segType = pt;
        return;
    }
    const int pt = pieceInfo->objType - keyPressed(SC_LEFT) + keyPressed(SC_RIGHT);
    if(pt < 0){
        pieceInfo->pieceType = M_SEG;
        pieceInfo->segType = S_N + pt;
        return;
    }else if(pt >= O_N){
        pieceInfo->pieceType = M_SEG;
        pieceInfo->segType = pt-O_N;
        return;
    }
    pieceInfo->objType = pt;
}

// zooms editor in or out focused on cursor
void editorInputZoom(Camera *cam, const Mouse mouse)
{
    if(!mouseScrolledY() || keyCtrlState())
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

// function that returns true when done editing current field
bool done(void)
{
    return keyState(SC_ESCAPE) || keyState(SC_RETURN);
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

// main loop while editing a text field
void mapEditText(Map *map, EditorState *state, char *text)
{
    if(!text || !state->sel)
        return;
    bool once = false;
    while(1){
        const uint t = frameStart();

        if(once){
            if(!textInputState()){
                if(state->sel->fields.piece.type == M_SEG){
                    if(state->sel->fields.piece.seg->type == S_WALL){
                        printf("Loading wall texture: \"%s\"\n", state->sel->fields.piece.seg->wall.path);
                        textureFree(state->sel->fields.piece.seg->wall.texture);
                        state->sel->fields.piece.seg->wall.texture = loadTexture(state->sel->fields.piece.seg->wall.path);
                    }else{
                        panic("???");
                    }
                }else if(state->sel->fields.piece.type == M_OBJ){
                    if(state->sel->fields.piece.obj->type == O_MOB){
                        printf("Loading mob texture: \"%s\"\n", state->sel->fields.piece.obj->mob.path);
                        textureFree(state->sel->fields.piece.obj->mob.texture);
                        state->sel->fields.piece.obj->mob.texture = loadTexture(state->sel->fields.piece.obj->mob.path);
                    }else{
                        panic("???");
                    }
                }else{
                    panic("???");
                }
                return;
            }
        }else{
            if(!keyState(SC_RETURN)){
                once = true;
                textInputStart(text, 128, done);
            }
        }

        editorDrawLines(state->snap, state->cam);
        editorDrawMap(map, state->cam.off, state->cam.scale, state->sel);
        editorDrawPieceFields(state->sel);
        editorDrawPieceCount(map, state->cam.wlen, state->sel);
        editorDrawNewPieceType(state->pieceInfo, state->cam.wlen);
        editorDrawNewPiecePos(state->mouse.win);

        frameEnd(t);
    }
}

// main loop while editing a text field
void mapEditFloat(Map *map, EditorState *state, float *f)
{
    if(!f || !state->sel)
        return;
    bool once = false;
    ull u = (ull)(*f * 1000.0f);
    while(1){
        const uint t = frameStart();

        if(once){
            if(keyPressed(SC_RETURN) || keyPressed(SC_ESCAPE))
                return;
            if(keyPressed(SC_BACKSPACE)){
                if(keyCtrlState()){
                    u = 0;
                }else{
                    u /= 10;
                }
            }
            int i = -1;
            if((i = numKeyPressed()) != -1){
                u = u*10+i;
                if(u/1000 > 1000)
                    u = 1000*1000;
            }
            *f = (float)u/1000.0f;
        }else{
            if(!keyState(SC_RETURN)){
                once = true;
            }
        }

        editorDrawLines(state->snap, state->cam);
        editorDrawMap(map, state->cam.off, state->cam.scale, state->sel);
        editorDrawPieceFields(state->sel);
        editorDrawPieceCount(map, state->cam.wlen, state->sel);
        editorDrawNewPieceType(state->pieceInfo, state->cam.wlen);
        editorDrawNewPiecePos(state->mouse.win);

        frameEnd(t);
    }
}

// main loop while editing a uint field
void mapEditUint(Map *map, EditorState *state, uint *u)
{
    if(!u || !state->sel)
        return;
    bool once = false;
    const uint old = *u;
    while(1){
        const uint t = frameStart();

        if(once){
            if(keyPressed(SC_RETURN) || keyPressed(SC_ESCAPE)){
                if(
                    state->sel->fields.piece.type == M_OBJ &&
                    state->sel->fields.piece.obj->type == O_CONV
                    // hasCycle(map, state->sel->fields.piece->obj)
                )
                    *u = old;
                return;
            }
            if(keyPressed(SC_BACKSPACE)){
                if(keyCtrlState()){
                    *u = 0;
                }else{
                    *u /= 10;
                }
            }
            int i = -1;
            if((i = numKeyPressed()) != -1){
                *u = *u*10+i;
            }
        }else{
            if(!keyState(SC_RETURN)){
                once = true;
            }
        }

        editorDrawLines(state->snap, state->cam);
        editorDrawMap(map, state->cam.off, state->cam.scale, state->sel);
        editorDrawPieceFields(state->sel);
        editorDrawPieceCount(map, state->cam.wlen, state->sel);
        editorDrawNewPieceType(state->pieceInfo, state->cam.wlen);
        editorDrawNewPiecePos(state->mouse.win);

        frameEnd(t);
    }
}

// main loop while editing a u8 field
void mapEditU8(Map *map, EditorState *state, u8 *u)
{
    if(!u || !state->sel)
        return;
    bool once = false;
    while(1){
        const uint t = frameStart();

        if(once){
            if(keyPressed(SC_RETURN) || keyPressed(SC_ESCAPE))
                return;
            if(keyPressed(SC_BACKSPACE)){
                if(keyCtrlState()){
                    *u = 0;
                }else{
                    *u /= 10;
                }
            }
            int i = -1;
            if((i = numKeyPressed()) != -1){
                uint ui = *u;
                ui = ui*10+i;
                if(ui > 255)
                    ui = 255;
                *u = ui;
            }
        }else{
            if(!keyState(SC_RETURN)){
                once = true;
            }
        }

        editorDrawLines(state->snap, state->cam);
        editorDrawMap(map, state->cam.off, state->cam.scale, state->sel);
        editorDrawPieceFields(state->sel);
        editorDrawPieceCount(map, state->cam.wlen, state->sel);
        editorDrawNewPieceType(state->pieceInfo, state->cam.wlen);
        editorDrawNewPiecePos(state->mouse.win);

        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITORINPUT_H */
