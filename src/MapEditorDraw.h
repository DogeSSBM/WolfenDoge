#ifndef MAPEDITORDRAW_H
#define MAPEDITORDRAW_H

// draws X and Y axis lines if visible
void editorDrawOriginLines(const Offset off, const Length wlen)
{
    for(int i = -1; i <= 1; i++){
        setColor(i == 0 ? WHITE : GREY);
        if(inBound(off.y+i, 0, wlen.y))
            drawHLine(0, off.y+i, wlen.x);
        if(inBound(off.x+i, 0, wlen.x))
            drawVLine(off.x+i, 0, wlen.y);
    }
}

// draws grid lines with snapLen distance between each line
void editorDrawGrid(const Offset off, const Length wlen, const float scale, const float snapLen)
{
    setColor(GREY);
    Coordf mpos = cfSnap(screenToMap(off, scale, iC(0,0)), snapLen);
    Coord spos = mapToScreen(off, scale, mpos);
    while(spos.x < wlen.x || spos.y < wlen.y){
        drawVLine(spos.x, 0, wlen.y);
        drawHLine(0, spos.y, wlen.x);
        mpos = cfAddf(mpos, snapLen);
        spos = mapToScreen(off, scale, mpos);
    }
}

void editorDrawPieceFields(Selection *sel)
{
    if(!sel)
        return;
    if(!sel->next){
        Coord pos = {0};
        for(int i = 0; i < (int)sel->fields.numFields; i++)
            pos = fieldDraw(sel->fields.field[i], pos, (sel->cursor->y == i) ? (sel->cursor->x)+1 : 0);
    }
}

void circleCoord(const Coord pos, const bool selected)
{
    const int radius = 8;
    if(selected)
        fillCircleCoord(pos, radius);
    else
        drawCircleCoord(pos, radius);
}

void editorDrawPiece(const MapPiece piece, const Offset off, const float scale, Selection *sel)
{
    const int thickness = 4;
    const bool selected = selPieceSelected(sel, piece);
    PieceCoords coords = pieceCoords(piece);
    setColor(pieceColor(piece));
    if(coords.numCoord >= 2){
        const Coord a = mapToScreen(off, scale, *coords.coord[0]);
        const Coord b = mapToScreen(off, scale, *coords.coord[1]);
        if(coords.numCoord == 4){
            const Coord c = mapToScreen(off, scale, *coords.coord[2]);
            const Coord d = mapToScreen(off, scale, *coords.coord[3]);
            if(selected){
                drawLineThickCoords(a, c, thickness);
                drawLineThickCoords(a, d, thickness);
                drawLineThickCoords(b, c, thickness);
                drawLineThickCoords(b, d, thickness);
                circleCoord(c, selPosSelected(sel, coords.coord[2]));
                circleCoord(d, selPosSelected(sel, coords.coord[3]));
            }
        }
        drawLineThickCoords(a, b, selected?thickness:1);
        if(selected){
            circleCoord(a, selPosSelected(sel, coords.coord[0]));
            circleCoord(b, selPosSelected(sel, coords.coord[1]));
        }
        return;
    }
    circleCoord(mapToScreen(off, scale, *coords.coord[0]), true);
}

void editorDrawPieceCount(Map *map, const Coord wlen)
{
    uint count = (uint)pieceCountTotal(map);
    const Field field = {.label = "Pieces: ", .type = F_UINT, .ptr = &count};
    char buf[64] = {0};
    sprintf(buf, "%s%3u", field.label, count);
    const Length len = getTextLength(buf);
    fieldDraw(field, coordSub(wlen, len), 0);
}

// draws x / y axies through origin and snap grid if enabled
void editorDrawLines(const Snap snap, const Camera cam)
{
    if(snap.active)
        editorDrawGrid(cam.off, cam.wlen, cam.scale, snap.len);
    editorDrawOriginLines(cam.off, cam.wlen);
}

// draws whole map
void editorDrawMap(Map *map, const Offset off, const float scale, Selection *sel)
{
    MapPiece start = pieceNext(map, (MapPiece){.type = M_ANY});
    if(start.type >= M_ANY)
        return;
    MapPiece cur = start;
    do{
        editorDrawPiece(cur, off, scale, sel);
        cur = pieceNext(map, cur);
    }while(!pieceSame(cur, start));
}

void editorDrawNewPieceType(const NewPieceInfo pieceInfo, const Length wlen)
{
    assertExpr(pieceInfo.pieceType < M_ANY);
    char qst[4] = "???";
    void *ptr = (void*)qst;
    FieldType type = F_PATH;
    if(pieceInfo.pieceType == M_SEG){
        type = F_SEGTYPE;
        ptr = (void*)&pieceInfo.segType;
    }
    if(pieceInfo.pieceType == M_OBJ){
        type = F_OBJTYPE;
        ptr = (void*)&pieceInfo.objType;
    }
    const Field field = {
        .label = "New Piece Type: ",
        .type = type,
        .ptr = ptr
    };
    char buf[64] = {0};
    sprintf(buf, "%s%s", field.label, pieceInfo.pieceType == M_SEG ? SegTypeStr[pieceInfo.segType] : ObjTypeStr[pieceInfo.objType]);
    const Length len = getTextLength(buf);
    fieldDraw(field, iC(wlen.x-len.x, 0), 0);
}

void editorDrawNewPiecePos(const MouseWin wmouse)
{
    if(!mouseBtnState(MOUSE_R))
        return;

    setColor(WHITE);
    drawLineThickCoords(wmouse.rdown, wmouse.pos, 4);
    drawCircleCoord(wmouse.rdown, 6);
    fillCircleCoord(wmouse.pos, 6);
}
#endif /* end of include guard: MAPEDITORDRAW_H */
