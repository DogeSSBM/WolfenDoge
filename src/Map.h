#ifndef MAP_H
#define MAP_H

// returns max coord of segList bounding box
Coordf segListBoundMax(Seg *segList)
{
    Coordf bound = {0};
    while(segList)
    {
        bound.x = fmost(bound.x, segList->a.x);
        bound.x = fmost(bound.x, segList->b.x);
        bound.y = fmost(bound.y, segList->a.y);
        bound.y = fmost(bound.y, segList->b.y);
        if(segList->type == S_TRIG){
            bound.x = fmost(bound.x, segList->trig.c.x);
            bound.x = fmost(bound.x, segList->trig.d.x);
            bound.y = fmost(bound.y, segList->trig.c.y);
            bound.y = fmost(bound.y, segList->trig.d.y);
        }
        segList = segList->next;
    }
    return bound;
}

// returns max coord of map segments bounding box
Coordf mapSegBoundMax(Seg *seg[S_N])
{
    Coordf maxBound = {0};
    for(SegType type = 0; type < S_N; type++){
        if(seg[type])
            maxBound = cfMost(maxBound, segListBoundMax(seg[type]));
    }
    return maxBound;
}

// returns min coord of segList bounding box
Coordf segListBoundMin(Seg *segList)
{
    Coordf bound = {0};
    while(segList)
    {
        bound.x = fleast(bound.x, segList->a.x);
        bound.x = fleast(bound.x, segList->b.x);
        bound.y = fleast(bound.y, segList->a.y);
        bound.y = fleast(bound.y, segList->b.y);
        if(segList->type == S_TRIG){
            bound.x = fleast(bound.x, segList->trig.c.x);
            bound.x = fleast(bound.x, segList->trig.d.x);
            bound.y = fleast(bound.y, segList->trig.c.y);
            bound.y = fleast(bound.y, segList->trig.d.y);
        }
        segList = segList->next;
    }
    return bound;
}

// returns min coord of map segments bounding box
Coordf mapSegBoundMin(Seg *seg[S_N])
{
    Coordf minBound = {0};
    for(SegType type = 0; type < S_N; type++){
        if(seg[type])
            minBound = cfLeast(minBound, segListBoundMin(seg[type]));
    }
    return minBound;
}

// returns length of map segments bounding box
Coordf mapSegBoundLen(Seg *seg[S_N])
{
    return cfSub(mapSegBoundMax(seg), mapSegBoundMin(seg));
}

// converts a coordinate relative to the window to a map coordinate
Coordf screenToMap(const Coord off, const float scale, const Coord pos)
{
    return cfMulf(CCf(coordSub(pos, off)), scale);
}

// converts a map coordinate to a coordinate relative to the window
Coord mapToScreen(const Coord off, const float scale, const Coordf pos)
{
    return coordAdd(CfC(cfDivf(pos, scale)), off);
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

// true if segment type has an id and it matches id
bool segMatchId(Seg *seg, const uint id)
{
    return (seg->type == S_DOOR && seg->door.id == id) ||
    (seg->type == S_TRIG && seg->trig.id == id) ||
    (seg->type == S_CONV && (seg->conv.idA == id || seg->conv.idB == id));
}

// sets map segments whos id equals id with state equal to state
void mapUpdateIdState(Map *map, const uint id, const bool state)
{
    Seg *cur = map->seg[S_DOOR];
    while(cur){
        if(cur->type == S_DOOR && cur->door.id == id)
            cur->door.state = state;
        cur = cur->next;
    }
}

// returns update with corrosponding id or NULL if none found
Update* upQueryId(Update *up, const uint id)
{
    while(up && up->id != id)
        up = up->next;
    return up;
}

// appends update to list
Update* upAppend(Update *head, Update *tail)
{
    if(!head)
        return tail;
    Update *cur = head;
    while(cur->next)
        cur = cur->next;
    cur->next = tail;
    return head;
}

// allocates and returns a new argument
Update* upNew(const uint id, const bool state)
{
    Update *up = calloc(1, sizeof(Update));
    up->id = id;
    up->state = state;
    return up;
}

// modifies an existing update if an update with id already exists, otherwise appends a new update
Update* upUpdate(Update *up, const uint id, const bool state)
{
    Update *existing = upQueryId(up, id);
    if(existing)
        existing->state |= state;
    else
        up = upAppend(up, upNew(id, state));
    return up;
}

// constructs and returns a list of updates that occured on the current frame
Update* mapQueueUpdates(const Coordf oldPos, const Coordf newPos, Map *map)
{
    Seg *cur = map->seg[S_TRIG];
    Update *up = NULL;
    while(cur){
        const bool oldState = cfInQuad(oldPos, cur->a, cur->b, cur->trig.c, cur->trig.d);
        const bool newState = cfInQuad(newPos, cur->a, cur->b, cur->trig.c, cur->trig.d);
        if(oldState != newState)
            up = upUpdate(up, cur->trig.id, newState);
        cur = cur->next;
    }
    cur = map->seg[S_CONV];
    while(cur){
        Update *src = upQueryId(up, cur->conv.idA);
        if(src)
            up = upUpdate(up, cur->conv.idB, src->state);
        cur = cur->next;
    }
    return up;
}

// applys and frees all updates in list to map
void mapApplyUpdates(Map *map, Update *up)
{
    while(up){
        Update *next = up->next;
        mapUpdateIdState(map, up->id, up->state);
        free(up);
        up = next;
    }
}

// updates the dynamic segments of the map
void mapUpdateDynamics(Map *map)
{
    Seg *cur = map->seg[S_DOOR];
    while(cur){
        cur->door.pos += cur->door.state ? -cur->door.speed : cur->door.speed;
        if(cur->door.pos < 0.0f)
            cur->door.pos = 0.0f;
        if(cur->door.pos > 1.0f)
            cur->door.pos = 1.0f;
        cur = cur->next;
    }
}

#endif /* end of include guard: MAP_H */
