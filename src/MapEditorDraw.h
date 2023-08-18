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

void editorDrawPiece(const MapPiece piece, const Offset off, const float scale, Coordf *selectedPos)
{
    const bool selected = pieceContainsCoord(piece, selectedPos);
    PieceCoords coords = pieceCoords(piece);
    setColor(pieceColor(piece));
    if(coords.numCoord >= 2){
        const Coord a = mapToScreen(off, scale, *coords.coord[0]);
        const Coord b = mapToScreen(off, scale, *coords.coord[1]);
        if(coords.numCoord == 4){
            const Coord c = mapToScreen(off, scale, *coords.coord[2]);
            const Coord d = mapToScreen(off, scale, *coords.coord[3]);
            if(selected){
                if(selectedPos == coords.coord[2])
                    fillCircleCoord(c, scale/4);
                else
                    drawCircleCoord(c, scale/4);
                if(selectedPos == coords.coord[3])
                    fillCircleCoord(d, scale/4);
                else
                    drawCircleCoord(d, scale/4);
            }
        }
        drawLineThickCoords(a, b, selected?4:1);
        if(selected){
            if(selectedPos == coords.coord[0])
                fillCircleCoord(a, scale/4);
            else
                drawCircleCoord(a, scale/4);
            if(selectedPos == coords.coord[1])
                fillCircleCoord(b, scale/4);
            else
                drawCircleCoord(b, scale/4);
        }
        return;
    }
    const Coord a = mapToScreen(off, scale, *coords.coord[0]);
    fillCircleCoord(a, scale/4);
}

void editorDrawMap(Map *map, const Offset off, const float scale, Coordf *selectedPos)
{
    MapPiece start = pieceNext(map, (MapPiece){.type = M_ANY});
    if(start.type >= M_ANY)
        return;
    MapPiece cur = start;
    do{
        editorDrawPiece(cur, off, scale, selectedPos);
        cur = pieceNext(map, cur);
    }while(!pieceSame(cur, start));
}

#endif /* end of include guard: MAPEDITORDRAW_H */
