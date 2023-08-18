#ifndef TYPES_H
#define TYPES_H

typedef struct{
    Coord pos;
    Length len;
}View;

typedef enum{S_END = -1,  S_WALL,    S_WIND,     S_TRIG,     S_DOOR,     S_CONV, S_N}SegType;
st SegTypeFields[S_N] = {      5,         7,          7,          9,          6     };
st SegTypeNumCoord[S_N] = {    2,         2,          4,          2,          2     };
char *SegTypeStr[S_N] = {"S_WALL",  "S_WIND",   "S_TRIG",   "S_DOOR",   "S_CONV"    };
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

typedef enum{             O_SPAWN,       O_KEY,      O_MOB, O_N}ObjType;
st ObjTypeFields[O_N] = {       3,           3,          8     };
st ObjTypeNumCoord[O_N] = {     1,           1,          4     };
char *ObjTypeStr[O_N] = {"O_SPAWN",     "O_KEY",    "O_MOB"    };
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
            Coordf a;       // a and b are updated dynamically at runtime. they corrospond to where the left and right sides of
            Coordf b;       // the mob would be as it faces the player
            Coordf origin;
            Coordf vec;
            Coordf len;
            Texture *texture;
            char path[128];
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

typedef enum{M_SEG, M_OBJ, M_ANY, M_NONE, M_N}MapPieceType;
char *MapPieceTypeStr[M_N] = {"M_SEG", "M_OBJ", "M_ANY", "M_NONE"};
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

// ray intersections
typedef struct Ray{
    MapPiece piece;
    float dst;
    Coordf pos;
    struct Ray *next;
}Ray;

typedef struct Update{
    uint id;
    bool state;
    struct Update *next;
}Update;

typedef enum              {F_MAPPIECETYPE, F_SEGTYPE, F_OBJTYPE, F_COORDF, F_COLOR, F_PATH, F_FLOAT, F_UINT, F_BOOL, F_DIR,  F_N}FieldType;
uint FieldTypeXlen[F_N] = {             1,         1,         1,        2,       3,      1,       1,      1,      1,     1      };
char *FieldTypeStr[F_N] = {"MapPieceType", "SegType", "ObjType", "Coordf", "Color", "Path", "float", "uint", "bool", "Direction"};
typedef struct{
    char *label;
    FieldType type;
    void *ptr;
}Field;

typedef struct{
    st numFields;
    MapPiece piece;
    Field field[10];
}PieceFields;

typedef struct{
    Offset off;
    float scale;
    Coord cursor;
    Length wlen;
    Lengthf mlen;
    bool selection;
    Coordf *selectedPos;
    PieceFields fields;
    struct{
        float scale;
        Length wlen;
        Lengthf mlen;
    }prv;
    struct{
        bool enabled;
        float len;
    }snap;
    struct{
        struct{
            Coordf pos;
            Coordf ldown;
        }map;
        struct{
            Coord pos;
            Coord ldown;
        }win;
    }mouse;
}EditorState;

typedef struct{
    MapPiece piece;
    st numCoord;
    Coordf *coord[4];
}PieceCoords;

#endif /* end of include guard: TYPES_H */
