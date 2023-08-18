#ifndef MAPEDITORSELECTION_H
#define MAPEDITORSELECTION_H

Selection* selNew(Coord *cursor, Coordf *pos, const PieceFields fields)
{
    assertExpr(cursor);
    if(!pos){
        printf("Cannot create selection with NULL pos\n");
        return NULL;
    }
    if(fields.piece.type >= M_ANY){
        printf("Cannot create selection with piece type: \"%s\"\n", MapPieceTypeStr[fields.piece.type]);
        return NULL;
    }
    Selection *sel = calloc(1, sizeof(Selection));
    sel->cursor = cursor;
    sel->pos = pos;
    sel->fields = fields;
    return sel;
}

Selection* selAppend(Selection *head, Selection *tail)
{
    if(!head)
        return tail;
    Selection *cur = head;
    while(cur->next)
        cur = cur->next;
    cur->next = tail;
    return head;
}

st selLen(Selection *list)
{
    st len = 0;
    while(list){
        len++;
        list = list->next;
    }
    return len;
}

Selection* selFree(Selection *list)
{
    if(!list)
        return NULL;
    Selection *next = list->next;
    free(list);
    return next;
}

Selection* selFreeList(Selection *sel)
{
    while(sel)
        sel = selFree(sel);
    return NULL;
}

Selection* selPosNearest(Map *map, Coord *cursor, const Coordf pos)
{
    Coordf *nearestPos;
    const MapPiece piece = pieceNearest(map, pos, &nearestPos);
    if(pieceEmpty(piece))
        return NULL;
    return selNew(cursor, nearestPos, pieceFields(piece));
}

#endif /* end of include guard: MAPEDITORSELECTION_H */