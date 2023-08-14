#ifndef TYPES_H
#define TYPES_H

typedef struct{
    Coord pos;
    Length len;
}View;

typedef enum{S_END = -1, S_WALL, S_WIND, S_TRIG, S_DOOR, S_CONV, S_N}SegType;
char *SegTypeStr[S_N] = {"S_WALL", "S_WIND", "S_TRIG", "S_DOOR", "S_CONV"};

const uint SegTypeNumFields[S_N] = {5, 7, 5, 9, 6};

typedef struct Seg{
    SegType type;
    Coordf a;
    Coordf b;
    Color color;
    union{
        struct{
            char path[128];
            Texture *texture;
        }wall;
        struct{
            Color topColor;
            float height;
            float top;
        }wind;
        struct{
            uint id;
            float pos;
            bool state;
            float speed;
            Direction closeDir;
        }door;
        struct{
            uint id;
            Coordf c;
            Coordf d;
        }trig;
        struct{
            uint idA;
            uint idB;
        }conv;
    };
    struct Seg *next;
}Seg;

typedef struct{
    Seg *seg;
    uint len;
}SegPacked;

typedef enum{O_SPAWN, O_KEY, O_MOB, O_N}ObjType;
char *ObjTypeStr[O_N] = {"O_SPAWN", "O_KEY", "O_MOB"};
typedef struct Obj{
    ObjType type;
    Coordf pos;
    union{
        struct{
            float ang;
        }spawn;
        struct{
            Color c;
        }key;
        struct{
            Coordf origin;
            Coordf vec;
            Texture *txtr;
            char *path;
        }mob;
    };
    struct Obj *next;
}Obj;

typedef struct Player{
    Coordf pos;
    float ang;
    float speed;
    uint health;
    uint maxHealth;
}Player;

typedef enum{M_SEG, M_OBJ, M_ANY, M_NONE}MapPieceType;
const int PieceTypeNum[2] = {S_N, O_N};
typedef struct{
    MapPieceType type;
    union{
        Seg *seg;
        Obj *obj;
    };
}MapPiece;

typedef struct{
    Player player;
    char *name;
    char *path;
    File *file;
    // each index is a list of the corrosponding SegType
    Seg *seg[S_N];
    // each index is a list of the corrosponding ObjType
    Obj *obj[O_N];
}Map;

typedef struct{
    Seg *wall;
    float dst;
    Coordf pos;
}Ray;

typedef struct{
    Coord spos;
    Coordf mpos;
    Coord sposd;
    Coordf mposd;
    Coord ssnap;
    Coordf msnap;
    Coord ssnapd;
    Coordf msnapd;
    bool drag;
}Minfo;

typedef struct Selection{
    SegType newSegType;
    ObjType newObjType;
    MapPiece piece;
    bool showInfo;
    Coordf posOrig;
    Coordf *pos;
    Coord cursor;
    uint tscale;
}Selection;

typedef struct Update{
    uint id;
    bool state;
    struct Update *next;
}Update;

#endif /* end of include guard: TYPES_H */
