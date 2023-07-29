#ifndef MAP_H
#define MAP_H

Seg* mapLoad(char *fileName)
{
    File *file = NULL;
    assertExpr(fileName);
    if((file = fopen(fileName, "rb")) == NULL){
        printf("Couldn't open file \"%s\"\n", fileName);
        return NULL;
    }
    uint len = 0;
    fread(&len, sizeof(uint), 1, file);
    if(feof(file) || len == 0){
        printf("Error reading len of map in file \"%s\"\n", fileName);
        fclose(file);
        return NULL;
    }
    WallPacked *mapPacked = calloc(len, sizeof(WallPacked));
    fread(mapPacked, sizeof(WallPacked), len, file);
    if(fgetc(file) != EOF || !feof(file))
        printf("Not at end of file after reading expected len (%u)\n", len);
    Seg* map = mapUnpack(mapPacked, len);
    free(mapPacked);
    return map;
}

Coordf mapBounds(Seg *map)
{
    Coordf bound = {0};
    while(map)
    {
        bound.x = fmost(bound.x, map->a.x);
        bound.x = fmost(bound.x, map->b.x);
        bound.y = fmost(bound.y, map->a.y);
        bound.y = fmost(bound.y, map->b.y);
        map = map->next;
    }
    return bound;
}

Coordf screenToMap(const Coord off, const float scale, const Coord pos)
{
    return cfMulf(CCf(coordSub(pos, off)), scale);
}

Coord mapToScreen(const Coord off, const float scale, const Coordf pos)
{
    return coordAdd(CfC(cfDivf(pos, scale)), off);
}

Seg* mapQueryObjId(Seg *map, const uint id)
{
    while(map){
        if(map->type == W_DOOR && map->door.id == id)
            break;
        map = map->next;
    }
    return map;
}

float triSign(const Coordf a, const Coordf b, const Coordf c)
{
    return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}

bool cfInTri(Coordf pos, Coordf a, Coordf b, Coordf c)
{
    const float s1 = triSign(pos, a, b);
    const float s2 = triSign(pos, b, c);
    const float s3 = triSign(pos, c, a);
    return !(((s1 < 0) || (s2 < 0) || (s3 < 0)) && ((s1 > 0) || (s2 > 0) || (s3 > 0)));
}

void mapUpdateIdState(Seg *map, const uint id, const bool state)
{
    while(map){
        if(map->type == W_DOOR && map->door.id == id)
            map->door.state = state;
        map = map->next;
    }
}

void mapUpdateTriggers(const Coordf oldPos, const Coordf newPos, Seg *map)
{
    Seg *cur = map;
    while(cur){
        if(cur->type == W_TRIG){
            const bool oldState = (
                cfInTri(oldPos, cur->a, cur->b, cur->trig.d) || cfInTri(oldPos, cur->b, cur->trig.d, cur->trig.c) ||
                cfInTri(oldPos, cur->a, cur->b, cur->trig.c) || cfInTri(oldPos, cur->a, cur->trig.d, cur->trig.c)
            );
            const bool newState = (
                cfInTri(newPos, cur->a, cur->b, cur->trig.d) || cfInTri(newPos, cur->b, cur->trig.d, cur->trig.c) ||
                cfInTri(newPos, cur->a, cur->b, cur->trig.c) || cfInTri(newPos, cur->a, cur->trig.d, cur->trig.c)
            );
            if(oldState != newState){
                mapUpdateIdState(map, cur->trig.id, newState);
            }
        }
        cur = cur->next;
    }
}

void mapUpdateDynamics(Seg *map)
{
    while(map){
        if(map->type == W_DOOR){
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
