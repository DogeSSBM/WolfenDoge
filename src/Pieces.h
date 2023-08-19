#ifndef PIECES_H
#define PIECES_H

MapPiece segToPiece(Seg *seg)
{
    return (MapPiece){
        .type = seg ? M_SEG : M_NONE,
        .seg = seg
    };
}

MapPiece objToPiece(Obj *obj)
{
    return (MapPiece){
        .type = obj ? M_OBJ : M_NONE,
        .obj = obj
    };
}

bool pieceEmpty(const MapPiece piece)
{
    if(piece.type >= M_ANY)
        return true;
    assertExpr(piece.seg || piece.obj);
    return false;
}

PieceCoords pieceCoords(const MapPiece piece)
{
    PieceCoords pp = {.piece = piece};
    if(piece.type >= M_ANY)
        return pp;
    if(piece.type == M_SEG){
        pp.numCoord = SegTypeNumCoord[piece.seg->type];
        pp.coord[0] = &piece.seg->a;
        pp.coord[1] = &piece.seg->b;
        if(piece.seg->type == S_TRIG){
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
    }
    return pp;
}

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

Color pieceColor(const MapPiece piece)
{
    assertExpr(piece.type < M_ANY);
    if(piece.type == M_SEG)
        return piece.seg->color;
    if(piece.obj->type == O_KEY)
        return piece.obj->key.c;
    return WHITE;
}

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

MapPiece pieceNext(Map *map, MapPiece piece)
{
    assertExpr(piece.type <= M_ANY);
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

MapPiece pieceNextCoord(Map *map, MapPiece piece, Coordf **pos)
{
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

MapPiece pieceNextSameCoord(Map *map, MapPiece piece, Coordf **pos)
{
    Coordf target = **pos;
    Coordf *coord = *pos;
    do{
        piece = pieceNextCoord(map, piece, &coord);
    }while(!cfSame(target, *coord));
    *pos = coord;
    return piece;
}

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

st pieceCountTotal(Map *map)
{
    const MapPiece start = pieceNext(map, (MapPiece){.type = M_ANY});
    if(start.type == M_NONE)
        return 0;
    MapPiece cur = start;
    st count = 0;
    while(!pieceSame((cur = pieceNext(map, cur)), start))
        count++;
    return count;
}

// MapPiece pieceNew(const NewPieceInfo )
// {
//     return (NewPieceInfo){
//         .
//     }
// }

#endif /* end of include guard: PIECES_H */
