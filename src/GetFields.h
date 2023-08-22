#ifndef GETFIELDS_H
#define GETFIELDS_H

// fills in fields for segment type pieces
PieceFields segFields(const MapPiece piece)
{
    assertExpr(piece.type == M_SEG);
    PieceFields fields = {
        SegTypeFields[piece.seg->type],
        piece,
        {
            (Field){.label = "SegType: ", .type = F_SEGTYPE, .ptr = &piece.seg->type},
            (Field){.label = "a:      ", .type = F_COORDF, .ptr = &piece.seg->a},
            (Field){.label = "b:      ", .type = F_COORDF, .ptr = &piece.seg->b},
            (Field){.label = "color: ", .type = F_COLOR, .ptr = &piece.seg->color},
        }
    };
    switch(piece.seg->type){
        case S_WALL:
            fields.field[4] = (Field){.label = "path: ", .type = F_PATH, .ptr = (char*)(piece.seg->wall.path)};
            break;
        case S_WIND:
            fields.field[4] = (Field){.label = "topColor: ", .type = F_COLOR, .ptr = &piece.seg->wind.topColor};
            fields.field[5] = (Field){.label = "height: ", .type = F_FLOAT, .ptr = &piece.seg->wind.height};
            fields.field[6] = (Field){.label = "top: ", .type = F_FLOAT, .ptr = &piece.seg->wind.top};
            break;
        case S_DOOR:
            fields.field[4] = (Field){.label = "id: ", .type = F_UINT, .ptr = &piece.seg->door.id};
            fields.field[5] = (Field){.label = "pos: ", .type = F_FLOAT, .ptr = &piece.seg->door.pos};
            fields.field[6] = (Field){.label = "state: ", .type = F_BOOL, .ptr = &piece.seg->door.state};
            fields.field[7] = (Field){.label = "speed: ", .type = F_FLOAT, .ptr = &piece.seg->door.speed};
            fields.field[8] = (Field){.label = "closeDir: ", .type = F_DIR, .ptr = &piece.seg->door.closeDir};
            break;
        case S_TRIG:
            fields.field[4] = (Field){.label = "id: ", .type = F_UINT, .ptr = &piece.seg->trig.id};
            fields.field[5] = (Field){.label = "c:      ", .type = F_COORDF, .ptr = &piece.seg->trig.c};
            fields.field[6] = (Field){.label = "d:      ", .type = F_COORDF, .ptr = &piece.seg->trig.d};
            break;
        case S_CONV:
            fields.field[4] = (Field){.label = "idA: ", .type = F_UINT, .ptr = &piece.seg->conv.idA};
            fields.field[5] = (Field){.label = "idB: ", .type = F_UINT, .ptr = &piece.seg->conv.idB};
            break;
        default:
            panic("Unknown SegType: %u", piece.seg->type);
            break;
    }
    return fields;
}

// fills in fields for object type pieces
PieceFields objFields(const MapPiece piece)
{
    assertExpr(piece.type == M_OBJ);
    PieceFields fields = {
        ObjTypeFields[piece.obj->type],
        piece,
        {
            (Field){.label = "ObjType: ", .type = F_OBJTYPE, .ptr = &piece.obj->type},
            (Field){.label = "pos:    ", .type = F_COORDF, .ptr = &piece.obj->pos}
        }
    };
    switch(piece.obj->type){
        case O_SPAWN:
            fields.field[2] = (Field){.label = "ang: ", .type = F_FLOAT, .ptr = &piece.obj->spawn.ang};
            break;
        case O_KEY:
            fields.field[2] = (Field){.label = "c: ", .type = F_COLOR, .ptr = &piece.obj->key.c};
            break;
        case O_MOB:
            fields.field[2] = (Field){.label = "a:      ", .type = F_COORDF, .ptr = &piece.obj->mob.a};
            fields.field[3] = (Field){.label = "b:      ", .type = F_COORDF, .ptr = &piece.obj->mob.b};
            fields.field[4] = (Field){.label = "origin: ", .type = F_COORDF, .ptr = &piece.obj->mob.origin};
            fields.field[5] = (Field){.label = "vec:    ", .type = F_COORDF, .ptr = &piece.obj->mob.vec};
            fields.field[6] = (Field){.label = "len:    ", .type = F_COORDF, .ptr = &piece.obj->mob.len};
            fields.field[7] = (Field){.label = "path: ", .type = F_PATH, .ptr = (char*)(piece.obj->mob.path)};
            break;
        default:
            panic("Unknown ObjType: %u", piece.obj->type);
            break;
    }
    return fields;
}

// fills in fields for given piece
PieceFields pieceFields(const MapPiece piece)
{
    assertExpr(piece.type < M_ANY);
    if(piece.type == M_SEG)
        return segFields(piece);
    return objFields(piece);
}

#endif /* end of include guard: GETFIELDS_H */
