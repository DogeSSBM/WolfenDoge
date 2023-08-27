#ifndef PIECES_H
#define PIECES_H

// returns a MapPiece of type M_SEG containing seg
MapPiece segToPiece(Seg *seg)
{
    return (MapPiece){
        .type = seg ? M_SEG : M_NONE,
        .seg = seg
    };
}

// returns a MapPiece of type M_OBJ containing obj
MapPiece objToPiece(Obj *obj)
{
    return (MapPiece){
        .type = obj ? M_OBJ : M_NONE,
        .obj = obj
    };
}

// returns true if piece has type indicating it does not contain a seg or obj
// panics if it should contain a piece but its respective pointer is NULL
bool pieceEmpty(const MapPiece piece)
{
    if(piece.type >= M_ANY)
        return true;
    assertExpr(piece.seg || piece.obj);
    return false;
}

// returns struct containing all of the pieces coords
PieceCoords pieceCoords(const MapPiece piece)
{
    PieceCoords pp = {.piece = piece};
    if(piece.type >= M_ANY)
        return pp;
    if(piece.type == M_SEG){
        pp.numCoord = SegTypeNumCoord[piece.seg->type];
        pp.coord[0] = &piece.seg->a;
        pp.coord[1] = &piece.seg->b;
        if(piece.seg->type == S_PORT){
            pp.coord[2] = &piece.seg->port.a;
            pp.coord[3] = &piece.seg->port.b;
        }else if(piece.seg->type == S_TRIG){
            pp.coord[2] = &piece.seg->trig.c;
            pp.coord[3] = &piece.seg->trig.d;
        }
        return pp;
    }
    pp.numCoord = ObjTypeNumCoord[piece.obj->type];
    pp.coord[0] = &piece.obj->pos;
    if(piece.obj->type == O_MOB){
        pp.coord[1] = &piece.obj->mob.a;
        pp.coord[2] = &piece.obj->mob.b;
        pp.coord[3] = &piece.obj->mob.origin;
        pp.coord[4] = &piece.obj->mob.vec;
        pp.coord[5] = &piece.obj->mob.len;
    }
    return pp;
}

// returns struct containing all of the piece from fields coords
PieceCoords fieldCoords(const PieceFields fields)
{
    return pieceCoords(fields.piece);
}

// returns true if pos is one of the pieces coords
bool pieceContainsCoord(const MapPiece piece, Coordf *pos)
{
    if(piece.type >= M_ANY || !pos)
        return false;
    PieceCoords coords = pieceCoords(piece);
    for(st i = 0; i < coords.numCoord; i++)
        if(coords.coord[i] == pos)
            return true;
    return false;
}

// if pos is not one of pieces coords returns -1
// otherwise returns the index of pieces PieceCoords array that contains pos
int pieceCoordIndex(const MapPiece piece, Coordf *pos)
{
    if(piece.type >= M_ANY || !pos)
        return -1;
    PieceCoords coords = pieceCoords(piece);
    for(int i = 0; i < (int)coords.numCoord; i++)
        if(coords.coord[i] == pos)
            return i;
    return -1;
}

// returns primary color of segment / object contained by piece
// panics if piece is empty
Color pieceColor(const MapPiece piece)
{
    assertExpr(piece.type < M_ANY);
    if(piece.type == M_SEG)
        return piece.seg->color;
    if(piece.obj->type == O_KEY)
        return piece.obj->key.c;
    return WHITE;
}

// removes piece from map and frees it (deep)
void pieceDelete(Map *map, MapPiece piece)
{
    assertExpr(map);
    if(pieceEmpty(piece))
        return;
    if(piece.type == M_SEG)
        map->seg[piece.seg->type] = segDelete(map->seg[piece.seg->type], piece.seg);
    else
        map->obj[piece.obj->type] = objDelete(map->obj[piece.obj->type], piece.obj);
}

// returns the next object / segment in map (wraps)
// if piece is M_ANY, returns first available object / segment in map
// if map is empty, returns piece with type M_NONE
MapPiece pieceNext(Map *map, MapPiece piece)
{
    assertExpr(map && piece.type <= M_ANY);
    if(piece.type == M_SEG || piece.type == M_ANY){
        if(piece.type != M_ANY && piece.seg && piece.seg->next){
            piece.seg = piece.seg->next;
            return piece;
        }
        for(SegType stype = ((piece.type == M_ANY || !piece.seg) ? 0 : piece.seg->type+1); stype < S_N; stype++){
            if(map->seg[stype]){
                piece.type = M_SEG;
                piece.seg = map->seg[stype];
                return piece;
            }
        }
        for(ObjType otype = 0; otype < O_N; otype++){
            if(map->obj[otype]){
                piece.type = M_OBJ;
                piece.obj = map->obj[otype];
                return piece;
            }
        }
        for(SegType stype = 0; stype <= ((piece.type == M_ANY || !piece.seg) ? S_N-1 : piece.seg->type); stype++){
            if(map->seg[stype]){
                piece.type = M_SEG;
                piece.seg = map->seg[stype];
                return piece;
            }
        }
        if(piece.type == M_SEG && !piece.seg)
            return (MapPiece){.type = M_NONE};
    }

    assertExpr(piece.type == M_OBJ || piece.type == M_ANY);
    if(piece.type != M_ANY && piece.obj && piece.obj->next){
        piece.obj = piece.obj->next;
        return piece;
    }
    for(ObjType otype = ((piece.type == M_ANY || !piece.obj) ? 0 : piece.obj->type+1); otype < O_N; otype++){
        if(map->obj[otype]){
            piece.type = M_OBJ;
            piece.obj = map->obj[otype];
            return piece;
        }
    }
    for(SegType stype = 0; stype < S_N; stype++){
        if(map->seg[stype]){
            piece.type = M_SEG;
            piece.seg = map->seg[stype];
            return piece;
        }
    }
    for(ObjType otype = 0; otype <= ((piece.type == M_ANY || !piece.obj) ? O_N-1 : piece.obj->type); otype++){
        if(map->obj[otype]){
            piece.type = M_OBJ;
            piece.obj = map->obj[otype];
            return piece;
        }
    }
    if(piece.type == M_ANY || !piece.obj)
        piece.type = M_NONE;
    return piece;
}

