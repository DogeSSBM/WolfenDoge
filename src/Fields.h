#ifndef FIELDS_H
#define FIELDS_H

FieldType fieldTypeFromMapPieceType(const MapPieceType ptype)
{
    assertExpr(ptype < M_ANY);
    return ptype == M_SEG ? F_SEGTYPE : F_OBJTYPE;
}

void fieldPrint(const MapPiece piece)
{
    assertExpr(piece.type < M_ANY);
    PieceFields fields = pieceFields(piece);
    for(st i = 0; i < fields.numFields; i++){
        // printf("printing type \"%s\"\n", FieldTypeStr[fields.field[i].type]);
        switch(fields.field[i].type){
            case F_MAPPIECETYPE:;
                MapPieceType val0 = *((MapPieceType *)(fields.field[i].ptr));
                printf("%s%s\n", fields.field[i].label, MapPieceTypeStr[val0]);
                break;
            case F_SEGTYPE:;
                SegType val1 = *((SegType *)(fields.field[i].ptr));
                printf("%s%s\n", fields.field[i].label, SegTypeStr[val1]);
                break;
            case F_OBJTYPE:;
                ObjType val2 = *((ObjType *)(fields.field[i].ptr));
                printf("%s%s\n", fields.field[i].label, ObjTypeStr[val2]);
                break;
            case F_COORDF:;
                Coordf val3 = *((Coordf *)(fields.field[i].ptr));
                printf("%s(%+10.3f,%+10.3f)\n", fields.field[i].label, val3.x, val3.y);
                break;
            case F_COLOR:;
                Color val4 = *((Color *)(fields.field[i].ptr));
                printf("%s(%3u,%3u,%3u)\n", fields.field[i].label, val4.r, val4.g, val4.b);
                break;
            case F_PATH:;
                char *empty = "NULL";
                char *val5 = fields.field[i].ptr ? fields.field[i].ptr : empty;
                printf("%s\"%s\"\n", fields.field[i].label, val5);
                break;
            case F_FLOAT:;
                float val6 = *((float *)(fields.field[i].ptr));
                printf("%s%+10.3f\n", fields.field[i].label, val6);
                break;
            case F_UINT:;
                uint val7 = *((uint *)(fields.field[i].ptr));
                printf("%s%3u\n", fields.field[i].label, val7);
                break;
            case F_BOOL:;
                bool val8 = *((bool *)(fields.field[i].ptr));
                printf("%s%s\n", fields.field[i].label, val8?"true":"false");
                break;
            case F_DIR:;
                Direction val9 = *((Direction *)(fields.field[i].ptr));
                printf("%s%c\n", fields.field[i].label, DirectionChar[val9]);
                break;
            default:
                panic("Unknown field type: %u", fields.field[i].type);
                break;
        }
    }
    printf("\n");
}

Coord fieldDrawMapPieceType(const MapPieceType val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    sprintf(buf, "%s%s", label, MapPieceTypeStr[val]);
    Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y += len.y;
    return pos;
}

Coord fieldDrawSegType(const SegType val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    sprintf(buf, "%s%s", label, SegTypeStr[val]);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y += len.y;
    return pos;
}

Coord fieldDrawObjType(const ObjType val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    sprintf(buf, "%s%s", label, ObjTypeStr[val]);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y += len.y;
    return pos;
}

Coord fieldDrawCoordf(const Coordf val, char *label, Coord pos, const uint selected)
{
    const Coord origin = pos;
    char buf[256] = {0};
    sprintf(buf, "%s(%+10.3f,%+10.3f)", label, val.x, val.y);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);

    setTextColor(GREY1);
    sprintf(buf, "%s(", label);
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected == 1 ? WHITE : GREY1);
    sprintf(buf, "%+10.3f", val.x);
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(GREY1);
    sprintf(buf, ",");
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected == 2 ? WHITE : GREY1);
    sprintf(buf, "%+10.3f", val.y);
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(GREY1);
    sprintf(buf, ")");
    drawTextCoord(buf, pos);
    pos.x = origin.x;
    pos.y += getTextYLen(buf);
    return pos;
}

