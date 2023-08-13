#ifndef MAPIO_H
#define MAPIO_H

// searches for wall with texture path, if found returns the texture else returns
// newly allocated texture
Texture* txtrQryLoad(Seg *map, char *path)
{
    while(map){
        if(map->type == S_WALL && map->wall.path && map->wall.texture && !strcmp(map->wall.path, path))
            return map->wall.texture;
        map = map->next;
    }
    printf("Loading texture: \"%s\"\n", path);
    return loadTexture(path);
}

// sets seg's texture to NULL, checks to see if any other segments in map have
// same texture, if none do, the texture is freed
Seg* txtrCleanup(Seg *map, Seg *seg)
{
    if(!map || !seg || seg->type != S_WALL || !seg->wall.texture)
        return map;
    Texture *txtr = seg->wall.texture;
    seg->wall.texture = NULL;
    Seg *cur = map;
    while(cur){
        if(cur->type == S_WALL && cur->wall.texture == txtr)
            return map;
        cur = cur->next;
    }
    textureFree(txtr);
    return map;
}

// applys a texture to all segments that match path
Seg* txtrApply(Seg *map, Texture *txtr, char *path)
{
    if(!map || !path)
        return map;
    Seg *cur = map;
    while(cur){
        if(cur->type == S_WALL && cur->wall.path[0] != '\0' && !strcmp(map->wall.path, path))
            map->wall.texture = txtr;
        cur = cur->next;
    }
    return map;
}

// attempts to open map.bork then map(n).bork with n starting at 1 and increasing
// once a file name that doesnt exist is found returns n
uint newMapFileNum(void)
{
    uint n = 0;
    File *file = NULL;
    char path[256] = "../Maps/map.bork";
    while((file = fopen(path, "rb")) != NULL){
        fclose(file);
        n++;
        sprintf(path, "../Maps/map(%u).bork", n);
    }
    return n;
}

// saves map to path
void mapSave(Seg *map, char *path)
{
    if(!map){
        printf("Map is empty, skipping save\n");
        return;
    }
    assertExpr(path);
    File *file = NULL;
    file = fopen(path, "wb");
    SegPacked mapPacked = mapPack(map);
    printf("Writing map of length: %u to \"%s\"\n", mapPacked.len, path);
    fwrite(&mapPacked.len, sizeof(uint), 1, file);
    fwrite(mapPacked.seg, sizeof(Seg), mapPacked.len, file);
    fclose(file);
    free(mapPacked.seg);
}

// Serializes the list of segments into an array
SegPacked mapPack(Seg *map)
{
    const uint len = segListLen(map);
    if(len == 0)
        return (SegPacked){0};
    SegPacked mapPacked = {
        .len = len,
        .seg = calloc(len, sizeof(Seg))
    };
    for(uint i = 0; i < len; i++){
        mapPacked.seg[i] = *map;
        map = map->next;
    }
    return mapPacked;
}

// De-serializes the map segment array into a list
Seg* mapUnpack(SegPacked mapPacked)
{
    if(mapPacked.len == 0 || !mapPacked.seg)
        return NULL;
    Seg *map = NULL;
    for(uint i = 0; i < mapPacked.len; i++){
        printf("copying seg %2u/%2u\n", i, mapPacked.len);
        Seg *seg = calloc(1, sizeof(Seg));
        memcpy(seg, &mapPacked.seg[i], sizeof(Seg));
        seg->wall.texture = txtrQryLoad(map, seg->wall.path);
        seg->next = NULL;
        map = segAppend(map, seg);
    }
    return map;
}

// allocates and returns the default map
Seg* mapDefault(void)
{
    Seg *map =           wallNew(GREEN,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=  0.0f}
    );
    map = segAppend(map, wallNew(MAGENTA,
        (const Coordf){.x=750.0f, .y=  0.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = segAppend(map, wallNew(MAGENTA,
        (const Coordf){.x=  0.0f, .y=  0.0f},
        (const Coordf){.x=  0.0f, .y=750.0f}
    ));
    map = segAppend(map, wallNew(GREEN,
        (const Coordf){.x=  0.0f, .y=750.0f},
        (const Coordf){.x=750.0f, .y=750.0f}
    ));
    map = segAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=250.0f, .y=500.0f},
        .25f, .25f
    ));
    map = segAppend(map, windNew(RED, BLUE,
        (const Coordf){.x=500.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        .25f, .25f
    ));
    map = segAppend(map, txtrNew(map, WHITE,
        (const Coordf){.x=250.0f, .y=500.0f},
        (const Coordf){.x=500.0f, .y=500.0f},
        "./Assets/Bricks.png"
    ));
    map = segAppend(map, doorNew(YELLOW,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=250.0f},
        0, 0.0f, false, 0.01f, DIR_D
    ));
    map = segAppend(map, trigNew(YELLOW,
        (const Coordf){.x=250.0f, .y=0.0f},
        (const Coordf){.x=500.0f, .y=0.0f},
        0,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=250.0f}
    ));
    map = segAppend(map, trigNew(YELLOW,
        (const Coordf){.x=250.0f, .y=250.0f},
        (const Coordf){.x=500.0f, .y=250.0f},
        0,
        (const Coordf){.x=250.0f, .y=500.0f},
        (const Coordf){.x=500.0f, .y=500.0f}
    ));
    return map;
}

#endif /* end of include guard: MAPIO_H */
