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
        editorInputMouseMove(state.cam, &state.mouse, &state.snap);
        editorInputMouseBtns(&state.mouse, &state.snap);
        editorInputSelect(map, state.mouse.map.pos, &state.cursor, &state.sel);
        editorInputNextSelection(map, state.sel);
        editorInputMoveCursor(state.sel);
        // editorInputNewPiece(&state.pieceInfo);
        editorInputZoom(&state.cam, state.mouse);
        editorInputPan(&state.cam.off);
        editorInputSave(map);

        editorDrawLines(state.snap, state.cam);
        editorDrawMap(map, state.cam.off, state.cam.scale, state.sel);
        editorDrawPieceFields(state.sel);
        editorDrawPieceCount(map, state.cam.wlen);
        editorDrawNewPieceType(state.pieceInfo, state.cam.wlen);

        editorUpdateDeleteSelection(map, &state.sel);
        editorUpdateMoveSelection(state.cam, state.snap, state.mouse.map, state.sel);
        // editorUpdateNewPiece()
        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITOR_H */
