#ifndef MAPEDITORUPDATE_H
#define MAPEDITORUPDATE_H

void editorUpdateDeleteSelection(Map *map, Selection **sel)
{
    assertExpr(map && sel);
    if(!keyPressed(SC_DELETE))
        return;
    while(*sel){
        pieceDelete(map, (*sel)->fields.piece);
        *sel = selFree(*sel);
    }
}

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

void editorUpdateNewPiece(Map *map, const NewPieceInfo pieceInfo, const Snap snap, const Mouse mouse)
{
    if(mouseBtnReleased(MOUSE_R)){
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


#endif /* end of include guard: MAPEDITORUPDATE_H */
