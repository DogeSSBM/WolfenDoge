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
    sel->holp = pos ? *pos : (Coordf){0};
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
    Coordf *nearestPos = NULL;
    const MapPiece piece = pieceNearest(map, pos, &nearestPos);
    if(pieceEmpty(piece))
        return NULL;
    return selNew(cursor, nearestPos, pieceFields(piece));
}

Selection* selLast(Selection *sel)
{
    if(!sel)
        return NULL;
    while(sel->next)
        sel = sel->next;
    return sel;
}

bool selPosSelected(Selection *list, Coordf *pos)
{
    while(list){
        if(list->pos == pos)
            return true;
        list = list->next;
    }
    return false;
}

bool selPieceSelected(Selection *list, const MapPiece piece)
{
    while(list){
        if(pieceContainsCoord(piece, list->pos))
            return true;
        list = list->next;
    }
    return false;
}

Selection* selPieceSelection(Selection *list, const MapPiece piece)
{
    while(list){
        if(pieceSame(piece, list->fields.piece))
            return list;
        list = list->next;
    }
    return NULL;
}

// duplicates selection such that the duplicate points to the same Seg / Obj
Selection* selDupShallow(Selection *sel)
{
    if(!sel)
        return NULL;
    Selection *dup = calloc(1, sizeof(Selection));
    *dup = *sel;
    dup->next = NULL;
    return dup;
}

// duplicates selection and the Seg / Obj
Selection* selDupDeep(Selection *sel)
{
    if(!sel)
        return NULL;
    Selection *dup = selDupShallow(sel);
    dup->fields =  pieceFieldsDup(sel->fields);
    if(sel->pos == NULL)
        return dup;
    const int index = pieceCoordIndex(sel->fields.piece, sel->pos);
    if(index >= 0)
        dup->pos = pieceCoords(dup->fields.piece).coord[index];
    return dup;
}

// returns piece if sel piece occurs in list up to sel (exclusive)
bool selPieceFieldsBefore(Selection *list, Selection *sel)
{
    assertExpr(sel);
    Selection *cur = list;
    while(cur && cur != sel){
        if(pieceSame(cur->fields.piece, sel->fields.piece))
            return true;
        cur = cur->next;
    }
    return false;
}

void selListSetPiece(Selection *list, const PieceFields replace, const PieceFields with)
{
    while(list){
        if(pieceSame(list->fields.piece, replace.piece)){
            list->pos = pieceCoords(with.piece).coord[pieceCoordIndex(list->fields.piece, list->pos)];
            list->fields = with;
        }
        list = list->next;
    }
}

// ensures all positions of each piece in the selection list is selected
// adds any missing position selections to the head of the list
// returns head of list
Selection* selListAddAllPiecePos(Selection *list)
{
    Selection *head = list;
    while(list){
        const PieceCoords pc = fieldCoords(list->fields);
        for(st i = 0; i < pc.numCoord; i++){
            if(!selPosSelected(list, pc.coord[i])){
                Selection *next = head;
                head = selNew(list->cursor, pc.coord[i], list->fields);
                head->next = next;
            }
        }
        list = list->next;
    }
    return head;
}

// duplicates each unique piece in the list, swapping them out for the old ones
//
void selListDupAddUniquePieces(Map *map, Selection *list)
{
    Selection *cur = list;
    while(cur){
        if(!selPieceFieldsBefore(list, cur)){
            const PieceFields new = pieceFieldsDup(cur->fields);
            mapAddPiece(map, new.piece);
            selListSetPiece(cur, cur->fields, new);
        }
        cur = cur->next;
    }
}

// prints the selection list
void selPrint(Selection *list)
{
    printf("================ Selection List ================\n");
    while(list){
        fieldPrint(list->fields.piece);
        list = list->next;
    }
}

#endif /* end of include guard: MAPEDITORSELECTION_H */
