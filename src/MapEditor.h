#ifndef MAPEDITOR_H
#define MAPEDITOR_H

// initial struct data for editor state
EditorState editorInitState(void)
{
    return (EditorState){
        .pieceInfo = { .pieceType = M_SEG, .segType = S_WALL},
        .cam = { .scale = 1.0f, .wlen = getWindowLen() },
        .snap = { .active = true, .len = 50.0f }
    };
}

bool done(void)
{
    return keyState(SC_ESCAPE) || keyState(SC_RETURN);
}

void mapEditText(Map *map, EditorState *state, char *text)
{
    if(!text)
        return;
    bool once = false;
    while(1){
        const uint t = frameStart();

        if(once){
            if(!textInputState()){
                if(state->sel->fields.piece.type == M_SEG){
                    if(state->sel->fields.piece.seg->type == S_WALL){
                        textureFree(state->sel->fields.piece.seg->wall.texture);
                        state->sel->fields.piece.seg->wall.texture = loadTexture(state->sel->fields.piece.seg->wall.path);
                    }else{
                        panic("???");
                    }
                }else if(state->sel->fields.piece.type == M_OBJ){
                    if(state->sel->fields.piece.obj->type == O_MOB){
                        textureFree(state->sel->fields.piece.obj->mob.texture);
                        state->sel->fields.piece.obj->mob.texture = loadTexture(state->sel->fields.piece.obj->mob.path);
                    }else{
                        panic("???");
                    }
                }else{
                    panic("???");
                }
                // (state->sel->fields.field[state->sel->cursor->y])
                return;
            }

            editorDrawLines(state->snap, state->cam);
            editorDrawMap(map, state->cam.off, state->cam.scale, state->sel);
            editorDrawPieceFields(state->sel);
            editorDrawPieceCount(map, state->cam.wlen);
            editorDrawNewPieceType(state->pieceInfo, state->cam.wlen);
            editorDrawNewPiecePos(state->mouse.win);

            setColor(WHITE);
            fillCircleCoord(getWindowMid(), 8);
            fieldDraw(state->sel->fields.field[state->sel->cursor->y], getWindowMid(), 1);

        }else{
            if(!keyState(SC_RETURN)){
                once = true;
                textInputStart(text, 128, done);
            }
        }

        frameEnd(t);
    }
}

// main loop while in map editor. on save, map is saved to map->path
void mapEdit(Map *map)
{
    setRelativeMouse(false);
    EditorState state = editorInitState();
    fieldPrintMap(map);
    while(1){
        const uint t = frameStart();

        if(editorInputExit(map, state.sel))
            return;

        editorInputResizeWindow(&state.cam);
        editorInputClearSelection(&state.sel);
        editorInputSnap(&state.snap);
        editorInputMouseMove(state.cam, &state.mouse, &state.snap);
        editorInputMouseBtns(&state.mouse, &state.snap);
        editorInputSelect(map, state.mouse.map.pos, &state.cursor, &state.sel);
        editorInputNextSelection(map, state.sel);
        editorInputCursorState(&state.cursorState, &state.cursorNext, t);
        editorInputMoveCursor(state.sel);
        editorInputNewPiece(state.sel, &state.pieceInfo);
        editorInputZoom(&state.cam, state.mouse);
        editorInputPan(&state.cam.off);
        editorInputSave(map);

        editorUpdateDeleteSelection(map, &state.sel);
        editorUpdateMoveSelection(state.cam, state.snap, state.mouse.map, state.sel);
        editorUpdateNewPiece(map, state.pieceInfo, state.snap, state.mouse);
        if(keyPressed(SC_RETURN) && state.sel && state.sel->fields.field[state.sel->cursor->y].type == F_PATH)
            mapEditText(map, &state, state.sel->fields.field[state.sel->cursor->y].ptr);
        // mapEditText(map, &state, editorUpdateSelectedFields(state.sel));

        editorDrawLines(state.snap, state.cam);
        editorDrawMap(map, state.cam.off, state.cam.scale, state.sel);
        editorDrawPieceFields(state.sel);
        editorDrawPieceCount(map, state.cam.wlen);
        editorDrawNewPieceType(state.pieceInfo, state.cam.wlen);
        editorDrawNewPiecePos(state.mouse.win);

        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITOR_H */