// points *pos to the next available coord in the map
// returns the piece that contains that coord
MapPiece pieceNextCoord(Map *map, MapPiece piece, Coordf **pos)
{
    assertExpr(map && pos);
    PieceCoords coords = pieceCoords(piece);
    st i = 0;
    for(i = 0; i < coords.numCoord; i++){
        if(coords.coord[i] == *pos && i+1 < coords.numCoord){
            *pos = coords.coord[i+1];
            return piece;
        }
    }
    piece = pieceNext(map, piece);
    coords = pieceCoords(piece);
    *pos = coords.coord[0];
    return piece;
}

// points *pos to the next available coord in the map
// returns the piece that contains that coord
MapPiece pieceNextSameCoord(Map *map, MapPiece piece, Coordf **pos)
{
    assertExpr(map && pos);
    Coordf target = **pos;
    Coordf *coord = *pos;
    do{
        piece = pieceNextCoord(map, piece, &coord);
    }while(!cfSame(target, *coord));
    *pos = coord;
    return piece;
}

// true if both components of pos are within the bounds of min (inclusive) and max (inclusive)
bool cfInBounds(const Coordf pos, const Coordf min, const Coordf max)
{
    return (
        pos.x >= min.x &&
        pos.x <= max.x &&
        pos.y >= min.y &&
        pos.y <= max.y
    );
}

// returns true if a and b contain the same segment / object
// or if a and b are both M_NONE / M_ANY
bool pieceSame(const MapPiece a, const MapPiece b)
{
    if(a.type != b.type)
        return false;
    if(a.type == M_SEG)
        return a.seg == b.seg;
    if(a.type == M_OBJ)
        return a.obj == b.obj;
    return true;
}

// returns a copy (deep) of piece
MapPiece pieceDup(const MapPiece piece)
{
    assertExpr(piece.type < M_ANY);
    if(piece.type == M_SEG){
        assertExpr(piece.seg);
        return (MapPiece){
            .type = piece.type,
            .seg = segDup(piece.seg)
        };
    }
    assertExpr(piece.obj);
    return (MapPiece){
        .type = piece.type,
        .obj = objDup(piece.obj)
    };

}

// returns a copy (deep) of fields
PieceFields pieceFieldsDup(const PieceFields fields)
{
    PieceFields ret = fields;
    ret.piece = pieceDup(fields.piece);
    return ret;
}

// points *nearestPos to the coord of a piece that is closest to pos
// returns the piece that contains that coord
// returns piece with type M_NONE if map is empty
MapPiece pieceNearest(Map *map, const Coordf pos, Coordf **nearestPos)
{
    MapPiece start = pieceNext(map, (MapPiece){.type = M_ANY});
    if(start.type >= M_ANY)
        return (MapPiece){.type = M_NONE};
    MapPiece cur = start;
    MapPiece nearest = start;
    *nearestPos = pieceCoords(start).coord[0];
    float nearestDst = cfDist(**nearestPos, pos);
    do{
        PieceCoords coords = pieceCoords(cur);
        for(st i = 0; i < coords.numCoord; i++){
            const float curDst = cfDist(*coords.coord[i], pos);
            if(curDst < nearestDst){
                nearestDst = curDst;
                *nearestPos = coords.coord[i];
                nearest = cur;
            }
        }
        cur = pieceNext(map, cur);
    }while(!pieceSame(cur, start));
    return nearest;
}

// returns total number of segments + objects in map
st pieceCountTotal(Map *map)
{
    const MapPiece start = pieceNext(map, (MapPiece){.type = M_ANY});
    if(start.type == M_NONE)
        return 0;
    MapPiece cur = start;
    st count = 1;
    while(!pieceSame((cur = pieceNext(map, cur)), start))
        count++;
    return count;
}

// returns a piece containing a new segment / object with coords a and b
MapPiece pieceNew(const NewPieceInfo pieceInfo, const Coordf a, const Coordf b)
{
    assertExpr(pieceInfo.pieceType < M_ANY);
    MapPiece piece = {.type = pieceInfo.pieceType};
    if(piece.type == M_SEG)
        piece.seg = segNew(pieceInfo.segType, a, b);
    else
        piece.obj = objNew(pieceInfo.objType, a);
    return piece;
}

#endif /* end of include guard: PIECES_H */
