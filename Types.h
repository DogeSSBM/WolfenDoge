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

typedef enum{W_WALL, W_WIND, W_TRIG, W_DOOR, W_N}SegType;
char *SegTypeStr[W_N] = {"W_WALL", "W_WIND", "W_TRIG", "W_DOOR"};
typedef struct Seg{
    SegType type;
    Coordf a;
    Coordf b;
    Color color;
    union{
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
    };
    struct Seg *next;
}Seg;

typedef struct{
    Seg *wall;
    float dst;
    Coordf pos;
}Ray;

typedef struct{
    Color c;
    Coordf a;
    Coordf b;
}WallPacked;

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
    bool showInfo;
    Seg *wall;
    Coordf posOrig;
    Coordf *pos;
    struct Selection *next;
    Coord cursor;
    uint tscale;
}Selection;

#endif /* end of include guard: TYPES_H */
