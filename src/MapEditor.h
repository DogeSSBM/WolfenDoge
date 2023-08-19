#ifndef MAPEDITOR_H
#define MAPEDITOR_H

// initial struct data for editor state
EditorState editorInitState(void)
{
    return (EditorState){
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
        editorInputMouseMove(state.cam, &state.mouse);
        editorInputMouseBtns(&state.mouse);
        editorInputSelect(map, state.mouse.map.pos, &state.cursor, &state.sel);
        editorInputNextSelection(map, state.sel);
        editorInputSingleSelMoveCursor(state.sel);
        editorInputZoom(&state.cam, state.mouse);
        editorInputPan(&state.cam.off);
        editorInputSave(map);

        editorDrawLines(state.snap, state.cam);
        editorDrawMap(map, state.cam.off, state.cam.scale, state.sel);
        editorDrawPieceFields(state.sel);
        editorDrawPieceCount(map, state.cam.wlen);
        // editorDrawNewPieceType();

        editorUpdateDeleteSelection(map, &state.sel);

        frameEnd(t);
    }
}

#endif /* end of include guard: MAPEDITOR_H */
