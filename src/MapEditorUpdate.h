#ifndef MAPEDITORUPDATE_H
#define MAPEDITORUPDATE_H

// deleted all selected pieces
void editorUpdateDeleteSelection(Map *map, Selection **sel)
{
    assertExpr(map && sel);
    if(!keyPressed(SC_DELETE))
        return;
    while(*sel){
        if(!selPieceSelected((*sel)->next, (*sel)->fields.piece))
            pieceDelete(map, (*sel)->fields.piece);
        *sel = selFree(*sel);
    }
}

// moves all selected coords
void editorUpdateMoveSelection(const Camera cam, const Snap snap, const MouseMap mmouse, Selection *sel)
{
    (void)cam;
    if(!sel)
        return;
    if(mouseBtnReleased(MOUSE_L)){
        while(sel){
            sel->moved = false;
            sel = sel->next;
        }
    }
    if(!mouseBtnState(MOUSE_L) || !mouseMoving())
        return;
    while(sel){
        if(sel->pos){
            if(!sel->moved){
                sel->holp = *sel->pos;
                sel->moved = true;
            }
            if(snap.active)
                *sel->pos = cfSnapMid(cfAdd(sel->holp, cfSub(mmouse.pos, mmouse.ldown)), snap.len);
            else
                *sel->pos = cfAdd(sel->holp, cfSub(mmouse.pos, mmouse.ldown));
        }
        sel = sel->next;
    }
}

// creates a new map piece once right mouse button is released
void editorUpdateNewPiece(Map *map, const NewPieceInfo pieceInfo, const Snap snap, const Mouse mouse)
{
    if(!keyCtrlState() && mouseBtnReleased(MOUSE_R)){
        MapPiece piece = {0};
        if(snap.active)
            piece = pieceNew(pieceInfo, mouse.map.rdown, mouse.map.pos);
        else
            piece = pieceNew(pieceInfo, snap.mouse.map.rdown, snap.mouse.map.pos);
        if(piece.type == M_SEG)
            map->seg[piece.seg->type] = segAppend(map->seg[piece.seg->type], piece.seg);
        else if(piece.type == M_OBJ)
            map->obj[piece.obj->type] = objAppend(map->obj[piece.obj->type], piece.obj);
        else
            panic("???");
    }
}

// selects the correct edit function given the cursor position and its corrosponding field
void editorUpdateSelectionVal(Map *map, EditorState *state)
{
    if(!state->sel || state->sel->next)
        return;
    if(keyPressed(SC_RETURN)){
        switch(state->sel->fields.field[state->sel->cursor->y].type){
            case F_PATH:
                mapEditText(map, state, state->sel->fields.field[state->sel->cursor->y].ptr);
                return;
                break;
            case F_FLOAT:
                mapEditFloat(map, state, state->sel->fields.field[state->sel->cursor->y].ptr);
                return;
                break;
            case F_COORDF:
                if(state->sel->cursor->x == 0)
                    mapEditFloat(map, state, &(((Coordf*)state->sel->fields.field[state->sel->cursor->y].ptr)->x));
                else
                    mapEditFloat(map, state, &(((Coordf*)state->sel->fields.field[state->sel->cursor->y].ptr)->y));
                return;
                break;
            case F_UINT:
                mapEditUint(map, state, state->sel->fields.field[state->sel->cursor->y].ptr);
                return;
                break;
            case F_COLOR:
                switch(state->sel->cursor->x){
                    case 0:
                        mapEditU8(map, state, &(((Color*)(state->sel->fields.field[state->sel->cursor->y].ptr))->r));
                        return;
                        break;
                    case 1:
                        mapEditU8(map, state, &(((Color*)(state->sel->fields.field[state->sel->cursor->y].ptr))->g));
                        return;
                        break;
                    case 2:
                        mapEditU8(map, state, &(((Color*)(state->sel->fields.field[state->sel->cursor->y].ptr))->b));
                        return;
                        break;
                    default:
                        panic("???");
                }
                return;
                break;
            default:
                return;
                break;
        }
    }
    switch(state->sel->fields.field[state->sel->cursor->y].type){
        case F_DIR:
            *((Direction*)(state->sel->fields.field[state->sel->cursor->y].ptr)) = wrap(
                *((Direction*)(state->sel->fields.field[state->sel->cursor->y].ptr)) + keyPressed(SC_LEFT) - keyPressed(SC_RIGHT),
                0, 4
            );
            return;
            break;
        case F_CONVTYPE:
            *((ConvType*)(state->sel->fields.field[state->sel->cursor->y].ptr)) = wrap(
                *((ConvType*)(state->sel->fields.field[state->sel->cursor->y].ptr)) + keyPressed(SC_LEFT) - keyPressed(SC_RIGHT),
                0, C_N
            );
            return;
            break;
        case F_TRIGTYPE:
            *((TrigType*)(state->sel->fields.field[state->sel->cursor->y].ptr)) = wrap(
                *((TrigType*)(state->sel->fields.field[state->sel->cursor->y].ptr)) + keyPressed(SC_LEFT) - keyPressed(SC_RIGHT),
                0, T_N
            );
            return;
            break;
        case F_BOOL:
            *((bool*)(state->sel->fields.field[state->sel->cursor->y].ptr)) = wrap(
                *((bool*)(state->sel->fields.field[state->sel->cursor->y].ptr)) + keyPressed(SC_LEFT) - keyPressed(SC_RIGHT),
                0, 2
            );
            return;
            break;
        default:
            return;
            break;
    }
}

// adds all coords within selection box to selection
void editorUpdateBoxSelect(Map *map, Coord *cursor, const Mouse mouse, Selection **list)
{
    assertExpr(list);
    if(!keyCtrlState() || !mouseBtnReleased(MOUSE_R))
        return;
    MapPiece start = pieceNext(map, (MapPiece){.type = M_ANY});
    if(start.type >= M_ANY)
        return;

    const Coordf min = cfLeast(mouse.map.rdown, mouse.map.pos);
    const Coordf max = cfMost(mouse.map.rdown, mouse.map.pos);

    MapPiece cur = start;
    do{
        PieceCoords coords = pieceCoords(cur);
        for(st i = 0; i < coords.numCoord; i++){
            if(cfInBounds(*coords.coord[i], min, max)){
                *list = selAppend(*list, selNew(cursor, coords.coord[i], pieceFields(cur)));
            }
        }
        cur = pieceNext(map, cur);
    }while(!pieceSame(cur, start));
}

// creates a copy of all selected pieces, deselects whole selection list
// and selects the copies
void editorUpdateSelectionDup(Map *map, const Coordf pos, Selection **list)
{
    (void)pos;
    assertExpr(map);
    if(!list || !keyPressed(SC_V))
        return;
    selListDupAddUniquePieces(map, *list);
    *list = selListAddAllPiecePos(*list);
}

#endif /* end of include guard: MAPEDITORUPDATE_H */
