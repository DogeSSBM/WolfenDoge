#ifndef TYPES_H
#define TYPES_H

typedef struct{
    Coord pos;
    Length len;
}View;

typedef struct Player{
    Coordf pos;
    float ang;
    float speed;
}Player;

typedef enum{S_WALL, S_WIND, S_TRIG, S_DOOR, S_CONV, S_N}SegType;
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
    bool showInfo;
    Seg *wall;
    Coordf posOrig;
    Coordf *pos;
    struct Selection *next;
    Coord cursor;
    uint tscale;
}Selection;

typedef struct Update{
    uint id;
    bool state;
    struct Update *next;
}Update;

#endif /* end of include guard: TYPES_H */