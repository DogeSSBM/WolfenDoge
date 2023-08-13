#ifndef MAPEDITORSEGMENTS_H
#define MAPEDITORSEGMENTS_H

// creates a new segment with type S_WALL
Seg* wallNew(const Color c, const Coordf a, const Coordf b)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    return w;
}

// creates a new segment with type S_WALL that has a texture
Seg* txtrNew(Seg *map, const Color c, const Coordf a, const Coordf b, char *path)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_WALL;
    w->color = c;
    w->a = a;
    w->b = b;
    const st len = strlen(path);
    assertExpr(len < 128);
    memcpy(w->wall.path, path, len);
    w->wall.texture = txtrQryLoad(map, path);
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
Seg* trigNew(const Color color, const Coordf a, const Coordf b, const uint id, const Coordf c, const Coordf d)
{
    Seg *w = wallNew(color, a, b);
    w->type = S_TRIG;
    w->trig.id = id;
    w->trig.c = c;
    w->trig.d = d;
    return w;
}

// creates a new segment with type S_CONV
Seg* convNew(const Color c, const Coordf a, const Coordf b, const uint idA, const uint idB)
{
    Seg *w = calloc(1, sizeof(Seg));
    w->type = S_CONV;
    w->color = c;
    w->a = a;
    w->b = b;
    w->conv.idA = idA;
    w->conv.idB = idB;
    return w;
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

// searches for del in map list, removes and frees it
Seg* segDelete(Seg *map, Seg *del)
{
    if(!del)
        return map;
    if(!map)
        return NULL;
    if(del == map){
        Seg *next = map->next;
        free(map);
        return next;
    }
    Seg *cur = map;
    while(cur && cur->next != del)
        cur = cur->next;
    if(!cur)
        return map;
    Seg *next = cur->next->next;
    free(cur->next);
    cur->next = next;
    return map;
}

// returns the number of segments in the map list
uint segListLen(Seg *map)
{
    uint len = 0;
    while(map){
        len++;
        map = map->next;
    }
    return len;
}

// frees all segments in the list
void segListFree(Seg *list)
{
    while(list){
        Seg *next = list->next;
        free(list);
        list = next;
    }
}

#endif /* end of include guard: MAPEDITORSEGMENTS_H */
