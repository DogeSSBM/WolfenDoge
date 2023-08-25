#ifndef MAPEDITOR_H
#define MAPEDITOR_H

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
        editorInputMoveCursor(state.sel);
        editorInputNewPiece(state.sel, &state.pieceInfo);
        editorInputZoom(&state.cam, state.mouse);
        editorInputPan(&state.cam.off);
        editorInputSave(map);

        editorUpdateDeleteSelection(map, &state.sel);
        editorUpdateMoveSelection(state.cam, state.snap, state.mouse.map, state.sel);
        editorUpdateNewPiece(map, state.pieceInfo, state.snap, state.mouse);
        editorUpdateBoxSelect(map, &state.cursor, state.mouse, &state.sel);
        editorUpdateSelectionVal(map, &state);
        editorUpdateSelectionDup(map, state.mouse.map.pos, &state.sel);

        editorDrawLines(state.snap, state.cam);
        editorDrawMap(map, state.cam.off, state.cam.scale, state.sel);
        editorDrawPieceFields(state.sel);
        editorDrawPieceCount(map, state.cam.wlen, state.sel);
        editorDrawNewPieceType(state.pieceInfo, state.cam.wlen);
        editorDrawNewPiecePos(state.mouse.win);

        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITOR_H */
