#ifndef MAPEDITORDRAS_H
#define MAPEDITORDRAS_H

void drawOriginLines(const Offset off, const Length wlen)
{
    setColor(WHITE);
    if(inBound(off.y, 0, wlen.y))
        drawHLine(0, off.y, wlen.x);
    if(inBound(off.x, 0, wlen.x))
        drawVLine(off.x, 0, wlen.y);
}

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

void drawEditorMap(Seg *map, const Selection sel, const Offset off, const float scale)
{
    Seg *cur = map;
    while(cur){
        const Coord a = mapToScreen(off, scale, cur->a);
        const Coord b = mapToScreen(off, scale, cur->b);
        setColor(cur->color);
        drawLineThickCoords(a, b, (sel.pos == &cur->a || sel.pos == &cur->b) ? 3 : 1);
        cur = cur->next;
    }
}

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

Coord drawstr(const Coord pos, const char *str, const bool selected)
{
    setColor(BLACK);
    const Length len = getTextLength(str);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY);
    drawTextCoord(str, pos);
    return iC(0, pos.y+len.y);
}

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

Coord drawSelCommon(const Selection sel)
{
    if(!sel.wall)
        return iiC(0);
    Coord pos = drawSegType(iiC(0), sel.wall->type, coordSame(sel.cursor, iC(0,0)));
    pos = drawCoordf(pos, "a", sel.wall->a, coordSame(sel.cursor, iC(0,1)));
    pos = drawCoordf(pos, "b", sel.wall->b, coordSame(sel.cursor, iC(0,2)));
    return drawColor(pos, sel.tscale, sel.wall->color, sel.cursor.y == 3, sel.cursor.x);
}

void drawSelWind(const Selection sel, Coord pos)
{
    if(!sel.wall || sel.wall->type != S_WIND)
        return;
    pos = drawColor(pos, sel.tscale, sel.wall->wind.topColor, sel.cursor.y == 4, sel.cursor.x);
    pos = drawf(pos, "height", sel.wall->wind.height, sel.cursor.y == 5);
    drawf(pos, "top", sel.wall->wind.top, sel.cursor.y == 6);
}

void drawSelDoor(const Selection sel, Coord pos)
{
    if(!sel.wall || sel.wall->type != S_DOOR)
        return;
    pos = drawu(pos, "id", sel.wall->door.id, sel.cursor.y == 4);
    pos = drawf(pos, "pos: ", sel.wall->door.pos, sel.cursor.y == 5);
    char buf[32] = {0};
    sprintf(buf, "state: %s", sel.wall->door.state ? "true" : "false");
    pos = drawstr(pos, buf, sel.cursor.y == 6);
    pos = drawf(pos, "speed: ", sel.wall->door.speed, sel.cursor.y == 7);
    sprintf(buf, "closeDir: %c", DirectionChar[sel.wall->door.closeDir]);
    pos = drawstr(pos, buf, sel.cursor.y == 8);
}

void drawSelTrig(const Selection sel, Coord pos)
{
    if(!sel.wall || sel.wall->type != S_TRIG)
        return;
    pos = drawu(pos, "id", sel.wall->trig.id, sel.cursor.y == 4);
}

void drawSel(const Selection sel, const Offset off, const float scale)
{
    if(sel.pos)
        fillCircleCoord(mapToScreen(off, scale, *sel.pos), 8);
    if(!sel.wall){
        drawstr(iiC(0), SegTypeStr[sel.newSegType], sel.cursor.y == 0);
        return;
    }
    drawCircleCoord(mapToScreen(off, scale, &(sel.wall->a) == sel.pos ? sel.wall->b : sel.wall->a), 8);
    if(!sel.showInfo)
        return;
    if(sel.wall->type == S_TRIG){
        fillCircleCoord(mapToScreen(off, scale, sel.wall->trig.c), 4);
        fillCircleCoord(mapToScreen(off, scale, sel.wall->trig.d), 4);
        setColor(WHITE);
        drawCircleCoord(mapToScreen(off, scale, sel.wall->trig.c), 4);
        drawCircleCoord(mapToScreen(off, scale, sel.wall->trig.d), 4);
    }
    Coord pos = drawSelCommon(sel);
    if(sel.wall->type == S_WIND)
        drawSelWind(sel, pos);
    if(sel.wall->type == S_DOOR)
        drawSelDoor(sel, pos);
    if(sel.wall->type == S_TRIG)
        drawSelTrig(sel, pos);
}

#endif /* end of include guard: MAPEDITORDRAS_H */
