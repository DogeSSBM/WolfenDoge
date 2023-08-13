#ifndef DOGELIB_COORDS_H
#define DOGELIB_COORDS_H

Direction dirROR(const Direction dir)
{
    return (dir+1)&3u;
}

Direction dirROL(const Direction dir)
{
    return (dir-1)&3u;
}

Direction dirINV(const Direction dir)
{
    return dir^2u;
}

Direction dirLR(const Direction dir)
{
    return dir&1;
}

Direction dirUD(const Direction dir)
{
    return !(dir&1);
}

bool dirAXA(const Direction dir1, const Direction dir2)
{
    return dirUD(dir1) == dirUD(dir2);
}

Direction dirPOS(const Direction dir)
{
    return dir==DIR_R||dir==DIR_D;
}

Direction dirNEG(const Direction dir)
{
    return dir==DIR_L||dir==DIR_U;
}

int coordMin(const Coord coord)
{
    return coord.c1 < coord.c2 ? coord.c1 : coord.c2;
}

Coord coordLeast(const Coord pos1, const Coord pos2)
{
    return (const Coord){.x = imin(pos1.x, pos2.x), .y = imin(pos1.y, pos2.y)};
}

int coordMax(const Coord coord)
{
    return coord.c1 > coord.c2 ? coord.c1 : coord.c2;
}

Coord coordMost(const Coord pos1, const Coord pos2)
{
    return (const Coord){.x = imax(pos1.x, pos2.x), .y = imax(pos1.y, pos2.y)};
}

Coord coordMaxi(const Coord pos, const int i)
{
    return (const Coord){.x=imax(pos.x, i), .y=imax(pos.y, i)};
}

Coord coordMini(const Coord pos, const int i)
{
    return (const Coord){.x=imin(pos.x, i), .y=imin(pos.y, i)};
}

bool coordSame(const Coord pos1, const Coord pos2)
{
    return pos1.x == pos2.x && pos1.y == pos2.y;
}

Coord coordAbs(const Coord pos)
{
    return (const Coord){.x=pos.x<0?-pos.x:pos.x, .y=pos.y<0?-pos.y:pos.y};
}

int clamp(const int n, const int min, const int max)
{
    if(n < min)
        return min;
    if(n >= max)
        return max-1;
    return n;
}

bool inBound(const int n, const int min, const int max)
{
    return !(n < min || n >= max);
}

bool inRange(const int n, const Range range)
{
    return n >= range.min && n < range.max;
}

int wrap(const int n, const int min, const int max)
{
    const uint size = max-min;
    if(n < min)
        return max-abs(n);
    if(n >= max)
        return min+(n%size);
    return n;
}

// Returns true if both axies of coord are in range
bool coordInRange(const Coord coord, const Range range)
{
    return inRange(coord.x, range) && inRange(coord.y, range);
}

// Clamps both x and y component of coord to >= 0 and < their respective component in len
Coord coordClampLen(const Coord coord, const Length len)
{
    return (const Coord){.x = clamp(coord.x, 0, len.x), .y = clamp(coord.y, 0, len.y)};
}

// Returns true if the x and y component of coord are >= 0 and < their respective component in len
bool coordInLen(const Coord coord, const Length len)
{
    return coord.x >= 0 && coord.x < len.x && coord.y >= 0 && coord.y < len.y;
}

// Returns true if each part of pos1 is less than their counterpart in pos2
bool coordMaxCoord(const Coord pos1, const Coord pos2)
{
    return pos1.x < pos2.x && pos1.y < pos2.y;
}

// Returns true if each part of pos1 is greater than or equal to their counterpart in pos2
bool coordMinCoord(const Coord pos1, const Coord pos2)
{
    return pos1.x >= pos2.x && pos1.y >= pos2.y;
}

// takes 2 opposite corner coords and returns rect
Rect coordsToRect(const Coord coord1, const Coord coord2)
{
    return (const Rect){
        .x = coord1.x,
        .y = coord1.y,
        .w = coord2.x - coord1.x,
        .h = coord2.y - coord1.y
    };
}

bool coordNz(const Coord coord)
{
    return coord.x||coord.y;
}

Coord coordMuli(const Coord coord, const int num)
{
    return (const Coord){.x = coord.x*num, .y = coord.y*num};
}

Coord CoordMul(const Coord coord1, const Coord coord2)
{
    return (const Coord){.x = coord1.x*coord2.x, .y = coord1.y*coord2.y};
}

