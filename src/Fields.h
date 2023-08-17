#ifndef FIELDS_H
#define FIELDS_H

void printPieceFields(const MapPiece piece)
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
                printf("%s(%+14.6f,%+14.6f)\n", fields.field[i].label, val3.x, val3.y);
                break;
            case F_COLOR:;
                Color val4 = *((Color *)(fields.field[i].ptr));
                printf("%s(%3u,%3u,%3u)\n", fields.field[i].label, val4.r, val4.g, val4.b);
                break;
            case F_PATH:;
                char *val5 = ((char **)(fields.field[i].ptr)) ? (*((char **)(fields.field[i].ptr))) : "NULL";
                printf("%s\"%s\"\n", fields.field[i].label, val5?val5:"NULL");
                break;
            case F_FLOAT:;
                float val6 = *((float *)(fields.field[i].ptr));
                printf("%s%+14.6f\n", fields.field[i].label, val6);
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
}

Coord drawField(const Field field, const Coord cursor, const Coord origin, const int i)
{
    Coord pos = origin;
    Length len = {0};
    setColor(BLACK);
    char buf[256] = {0};
    switch(field.type){
        case F_MAPPIECETYPE:;
            MapPieceType val0 = *((MapPieceType *)(field.ptr));
            sprintf(buf, "%s%s", field.label, MapPieceTypeStr[val0]);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y += len.y;
            break;
        case F_SEGTYPE:;
            SegType val1 = *((SegType *)(field.ptr));
            sprintf(buf, "%s%s", field.label, SegTypeStr[val1]);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y += len.y;
            break;
        case F_OBJTYPE:;
            ObjType val2 = *((ObjType *)(field.ptr));
            sprintf(buf, "%s%s", field.label, ObjTypeStr[val2]);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y += len.y;
            break;
        case F_COORDF:;
            Coordf val3 = *((Coordf *)(field.ptr));
            sprintf(buf, "%s(%+14.6f,%+14.6f)", field.label, val3.x, val3.y);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);

            setTextColor(GREY1);
            sprintf(buf, "%s(", field.label);
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i && cursor.x == 0 ? WHITE : GREY1);
            sprintf(buf, "%+14.6f", val3.x);
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(GREY1);
            sprintf(buf, ",");
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i && cursor.x == 1 ? WHITE : GREY1);
            sprintf(buf, "%+14.6f", val3.y);
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(GREY1);
            sprintf(buf, ")");
            drawTextCoord(buf, pos);
            pos.x = origin.x;
            pos.y += getTextYLen(buf);
            break;
        case F_COLOR:;
            Color val4 = *((Color *)(field.ptr));
            sprintf(buf, "%s(%3u,%3u,%3u)", field.label, val4.r, val4.g, val4.b);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);

            setTextColor(cursor.y == i ? WHITE : GREY1);
            sprintf(buf, "%s(", field.label);
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i && cursor.x == 0 ? RED : GREY1);
            sprintf(buf, "%3u", val4.r);
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i ? WHITE : GREY1);
            sprintf(buf, ",");
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i && cursor.x == 1 ? GREEN : GREY1);
            sprintf(buf, "%3u", val4.g);
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i ? WHITE : GREY1);
            sprintf(buf, ",");
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i && cursor.x == 2 ? BLUE : GREY1);
            sprintf(buf, "%3u", val4.b);
            drawTextCoord(buf, pos);
            pos.x += getTextXLen(buf);

            setTextColor(cursor.y == i ? WHITE : GREY1);
            sprintf(buf, ")");
            drawTextCoord(buf, pos);
            pos.x = origin.x;
            pos.y += getTextYLen(buf);
            break;
        case F_PATH:;
            char *val5 = ((char **)(field.ptr)) ? (*((char **)(field.ptr))) : "NULL";
            sprintf(buf, "%s\"%s\"", field.label, val5?val5:"NULL");
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y+=len.y;
            break;
        case F_FLOAT:;
            float val6 = *((float *)(field.ptr));
            sprintf(buf, "%s%+14.6f", field.label, val6);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y+=len.y;
            break;
        case F_UINT:;
            uint val7 = *((uint *)(field.ptr));
            sprintf(buf, "%s%3u", field.label, val7);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y+=len.y;
            break;
        case F_BOOL:;
            bool val8 = *((bool *)(field.ptr));
            sprintf(buf, "%s%s", field.label, val8?"true":"false");
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y+=len.y;
            break;
        case F_DIR:;
            Direction val9 = *((Direction *)(field.ptr));
            sprintf(buf, "%s%c", field.label, DirectionChar[val9]);
            len = getTextLength(buf);
            fillRectCoordLength(pos, len);
            setTextColor(cursor.y == i ? WHITE : GREY1);
            drawTextCoord(buf, pos);
            pos.y+=len.y;
            break;
        default:
            panic("Unknown field type: %u", field.type);
            break;
    }
    return pos;
}

Coord drawPieceFields(const PieceFields fields, const Coord cursor, const Coord origin)
{
    Coord pos = origin;
    for(int i = 0; i < (int)fields.numFields; i++)
        pos = drawField(fields.field[i], cursor, pos, i);
    return pos;
}

#endif /* end of include guard: FIELDS_H */