Coord fieldDrawColor(const Color val, char *label, Coord pos, const uint selected)
{
    const Coord origin = pos;
    char buf[256] = {0};
    sprintf(buf, "%s(%3u,%3u,%3u)", label, val.r, val.g, val.b);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);

    setTextColor(selected ? WHITE : GREY1);
    sprintf(buf, "%s(", label);
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected == 1 ? RED : GREY1);
    sprintf(buf, "%3u", val.r);
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected ? WHITE : GREY1);
    sprintf(buf, ",");
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected == 2 ? GREEN : GREY1);
    sprintf(buf, "%3u", val.g);
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected ? WHITE : GREY1);
    sprintf(buf, ",");
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected == 3 ? BLUE : GREY1);
    sprintf(buf, "%3u", val.b);
    drawTextCoord(buf, pos);
    pos.x += getTextXLen(buf);

    setTextColor(selected ? WHITE : GREY1);
    sprintf(buf, ")");
    drawTextCoord(buf, pos);
    pos.x = origin.x;
    pos.y += getTextYLen(buf);
    return pos;
}

Coord fieldDrawPath(char *val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    char *empty = "NULL";
    char *text = val ? val : empty;
    sprintf(buf, "%s\"%s\"", label, text);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y+=len.y;
    return pos;
}

Coord fieldDrawFloat(const float val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    sprintf(buf, "%s%+10.3f", label, val);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y+=len.y;
    return pos;
}

Coord fieldDrawUint(const uint val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    sprintf(buf, "%s%3u", label, val);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y+=len.y;
    return pos;
}

Coord fieldDrawBool(const bool val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    sprintf(buf, "%s%s", label, val?"true":"false");
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y+=len.y;
    return pos;
}

Coord fieldDrawDir(const Direction val, char *label, Coord pos, const uint selected)
{
    char buf[256] = {0};
    sprintf(buf, "%s%c", label, DirectionChar[val]);
    const Length len = getTextLength(buf);
    fillRectCoordLength(pos, len);
    setTextColor(selected ? WHITE : GREY1);
    drawTextCoord(buf, pos);
    pos.y+=len.y;
    return pos;
}

// draws field, if selected > 0, highlights field as selected
// for fields with multiple entries, highlights entry in index of selected+1
Coord fieldDraw(const Field field, const Coord origin, const uint selected)
{
    Coord pos = origin;
    setColor(BLACK);
    switch(field.type){
        case F_MAPPIECETYPE:
            pos = fieldDrawMapPieceType(*((MapPieceType *)(field.ptr)), field.label, pos, selected);
            break;
        case F_SEGTYPE:
            pos = fieldDrawSegType(*((SegType *)(field.ptr)), field.label, pos, selected);
            break;
        case F_OBJTYPE:
            pos = fieldDrawObjType(*((ObjType *)(field.ptr)), field.label, pos, selected);
            break;
        case F_COORDF:
            pos = fieldDrawCoordf(*((Coordf *)(field.ptr)), field.label, pos, selected);
            break;
        case F_COLOR:
            pos = fieldDrawColor(*((Color *)(field.ptr)), field.label, pos, selected);
            break;
        case F_PATH:
            pos = fieldDrawPath(field.ptr, field.label, pos, selected);
            break;
        case F_FLOAT:
            pos = fieldDrawFloat(*((float *)(field.ptr)), field.label, pos, selected);
            break;
        case F_UINT:
            pos = fieldDrawUint(*((uint *)(field.ptr)), field.label, pos, selected);
            break;
        case F_BOOL:
            pos = fieldDrawBool(*((bool *)(field.ptr)), field.label, pos, selected);
            break;
        case F_DIR:
            pos = fieldDrawDir(*((Direction *)(field.ptr)), field.label, pos, selected);
            break;
        default:
            panic("Unknown field type: %u", field.type);
            break;
    }
    return pos;
}

void fieldPrintMap(Map *map)
{
    MapPiece start = pieceNext(map, (MapPiece){.type = M_ANY});
    if(start.type >= M_ANY)
        return;
    MapPiece cur = start;
    do{
        fieldPrint(cur);
        cur = pieceNext(map, cur);
    }while(!pieceSame(cur, start));
}

#endif /* end of include guard: FIELDS_H */
