#ifndef MAP_H
#define MAP_H

// loads the map located at filePath and returns it as a list of map segments
Seg* mapLoad(char *filePath)
{
    File *file = NULL;
    assertExpr(filePath);
    if((file = fopen(filePath, "rb")) == NULL){
        printf("Couldn't open file \"%s\"\n", filePath);
        return NULL;
    }
    SegPacked packed = {0};
    fread(&packed.len, sizeof(uint), 1, file);
    printf("map file len: %u\n", packed.len);
    if(feof(file) || packed.len == 0){
        printf("Error reading len of map in file \"%s\"\n", filePath);
        fclose(file);
        return NULL;
    }
    packed.seg = calloc(packed.len, sizeof(Seg));
    fread(packed.seg, sizeof(Seg), packed.len, file);
    if(fgetc(file) != EOF || !feof(file))
        printf("Not at end of file after reading expected len (%u)\n", packed.len);
    printf("finished reading packed map\n");
    Seg* map = mapUnpack(packed);
    free(packed.seg);
    printf("free'd packed map\n");
    return map;
}

// returns max map bounds
Coordf mapBoundMax(Seg *map)
{
    Coordf bound = {0};
    while(map)
    {
        bound.x = fmost(bound.x, map->a.x);
        bound.x = fmost(bound.x, map->b.x);
        bound.y = fmost(bound.y, map->a.y);
        bound.y = fmost(bound.y, map->b.y);
        if(map->type == S_TRIG){
            bound.x = fmost(bound.x, map->trig.c.x);
            bound.x = fmost(bound.x, map->trig.d.x);
            bound.y = fmost(bound.y, map->trig.c.y);
            bound.y = fmost(bound.y, map->trig.d.y);
        }
        map = map->next;
    }
    return bound;
}

// returns min map bounds
Coordf mapBoundMin(Seg *map)
{
    Coordf bound = {0};
    while(map)
    {
        bound.x = fleast(bound.x, map->a.x);
        bound.x = fleast(bound.x, map->b.x);
        bound.y = fleast(bound.y, map->a.y);
        bound.y = fleast(bound.y, map->b.y);
        if(map->type == S_TRIG){
            bound.x = fleast(bound.x, map->trig.c.x);
            bound.x = fleast(bound.x, map->trig.d.x);
            bound.y = fleast(bound.y, map->trig.c.y);
            bound.y = fleast(bound.y, map->trig.d.y);
        }
        map = map->next;
    }
    return bound;
}

// returns length of map
Coordf mapLength(Seg *map)
{
    return cfSub(mapBoundMax(map), mapBoundMin(map));
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

// returns the first segment in the list that contains an id equal to id
Seg* mapQueryObjId(Seg *map, const uint id)
{
    while(map){
        if(
            (map->type == S_DOOR && map->door.id == id) ||
            (map->type == S_TRIG && map->trig.id == id) ||
            (map->type == S_CONV && (map->conv.idA == id || map->conv.idB == id))
        )
            break;
        map = map->next;
    }
    return map;
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

// sets map segments whos id equals id with state equal to state
Seg* mapUpdateIdState(Seg *map, const uint id, const bool state)
{
    Seg *cur = map;
    while(cur){
        if(cur->type == S_DOOR && cur->door.id == id)
            cur->door.state = state;
        cur = cur->next;
    }
    return map;
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
Update* mapQueueUpdates(const Coordf oldPos, const Coordf newPos, Seg *map)
{
    Seg *cur = map;
    Update *up = NULL;
    while(cur){
        if(cur->type == S_TRIG){
            const bool oldState = cfInQuad(oldPos, cur->a, cur->b, cur->trig.c, cur->trig.d);
            const bool newState = cfInQuad(newPos, cur->a, cur->b, cur->trig.c, cur->trig.d);
            if(oldState != newState){
                up = upUpdate(up, cur->trig.id, newState);
            }
        }
        cur = cur->next;
    }
    return up;
}

// applys and frees all updates in list to map
Seg* mapApplyUpdates(Seg *map, Update *up)
{
    while(up){
        Update *next = up->next;
        map = mapUpdateIdState(map, up->id, up->state);
        free(up);
        up = next;
    }
    return map;
}

// updates the dynamic segments of the map
void mapUpdateDynamics(Seg *map)
{
    while(map){
        if(map->type == S_DOOR){
            map->door.pos += map->door.state ? -map->door.speed : map->door.speed;
            if(map->door.pos < 0.0f)
                map->door.pos = 0.0f;
            if(map->door.pos > 1.0f)
                map->door.pos = 1.0f;
        }
        map = map->next;
    }
}

#endif /* end of include guard: MAP_H */