Coord coordInv(const Coord coord)
{
    return (const Coord){.x = -coord.x, .y = -coord.y};
}

Coord coordAddi(const Coord coord1, const int num)
{
    return (const Coord){.x = coord1.x+num, .y = coord1.y+num};
}

Coord coordSub(const Coord coord1, const Coord coord2)
{
    return (const Offset){.x = coord1.x-coord2.x, .y = coord1.y-coord2.y};
}

Coord coordMod(const Coord coord1, const Coord coord2)
{
    return (const Coord){.x = coord1.x%coord2.x, .y = coord1.y%coord2.y};
}

Coord coordModi(const Coord coord, const int num)
{
    return (const Coord){.x = coord.x%num, .y = coord.y%num};
}

Coord coordMid(const Coord coord1, const Coord coord2)
{
    return (const Coord){.x = (coord1.x+coord2.x)/2, .y = (coord1.y+coord2.y)/2};
}

uint coordDistSq(const Coord coord1, const Coord coord2)
{
    Coord distvec = coordSub(coord2, coord1);
    distvec = CoordMul(distvec, distvec);
    return distvec.x + distvec.y;
}

Coord coordDivi(const Coord coord, const int num)
{
    return (const Coord){.x = num ? coord.x / num : 0, .y = num ? coord.y / num : 0};
}

Coord coordDiv(const Coord coord1, const Coord coord2)
{
    return (const Coord){
        .x = coord2.x ? coord1.x/coord2.x : 0,
        .y = coord2.y ? coord1.y/coord2.y : 0
    };
}

Coord coordWrap(const Coord coord, const Range x, const Range y)
{
    return (const Coord){
        .x = wrap(coord.x, x.min, x.max),
        .y = wrap(coord.y, y.min, y.max)
    };
}

Coord coordClamp(const Coord coord, const Range x, const Range y)
{
    return (const Coord){
        .x = clamp(coord.x, x.min, x.max),
        .y = clamp(coord.y, y.min, y.max)
    };
}

Coord coordShift(const Coord coord, const Direction dir, const int units)
{
    if(dirUD(dir))
        return (const Coord){.x = coord.x, .y = dirPOS(dir)? coord.y+units : coord.y-units};
    return (const Coord){.x = dirPOS(dir)? coord.x+units : coord.x-units, .y = coord.y};
}

Coord coordAdd(const Coord coord, const Offset off)
{
    return (const Coord){.x = coord.x+off.x, .y = coord.y+off.y};
}

// offsets the coord by 1/2 the length
Coord coordCenter(const Coord coord, const Length len)
{
    return coordAdd(coord, coordDivi(len, 2));
}

// offsets the coord by -1/2 the length
Coord coordUncenter(const Coord coord, const Length len)
{
    return coordAdd(coord, coordDivi(len, -2));
}

// scales and offsets pos
Coord coordOffScale(const Coord pos, const Offset off, const int scale)
{
    return coordAdd(coordMuli(pos, scale), off);
}

// returns rect at pos with length len
Rect rectify(const Coord pos, const Length len)
{
    return (const Rect){
        .x = pos.x,
        .y = pos.y,
        .w = len.x,
        .h = len.y
    };
}

// offsets rect pos by 1/2 its length
Rect rectCenter(Rect r)
{
    const Length len = irL(r);
    return rectify(coordCenter(irC(r), len), len);
}

// returns rect pos offset by len of rect
Length unrectify(const Rect rect)
{
    return coordAdd(irC(rect), irL(rect));
}

// Returns true if coord is in rect
bool coordInRect(const Coord coord, const Rect rect)
{
    return inBound(coord.x, rect.x, rect.x+rect.w) && inBound(coord.y, rect.y, rect.y+rect.h);
}

// returns index of first rect that coord can be found in or -1 if in none
int coordInRectArr(const Coord coord, Rect *const rect, const int num)
{
    if(num < 1)
        return -1;
    for(int i = 0; i < num; i++)
        if(coordInRect(coord, rect[i]))
            return i;
    return -1;
}

Rect rectOffset(const Rect rect, const Offset off)
{
    const Coord pos = coordAdd((const Coord){.x = rect.x, .y = rect.y}, off);
    return (Rect){.x = pos.x, .y = pos.y, .w = rect.w, .h = rect.h};
}

#endif /* end of include guard: DOGELIB_COORDS_H */
