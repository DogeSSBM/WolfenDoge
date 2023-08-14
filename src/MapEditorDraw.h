#ifndef MAPEDITORDRAW_H
#define MAPEDITORDRAW_H

// draws X and Y axis lines if visible
void drawOriginLines(const Offset off, const Length wlen)
{
    setColor(WHITE);
    if(inBound(off.y, 0, wlen.y))
        drawHLine(0, off.y, wlen.x);
    if(inBound(off.x, 0, wlen.x))
        drawVLine(off.x, 0, wlen.y);
}

// if snap is enabled, draws grid lines with snaplen distance between each line
void drawGrid(const Offset off, const Length wlen, const float scale, const bool snap, const float snaplen)
{
    if(!snap)
        return;
    setColor(GREY);
    Coordf mpos = cfSnap(screenToMap(off, scale, iC(0,0)), snaplen);
    Coord spos = mapToScreen(off, scale, mpos);
    while(spos.x < wlen.x || spos.y < wlen.y){
        drawVLine(spos.x, 0, wlen.y);
        drawHLine(0, spos.y, wlen.x);
        mpos = cfAddf(mpos, snaplen);
        spos = mapToScreen(off, scale, mpos);
    }
}

// draws segment type text returns position of beginning of next line
Coord drawSegType(const Coord pos, const SegType type, const bool selected)
{
    char buf[32] = {0};
    sprintf(buf, "%s", SegTypeStr[type]);
    Length len = getTextLength(buf);
    setColor(BLACK);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY);
    drawTextCoord(buf, pos);
    return iC(0, pos.y+len.y);
}

// draws color text returns position of beginning of next line
Coord drawColor(Coord pos, const uint tscale, Color c, const bool selected, const int ci)
{
    setColor(BLACK);
    fillRectCoordLength(pos, iC(tscale*24+tscale/2, tscale));
    setColor(c);
    fillSquareCoord(pos, tscale);
    pos = coordShift(pos, DIR_R, tscale+tscale/2);
    char letter[3] = {'R', 'G', 'B'};
    Color indexcolor[3] = {RED, GREEN, BLUE};
    for(int i = 0; i < 3; i++){
        char buf[16] = {0};
        sprintf(buf, "%c: %3u", letter[i], *(colorIndex(&c, i)));
        setTextColor(selected && i == ci ? indexcolor[i] : GREY);
        drawTextCoord(buf, pos);
        pos.x += 8*tscale;
    }
    return iC(0, pos.y+tscale);
}

// draws label and Coordf text returns position of beginning of next line
Coord drawCoordf(const Coord pos, const char *label, const Coordf cf, const bool selected)
{
    char buf[64] = {0};
    sprintf(buf, "%s: %+14.6f, %+14.6f", label, cf.x, cf.y);
    const Length len = getTextLength(buf);
    setColor(BLACK);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY);
    drawTextCoord(buf, pos);
    return iC(0, pos.y+len.y);
}

// draws label and float text returns position of beginning of next line
Coord drawf(const Coord pos, const char *label, const float f, const bool selected)
{
    char buf[32] = {0};
    sprintf(buf, "%s: %+14.6f", label, f);
    const Length len = getTextLength(buf);
    setColor(BLACK);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY);
    drawTextCoord(buf, pos);
    return iC(0, pos.y+len.y);
}

// draws str text returns position of beginning of next line
Coord drawstr(const Coord pos, const char *str, const bool selected)
{
    setColor(BLACK);
    const Length len = getTextLength(str);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY);
    drawTextCoord(str, pos);
    return iC(0, pos.y+len.y);
}

// draws uint text returns position of beginning of next line
Coord drawu(const Coord pos, const char *label, const uint u, const bool selected)
{
    char buf[16] = {0};
    sprintf(buf, "%s: %3u", label, u);
    Length len = getTextLength(buf);
    setColor(BLACK);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY);
    drawTextCoord(buf, pos);
    return iC(0, pos.y+len.y);
}

// draws fields common to all segments when selected returns position of beginning of next line
Coord drawSelCommon(const Selection sel)
{
    if(!sel.piece.seg)
        return iiC(0);
    Coord pos = drawSegType(iiC(0), sel.piece.seg->type, coordSame(sel.cursor, iC(0,0)));
    pos = drawCoordf(pos, "a", sel.piece.seg->a, coordSame(sel.cursor, iC(0,1)));
    pos = drawCoordf(pos, "b", sel.piece.seg->b, coordSame(sel.cursor, iC(0,2)));
    return drawColor(pos, sel.tscale, sel.piece.seg->color, sel.cursor.y == 3, sel.cursor.x);
}

// draws wall segment fields when selected
void drawSelWall(const Selection sel, Coord pos)
{
    if(!sel.piece.seg || sel.piece.seg->type != S_WALL)
        return;
    pos = drawstr(pos, (sel.piece.seg->wall.path[0] == '\0') ? "       " : sel.piece.seg->wall.path, sel.cursor.y == 4);
}

