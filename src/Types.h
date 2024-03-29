#ifndef TYPES_H
#define TYPES_H

typedef enum             { C_CONV,   C_NOT,   C_AND,   C_OR, C_N}ConvType;
char *ConvTypeStr[C_N] = {"C_CONV", "C_NOT", "C_AND", "C_OR"    };

typedef enum             { T_ZONE,   T_FLIP,   T_ZONE_ONCE,   T_FLIP_ONCE, T_N}TrigType;
char *TrigTypeStr[T_N] = {"T_ZONE", "T_FLIP", "T_ZONE_ONCE", "T_FLIP_ONCE"    };

typedef enum   {S_END = -1, S_WALL,    S_WIND,     S_TRIG,   S_PORT,     S_DOOR,    S_N}SegType;
char *SegTypeStr[S_N] =   {"S_WALL",  "S_WIND",   "S_TRIG", "S_PORT",   "S_DOOR",      };
st SegTypeFields[S_N] =   {      5,         7,         10,        6,          9,       };
st SegTypeNumCoord[S_N] = {      2,         2,          4,        4,          2,       };
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
            Coordf a;
            Coordf b;
        }port;
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
            TrigType type;
            uint id;
            Coordf c;
            Coordf d;
            bool start;
            bool state;
        }trig;
    };
    struct Seg *next;
}Seg;

typedef enum              { O_SPAWN,       O_KEY,      O_MOB,   O_CONV, O_N}ObjType;
char *ObjTypeStr[O_N] =   {"O_SPAWN",     "O_KEY",    "O_MOB", "O_CONV"    };
st ObjTypeFields[O_N] =   {       3,           3,          8,        7     };
st ObjTypeNumCoord[O_N] = {       1,           1,          6,        1     };
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
        struct{
            ConvType type;
            uint inIdA;
            uint inIdB;
            uint outId;
            bool outState;
        }conv;
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

const int PieceTypeNum[2] =  {S_N, O_N};
typedef enum                 { M_SEG,   M_OBJ,   M_ANY,   M_NONE, M_N}MapPieceType;
char *MapPieceTypeStr[M_N] = {"M_SEG", "M_OBJ", "M_ANY", "M_NONE"    };
typedef struct{
    MapPieceType type;
    union{
        Seg *seg;
        Obj *obj;
    };
}MapPiece;

typedef struct{
    Coord pos;
    Length len;
}View;

typedef struct Ray{
    MapPiece piece;
    float ang;
    float dst;
    Coordf origin;
    Coordf pos;
    bool cor;
    struct Ray *next;
}Ray;

typedef enum              {F_MAPPIECETYPE,  F_SEGTYPE,  F_OBJTYPE,   F_CONVTYPE,  F_COORDF,  F_COLOR,  F_PATH,  F_FLOAT,  F_UINT,  F_BOOL,  F_TRIGTYPE,  F_DIR,       F_N}FieldType;
char *FieldTypeStr[F_N] = { "MapPieceType",  "SegType",  "ObjType", "F_CONVTYPE",  "Coordf",  "Color",  "Path",  "float",  "uint",  "bool",  "TrigType",  "Direction"    };
uint FieldTypeXlen[F_N] = {             1,          1,          1,            1,         2,        3,       1,        1,       1,       1,           1,            1     };
typedef struct{
    char *label;
    FieldType type;
    void *ptr;
}Field;

typedef struct{
    MapPiece piece;
    st numCoord;
    Coordf *coord[10];
}PieceCoords;

typedef struct{
    st numFields;
    MapPiece piece;
    Field field[10];
}PieceFields;

typedef struct{
    Offset off;
    Length wlen;
    Lengthf mlen;
    float scale;
    struct{
        Offset off;
        Length wlen;
        Lengthf mlen;
        float scale;
    }prv;
}Camera;

typedef struct Selection{
    Coord *cursor;
    Coordf holp;
    bool moved;
    Coordf *pos;
    PieceFields fields;
    struct Selection *next;
}Selection;

typedef struct{
    Coordf pos;
    Coordf ldown;
    Coordf rdown;
    struct{
        Coordf pos;
        Coordf ldown;
        Coordf rdown;
    }prv;
}MouseMap;

typedef struct{
    Coord pos;
    Coord ldown;
    Coord rdown;
    struct{
        Coord pos;
        Coord ldown;
        Coord rdown;
    }prv;
}MouseWin;

typedef struct{
    MouseMap map;
    MouseWin win;
}Mouse;

typedef struct{
    bool active;
    float len;
    Mouse mouse;
}Snap;

typedef struct{
    MapPieceType pieceType;
    union{
        SegType segType;
        ObjType objType;
    };
}NewPieceInfo;

typedef struct{
    NewPieceInfo pieceInfo;
    Camera cam;
    Coord cursor;
    Selection *sel;
    Mouse mouse;
    Snap snap;
}EditorState;

#endif /* end of include guard: TYPES_H */
