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

// updates all doors with matching id to state
void mapDoorSet(Map *map, const uint id, const bool state)
{
    for(Seg *door = map->seg[S_DOOR]; door; door = door->next){
        if(door->door.id == id)
            door->door.state = state;
    }
}

// resets all door states to false
void mapDoorReset(Map *map)
{
    for(Seg *door = map->seg[S_DOOR]; door; door = door->next)
        door->door.state = false;
}

bool mapTrigQuery(Map *map, const uint id)
{
    Seg *cur = map->seg[S_TRIG];
    while(cur){
        if(cur->trig.id == id && cur->trig.state)
            return true;
        cur = cur->next;
    }
    return false;
}

void mapDoorUpdate(Map *map)
{
    Seg *cur = map->seg[S_DOOR];
    while(cur){
        cur->door.state = mapTrigQuery(map, cur->door.id);
        cur->door.pos += cur->door.state ? -cur->door.speed : cur->door.speed;
        if(cur->door.pos < 0.0f)
            cur->door.pos = 0.0f;
        if(cur->door.pos > 1.0f)
            cur->door.pos = 1.0f;
        cur = cur->next;
    }
}

void mapTrigUpdate(Map *map)
{
    Seg *cur = map->seg[S_TRIG];
    while(cur){
        switch(cur->trig.type){
            case T_FLIP:
                if(keyPressed(SC_E) && cfInTrig(map->player.pos, cur))
                    cur->trig.state = !cur->trig.state;
                break;
            case T_ZONE_ONCE:
                if(cur->trig.start == cur->trig.state && cfInTrig(map->player.pos, cur) != cur->trig.start)
                    cur->trig.state = !cur->trig.state;
                break;
            case T_FLIP_ONCE:
            if(
                keyPressed(SC_E) && cur->trig.start == cur->trig.state &&
                cfInTrig(map->player.pos, cur) != cur->trig.start
            ){
                cur->trig.state = !cur->trig.state;
            }
                break;
            case T_ZONE:
            default:
                cur->trig.state = cfInTrig(map->player.pos, cur);
                break;
        }

        cur = cur->next;
    }
}

void mapObjUpdate(Map *map)
{
    Obj *mob = map->obj[O_MOB];
    while(mob){
        const float hwidth = mob->mob.len.x/2.0f;
        const float angToPlayer = cfCfToDeg(mob->pos, map->player.pos);
        const float rang = degReduce(angToPlayer-90.0f);
        const float lang = degReduce(angToPlayer+90.0f);
        mob->mob.a = cfAdd(mob->pos, degMagToCf(lang, hwidth));
        mob->mob.b = cfAdd(mob->pos, degMagToCf(rang, hwidth));
        mob = mob->next;
    }
}

// updates the dynamic segments of the map
void mapUpdateDynamics(Map *map)
{
    assertExpr(map);
    mapTrigUpdate(map);
    mapDoorUpdate(map);
    mapObjUpdate(map);
}

// adds piece to map
void mapAddPiece(Map *map, const MapPiece piece)
{
    assertExpr(map && piece.type < M_ANY);
    if(piece.type == M_SEG){
        assertExpr(piece.seg);
        map->seg[piece.seg->type] = segAppend(map->seg[piece.seg->type], piece.seg);
        return;
    }
    assertExpr(piece.obj);
    map->obj[piece.obj->type] = objAppend(map->obj[piece.obj->type], piece.obj);
}

#endif /* end of include guard: MAP_H */
