#ifndef MAPEDITORUPDATE_H
#define MAPEDITORUPDATE_H

// updates left and right mouse button info
// clears selection wall / pos  and drag info if escape is pressed
void updateMouseCommon(Minfo *ml, Minfo *mr, Selection *sel, const Offset off, const float scale, const float snaplen)
{
    ml->spos = mouse.pos;
    mr->spos = ml->spos;
    ml->mpos = screenToMap(off, scale, ml->spos);
    mr->mpos = ml->mpos;
    ml->msnap = cfSnapMid(ml->mpos, snaplen);
    mr->msnap = ml->msnap;
    ml->ssnap = mapToScreen(off, scale, ml->msnap);
    mr->ssnap = ml->ssnap;
    if(!keyPressed(SC_ESCAPE))
        return;
    if(sel->piece.type == M_SEG)
        sel->piece.seg = NULL;
    else if(sel->piece.type == M_SEG)
        sel->piece.seg = NULL;
    sel->piece.type = M_NONE;
    sel->pos = NULL;
    mr->drag = false;
    ml->drag = false;
}

// updates left mouse button info
// updates selection if left mouse is pressed, dragged, or released
Minfo updateMouseL(Minfo ml, Selection *sel, Map *map, const float scale, const bool snap, const float snaplen)
{
    if(sel->piece.type == M_NONE && mouseBtnReleased(MOUSE_L))
        sel->piece = posNearest(map, ml.mpos, M_ANY, &(sel->pos));

    if(mouseBtnPressed(MOUSE_L) && sel->pos){
        ml.drag = true;
        ml.sposd = ml.spos;
        ml.ssnapd = ml.ssnap;
        ml.mposd = ml.mpos;
        ml.msnapd = ml.msnap;
        sel->posOrig = *(sel->pos);
        if(snap)
            sel->posOrig = cfSnapMid(*(sel->pos), snaplen);
    }

    if(ml.drag && mouseBtnReleased(MOUSE_L)){
        ml.drag = false;
    }

    if(ml.drag && sel->pos){
        if(snap)
            *(sel->pos) = cfAdd(sel->posOrig, cfSnapMid(cfSub(ml.mpos, ml.mposd), snaplen));
        else
            *(sel->pos) = cfAdd(*(sel->pos), cfMulf(CCf(mouseMovement()), scale));
    }

    return ml;
}

// updates right mouse button info
// updates selection if right mouse is pressed, dragged, or released
Minfo updateMouseR(Minfo mr, Selection *sel, Map *map, const Color c, const bool snap)
{
    if(mouseBtnPressed(MOUSE_R)){
        mr.sposd = mr.spos;
        mr.ssnapd = mr.ssnap;
        mr.mposd = mr.mpos;
        mr.msnapd = mr.msnap;
        mr.drag = true;
    }

    if(mr.drag && mouseBtnReleased(MOUSE_R)){
        mr.drag = false;
        Seg *newSeg = NULL;
        const Coordf a = snap ? mr.msnapd : mr.mposd;
        const Coordf b = snap ? mr.msnap : mr.mpos;
        switch(sel->newSegType){
            case S_WALL:
                newSeg = wallNew(c, a, b);
                break;
            case S_WIND:
                newSeg = windNew(c, c, a, b, .25f, .25f);
                break;
            case S_DOOR:
                newSeg = doorNew(c, a, b, 0, 0.0f, false, 0.01f, DIR_D);
                break;
            case S_TRIG:
                newSeg = trigNew(c, a, b, 0, cfAddf(a, 100.0f), cfAddf(b, 100.0f));
                break;
            case S_CONV:
                newSeg = convNew(c, a, b, 0, 0);
                break;
            default:
                panic("uh oh");
                break;
        }
        assertExpr(newSeg);
        map->seg[newSeg->type] = segAppend(map->seg[newSeg->type], newSeg);
        sel->piece.seg = newSeg;
        sel->pos = &(newSeg->a);
    }

    if(mr.drag){
        setColor(c);
        drawLineCoords(snap ? mr.ssnapd : mr.sposd, snap ? mr.ssnap : mr.spos);
        setColor(YELLOW);
        fillCircleCoord(snap ? mr.ssnap : mr.spos, 8);
        setColor(GREEN);
        fillCircleCoord(snap ? mr.ssnapd : mr.sposd, 8);
    }

    return mr;
}

// updates selection cursor.y if up or down arrow keys are pressed
// sets cursor.x to 0 if cursor.y changed to a selection that only has a single option
Selection updateSelCursor(Selection sel)
{
    if(!sel.piece.seg || textInputState())
        return sel;
    sel.cursor.y = wrap(sel.cursor.y + keyPressed(SC_DOWN) - keyPressed(SC_UP), 0, SegTypeNumFields[sel.piece.seg->type]);
    if(!(
        sel.cursor.y == 3 ||
        (sel.piece.seg->type == S_WIND && sel.cursor.y == 4) ||
        (sel.piece.seg->type == S_DOOR && sel.cursor.y == 8)
    ))
        sel.cursor.x = 0;
    return sel;
}

// updates selection to next seg / coord that overlaps
Selection updateSelNext(Selection sel, Map *map)
{
    if(!keyPressed(SC_N))
        return sel;
    sel.piece = posNext(map, sel.piece, &sel.pos);
    return sel;
}

// if a segment is selected, deletes it from map segment list and updates *sel
bool updateDel(Map *map, Selection *sel)
{
    if(!keyPressed(SC_DELETE))
        return false;
    if(sel->piece.type == M_SEG && sel->piece.seg){
        map->seg[sel->piece.seg->type] = segDelete(map->seg[sel->piece.seg->type], sel->piece.seg);
        sel->piece.type = M_NONE;
        sel->piece.seg = NULL;
        sel->pos = NULL;
        return true;
    }else if(sel->piece.type == M_OBJ && sel->piece.obj){
        map->obj[sel->piece.obj->type] = objDelete(map->obj[sel->piece.obj->type], sel->piece.obj);
        sel->piece.type = M_NONE;
        sel->piece.obj = NULL;
        sel->pos = NULL;
        return true;
    }
    return false;
}

// if window is resized updates it preserving off relative to new window lengths
// eg off will be in the same place in terms of percentage offset relative to old / new window lengths
Length updateResize(Length wlen, Offset *off)
{
    if(windowResized()){
        const Length wlenOld = wlen;
        wlen = getWindowLen();
        *off = resizeTransform(wlenOld, wlen, *off);
    }
    return wlen;
}

// pans map while middle mouse or left shift is held
Offset updatePan(Offset off, Minfo *ml, Minfo *mr)
{
    if(mouseBtnState(MOUSE_M) || keyState(SC_LSHIFT)){
        off = coordAdd(off, mouseMovement());
        mr->sposd = mr->spos;
        ml->sposd = ml->spos;
    }
    return off;
}

#endif /* end of include guard: MAPEDITORUPDATE_H */