// draws window segment fields when selected
void drawSelWind(const Selection sel, Coord pos)
{
    if(!sel.piece.seg || sel.piece.seg->type != S_WIND)
        return;
    pos = drawColor(pos, sel.tscale, sel.piece.seg->wind.topColor, sel.cursor.y == 4, sel.cursor.x);
    pos = drawf(pos, "height", sel.piece.seg->wind.height, sel.cursor.y == 5);
    drawf(pos, "top", sel.piece.seg->wind.top, sel.cursor.y == 6);
}

// draws door segment fields when selected
void drawSelDoor(const Selection sel, Coord pos)
{
    if(!sel.piece.seg || sel.piece.seg->type != S_DOOR)
        return;
    pos = drawu(pos, "id", sel.piece.seg->door.id, sel.cursor.y == 4);
    pos = drawf(pos, "pos: ", sel.piece.seg->door.pos, sel.cursor.y == 5);
    char buf[32] = {0};
    sprintf(buf, "state: %s", sel.piece.seg->door.state ? "true" : "false");
    pos = drawstr(pos, buf, sel.cursor.y == 6);
    pos = drawf(pos, "speed: ", sel.piece.seg->door.speed, sel.cursor.y == 7);
    sprintf(buf, "closeDir: %c", DirectionChar[sel.piece.seg->door.closeDir]);
    pos = drawstr(pos, buf, sel.cursor.y == 8);
}

// draws trigger segment fields when selected
void drawSelTrig(const Selection sel, Coord pos)
{
    if(!sel.piece.seg || sel.piece.seg->type != S_TRIG)
        return;
    pos = drawu(pos, "id", sel.piece.seg->trig.id, sel.cursor.y == 4);
}

// draws converter segment fields when selected
void drawSelConv(const Selection sel, Coord pos)
{
    if(!sel.piece.seg || sel.piece.seg->type != S_CONV)
        return;
    pos = drawu(pos, "idA", sel.piece.seg->conv.idA, sel.cursor.y == 4);
    pos = drawu(pos, "idB", sel.piece.seg->conv.idB, sel.cursor.y == 5);
}

// draws selected segment highlights
void drawSel(const Selection sel, const Offset off, const float scale)
{
    if(sel.pos)
        fillCircleCoord(mapToScreen(off, scale, *sel.pos), 8);
    if(!sel.piece.seg){
        drawstr(iiC(0), SegTypeStr[sel.newSegType], sel.cursor.y == 0);
        return;
    }
    drawCircleCoord(mapToScreen(off, scale, &(sel.piece.seg->a) == sel.pos ? sel.piece.seg->b : sel.piece.seg->a), 8);
    if(!sel.showInfo)
        return;
    if(sel.piece.seg->type == S_TRIG){
        fillCircleCoord(mapToScreen(off, scale, sel.piece.seg->trig.c), 4);
        fillCircleCoord(mapToScreen(off, scale, sel.piece.seg->trig.d), 4);
        setColor(WHITE);
        drawCircleCoord(mapToScreen(off, scale, sel.piece.seg->trig.c), 4);
        drawCircleCoord(mapToScreen(off, scale, sel.piece.seg->trig.d), 4);
    }
    Coord pos = drawSelCommon(sel);
    if(sel.piece.seg->type == S_WALL)
        drawSelWall(sel, pos);
    if(sel.piece.seg->type == S_WIND)
        drawSelWind(sel, pos);
    if(sel.piece.seg->type == S_DOOR)
        drawSelDoor(sel, pos);
    if(sel.piece.seg->type == S_TRIG)
        drawSelTrig(sel, pos);
    if(sel.piece.seg->type == S_CONV)
        drawSelConv(sel, pos);
}

// draws all map segments in the editor
void drawEditorMap(Map *map, const Selection sel, const Offset off, const float scale)
{
    for(SegType type = 0; type < S_N; type++){
        Seg *cur = map->seg[type];
        while(cur){
            const Coord a = mapToScreen(off, scale, cur->a);
            const Coord b = mapToScreen(off, scale, cur->b);
            if(cur->type == S_TRIG){
                const Coord c = mapToScreen(off, scale, cur->trig.c);
                const Coord d = mapToScreen(off, scale, cur->trig.d);
                setColor(YELLOW);
                drawLineCoords(a, c);
                drawLineCoords(a, d);
                drawLineCoords(b, c);
                drawLineCoords(b, d);
                drawLineCoords(c, d);
            }
            setColor(cur->color);
            drawLineThickCoords(a, b, (sel.pos == &cur->a || sel.pos == &cur->b) ? 3 : 1);
            cur = cur->next;
        }
    }
}

#endif /* end of include guard: MAPEDITORDRAW_H */
