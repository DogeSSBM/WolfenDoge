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

// creates a new segment with type S_END
// (for denoting end of segment portion of map when saving / reading to file)
Seg* segEndNew(void)
{
    Seg *end = calloc(1, sizeof(Seg));
    end->type = S_END;
    return end;
}

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
            return trigNew(WHITE, a, b, 0, cfAddf(a, 50.0f), cfAddf(b, 50.0f));
            break;
        case S_CONV:
            return convNew(WHITE, a, b, 0, 0);
            break;
        default:
            break;
    }
    panic("???");
    return NULL;
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

// searches for del in segList, removes and frees it
Seg* segDelete(Seg *segList, Seg *del)
{
    if(!del)
        return segList;
    if(!segList)
        return NULL;
    if(del == segList){
        Seg *next = segList->next;
        free(segList);
        return next;
    }
    Seg *cur = segList;
    while(cur && cur->next != del)
        cur = cur->next;
    if(!cur)
        return segList;
    Seg *next = cur->next->next;
    free(cur->next);
    cur->next = next;
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

// frees all segments in segList
Seg* segListFree(Seg *segList)
{
    while(segList)
        segList = segFree(segList);
    return NULL;
}

#endif /* end of include guard: SEGMENTS_H */
