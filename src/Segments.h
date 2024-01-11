#ifndef SEGMENTS_H
#define SEGMENTS_H

// creates a new segment with type S_WALL
Seg* wallNew(const Color c, const Coordf a, const Coordf b)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    memset(w->wall.path, '\0', sizeof(w->wall.path));
    return w;
}

// creates a new segment with type S_WALL that has a texture
Seg* txtrNew(const Color c, const Coordf a, const Coordf b, char *path)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    const st len = strlen(path);
    assertExpr(len < 127);
    memcpy(w->wall.path, path, len);
    w->wall.texture = loadTexture(path);
    printf("loaded texture :\"%s\"\n", w->wall.path);
    // w->wall.texture = wallListTxtrQryLoad(wallList, path);
    return w;
}

// creates a new segment with type S_WIND
Seg* windNew(const Color c, const Color topColor, const Coordf a, const Coordf b, const float height, const float top)
{
    Seg *w = wallNew(c, a, b);
    w->type = S_WIND;
    w->wind.topColor = topColor;
    w->wind.height = height;
    w->wind.top = top;
    return w;
}

// creates a new segment with type S_DOOR
Seg* doorNew(const Color c, const Coordf a, const Coordf b, const uint id, const float pos, const bool state, const float speed, const Direction closeDir)
{
    Seg *w = wallNew(c, a, b);
    w->type = S_DOOR;
    w->door.id = id;
    w->door.pos = pos;
    w->door.state = state;
    w->door.speed = speed;
    w->door.closeDir = closeDir;
    return w;
}

// creates a new segment with type S_TRIG
Seg* trigNew(const Color color, const TrigType type, const Coordf a, const Coordf b, const uint id, const Coordf c, const Coordf d)
{
    Seg *w = wallNew(color, a, b);
    w->type = S_TRIG;
    w->trig.type = type;
    w->trig.id = id;
    w->trig.c = c;
    w->trig.d = d;
    w->trig.start = false;
    w->trig.state = false;
    return w;
}

// creates a new segment with type S_PORT
Seg* portNew(const Coordf a, const Coordf b, const Coordf porta, const Coordf portb)
{
    Seg *w = wallNew(WHITE, a, b);
    w->type = S_PORT;
    w->port.a = porta;
    w->port.b = portb;
    return w;
}

// creates a new segment with type S_END
// (for denoting end of segment portion of map when saving / reading to file)
Seg* segEndNew(void)
{
    Seg *end = calloc(1, sizeof(Seg));
    end->type = S_END;
    return end;
}

// creates a new segment
Seg* segNew(const SegType type, const Coordf a, const Coordf b)
{
    switch(type){
        case S_WALL:
            return wallNew(WHITE, a, b);
            break;
        case S_WIND:
            return windNew(WHITE, WHITE, a, b, .25f, .25f);
            break;
        case S_DOOR:
            return doorNew(WHITE, a, b, 0, 0.0f, false, 0.01f, DIR_D);
            break;
        case S_TRIG:
            return trigNew(WHITE, T_ZONE, a, b, 0, cfAddf(a, 50.0f), cfAddf(b, 50.0f));
            break;
        case S_PORT:
            return portNew(a, b, cfAddf(a, 50.0f), cfAddf(b, 50.0f));
            break;
        default:
            break;
    }
    panic("???");
    return NULL;
}

// duplicates segment (deep copy)
Seg* segDup(Seg *seg)
{
    if(!seg)
        return NULL;
    Seg *dup = calloc(1, sizeof(Seg));
    memcpy(dup, seg, sizeof(Seg));
    if(seg->type == S_WALL && seg->wall.path[0] != '\0')
        dup->wall.texture = loadTexture(dup->wall.path);
    dup->next = NULL;
    return dup;
}

// appends tail to the end of the list (head)
Seg* segAppend(Seg *head, Seg *tail)
{
    if(!head)
        return tail;
    Seg *cur = head;
    while(cur->next)
        cur = cur->next;
    cur->next = tail;
    return head;
}

// frees segment (deep free)
Seg* segFree(Seg *seg)
{
    if(!seg)
        return NULL;
    Seg *next = seg->next;
    if(seg->type == S_WALL && seg->wall.texture)
        textureFree(seg->wall.texture);
    free(seg);
    return next;
}

// searches for del in segList, removes and frees (deep)
Seg* segDelete(Seg *segList, Seg *del)
{
    if(!del)
        return segList;
    if(!segList)
        return NULL;
    if(del == segList)
        return segFree(segList);
    Seg *cur = segList;
    while(cur && cur->next != del)
        cur = cur->next;
    if(cur)
        cur->next = segFree(cur->next);
    return segList;
}

// returns the number of segments in the list
st segListLen(Seg *segList)
{
    st len = 0;
    while(segList){
        len++;
        segList = segList->next;
    }
    return len;
}

// frees all segments in segList
Seg* segListFree(Seg *segList)
{
    while(segList)
        segList = segFree(segList);
    return NULL;
}

// honestly idk
float triSign(const Coordf a, const Coordf b, const Coordf c)
{
    return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}

// returns true if pos is within triangle abc
bool cfInTri(const Coordf pos, const Coordf a, const Coordf b, const Coordf c)
{
    const float s1 = triSign(pos, a, b);
    const float s2 = triSign(pos, b, c);
    const float s3 = triSign(pos, c, a);
    return !(((s1 < 0) || (s2 < 0) || (s3 < 0)) && ((s1 > 0) || (s2 > 0) || (s3 > 0)));
}

// returns true if pos is within quad
bool cfInQuad(const Coordf pos, const Coordf a, const Coordf b, const Coordf c, const Coordf d)
{
    return (
        cfInTri(pos, a, b, d) ||
        cfInTri(pos, b, d, c) ||
        cfInTri(pos, a, b, c) ||
        cfInTri(pos, a, d, c)
    );
}

// returns true if pos in trigger zone
bool cfInTrig(const Coordf pos, Seg *trig)
{
    assertExpr(trig && trig->type == S_TRIG);
    return cfInQuad(pos, trig->a, trig->b, trig->trig.c, trig->trig.d);
}

// returns first available trigger in map if cur is NULL
// returns first trigger after cur if cur is not NULL
// returns NULL if none left
Seg* trigQueryId(Map *map, Seg *cur, const uint id)
{
    assertExpr(map);
    if(cur){
        assertExpr(cur->type == S_TRIG);
        cur = cur->next;
    }
    while(cur){
        if(cur->trig.id == id)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

#endif /* end of include guard: SEGMENTS_H */
