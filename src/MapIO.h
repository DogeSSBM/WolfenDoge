#ifndef MAPIO_H
#define MAPIO_H

// gets length of map name, allocates memory for it, parses it
// returns its strlen
st mapParseName(Map *map)
{
    assertExpr(map->file);
    int c = ' ';
    st len = 0;
    while((c = fgetc(map->file)) != '\n' && c != EOF)
        len++;
    assertExpr(len && c != EOF);

    map->name = calloc(len+1, sizeof(char));
    rewind(map->file);
    for(st i = 0; i < len; i++)
        map->name[i] = fgetc(map->file);

    assertExpr(fgetc(map->file) == '\n');
    return len;
}

// resets player health / speed
// sets player pos / ang to random spawn from map->obj[O_SPAWN]
// if there are no spawn objs, places at random ang and pos in map bounding box
void mapSpawnPlayer(Map *map)
{
    assertExpr(map);
    map->player.speed = 0;
    map->player.health = map->player.maxHealth;
    if(!map->obj[O_SPAWN]){
        const Coordf maxBound = mapSegBoundMax(map->seg);
        const Coordf minBound = mapSegBoundMin(map->seg);
        const Coordf lenBound = cfSub(maxBound, minBound);
        map->player.pos = fC(minBound.x + randRange(0.0f, lenBound.x), minBound.y + randRange(0.0f, lenBound.y));
        map->player.ang = rand() % 360;
        return;
    }

    const st index = rand() % objListLen(map->obj[O_SPAWN]);
    Obj *obj = map->obj[O_SPAWN];
    for(st i = 0; i < index; i++){
        obj = obj->next;
        assertExpr(obj);
    }
    map->player.pos = obj->pos;
    map->player.ang = obj->spawn.ang;
}

// parses a list of all segments from map file
Seg* mapParseAllSegments(Map *map)
{
    assertExpr(map && map->file);
    Seg *segList = NULL;
    st len = 0;
    while(1){
        Seg *seg = calloc(1, sizeof(Seg));
        assertExpr(fread(seg, sizeof(Seg), 1, map->file) == 1);
        if(seg->type == S_END){
            free(seg);
            break;
        }
        segList = segAppend(segList, seg);
        len++;
    }
    printf("Total segments: %zu\n", len);
    return segList;
}

// takes list of all map segments (ordered by SegType)
// splits list up into seperate lists for each SegType in map->seg[type]
void mapSortSegments(Map *map, Seg *segList)
{
    for(SegType type = 0; type < S_N; type++){
        if(!segList)
            return;
        if(segList->type != type)
            continue;
        map->seg[type] = segList;
        while(segList && segList->type == type){
            if(segList->next->type == type){
                segList = segList->next;
            }else{
                Seg *next = segList->next;
                segList->next = NULL;
                segList = next;
            }
        }
    }
}

// parses map segments into their respective SegType index
void mapParseSegments(Map *map)
{
    mapSortSegments(map, mapParseAllSegments(map));
    for(SegType type = 0; type < S_N; type++)
        printf("%s: %zu\n", SegTypeStr[type], segListLen(map->seg[type]));
}

// parses a list of all objects from map file
Obj* mapParseAllObjects(Map *map)
{
    assertExpr(map && map->file);
    Obj *objList = NULL;
    st len = 0;
    while(!feof(map->file)){
        Obj *obj = calloc(1, sizeof(Obj));
        objList = objAppend(objList, obj);
        len++;
    }
    printf("Total objments: %zu\n", len);
    return objList;
}

// takes list of all map segments (ordered by SegType)
// splits list up into seperate lists for each SegType in map->seg[type]
void mapSortObjects(Map *map, Obj *objList)
{
    for(ObjType type = 0; type < O_N; type++){
        if(!objList)
            return;
        if(objList->type != type)
            continue;
        map->obj[type] = objList;
        while(objList && objList->type == type){
            if(objList->next->type == type){
                objList = objList->next;
            }else{
                Obj *next = objList->next;
                objList->next = NULL;
                objList = next;
            }
        }
    }
}

// parses map objects into their respective ObjType index
void mapParseObjects(Map *map)
{
    mapSortObjects(map, mapParseAllObjects(map));
    for(ObjType type = 0; type < O_N; type++)
        printf("%s: %zu\n", ObjTypeStr[type], objListLen(map->obj[type]));
}

// parses map file
void mapParseFile(Map *map)
{
    assertExpr(map && map->file);
    assertExpr(mapParseName(map) > 0);
    mapParseSegments(map);
    mapParseObjects(map);
    fclose(map->file);
}

// attempts to open ../Maps/map.bork then ../Maps/map(n).bork with n starting at 1 and increasing
// once a file name that doesnt exist is found returns the path
char* newMapFileNum(void)
{
    uint n = 0;
    File *file = NULL;
    char path[256] = "../Maps/map.bork";
    while((file = fopen(path, "rb")) != NULL){
        fclose(file);
        n++;
        sprintf(path, "../Maps/map(%u).bork", n);
    }
    printf("No map path supplied, this map will be saved to \"%s\"\n", path);
    return strdup(path);
}

// sets seg's texture to NULL, checks to see if any other segments in map have
// same texture, if none do, the texture is freed
Seg* wallListTxtrCleanup(Seg *wallList, Seg *seg)
{
    if(!wallList || !seg || !seg->wall.texture)
        return wallList;
    assertExpr(seg->type == S_WALL);
    Texture *txtr = seg->wall.texture;
    seg->wall.texture = NULL;
    Seg *cur = wallList;
    while(cur){
        assertExpr(cur->type == S_WALL);
        if(cur->wall.texture == txtr)
            return wallList;
        cur = cur->next;
    }
    textureFree(txtr);
    return wallList;
}

// applys a texture to all wall segments that match path
Seg* wallListTxtrApply(Seg *wallList, Texture *txtr, char *path)
{
    if(!wallList || !path)
        return wallList;
    Seg *cur = wallList;
    while(cur){
        assertExpr(cur->type == S_WALL);
        if(!strcmp(wallList->wall.path, path))
            wallList->wall.texture = txtr;
        cur = cur->next;
    }
    return wallList;
}

// returns already loaded texture with matching path if exists
// else returns newly allocated texture
Texture* wallListTxtrQryLoad(Seg *wallList, char *path)
{
    while(wallList){
        assertExpr(wallList->type == S_WALL);
        if(wallList->wall.texture && !strcmp(wallList->wall.path, path)){
            printf("Found texture: \"%s\"\n", path);
            return wallList->wall.texture;
        }
        wallList = wallList->next;
    }
    printf("Loading texture: \"%s\"\n", path);
    return loadTexture(path);
}

// Loads default map segments
void mapDefaultSegments(Map *map)
{
    assertExpr(map);
    // Walls
    map->seg[S_WALL] = segAppend(map->seg[S_WALL], wallNew(GREEN,
        fC(  0.0f,   0.0f), fC(750.0f,   0.0f)
    ));
    map->seg[S_WALL] = segAppend(map->seg[S_WALL], wallNew(MAGENTA,
        fC(750.0f,   0.0f), fC(750.0f, 750.0f)
    ));
    map->seg[S_WALL] = segAppend(map->seg[S_WALL], wallNew(MAGENTA,
        fC(  0.0f,   0.0f), fC(  0.0f, 750.0f)
    ));
    map->seg[S_WALL] = segAppend(map->seg[S_WALL], wallNew(GREEN,
        fC(  0.0f, 750.0f), fC(750.0f, 750.0f)
    ));

    // Textured walls
    map->seg[S_WALL] = segAppend(map->seg[S_WALL], txtrNew(map->seg[S_WALL], WHITE,
        fC(250.0f, 500.0f), fC(500.0f, 500.0f), "./Assets/Bricks.png"
    ));

    // Windows
    map->seg[S_WIND] = segAppend(map->seg[S_WIND], windNew(RED, BLUE,
        fC(250.0f, 250.0f), fC(250.0f, 500.0f), .25f, .25f
    ));
    map->seg[S_WIND] = segAppend(map->seg[S_WIND], windNew(RED, BLUE,
        fC(500.0f, 250.0f), fC(500.0f, 500.0f), .25f, .25f
    ));

    // Doors
    map->seg[S_DOOR] = segAppend(map->seg[S_DOOR], doorNew(YELLOW,
        fC(250.0f, 250.0f), fC(500.0f, 250.0f), 0, 0.0f, false, 0.01f, DIR_D
    ));

    // Trigger Zones
    map->seg[S_TRIG] = segAppend(map->seg[S_TRIG], trigNew(YELLOW,
        fC(250.0f,   0.0f), fC(500.0f,   0.0f), 0,
        fC(250.0f, 250.0f), fC(500.0f, 250.0f)
    ));
    map->seg[S_TRIG] = segAppend(map->seg[S_TRIG], trigNew(YELLOW,
        fC(250.0f, 250.0f), fC(500.0f, 250.0f), 0,
        fC(250.0f, 500.0f), fC(500.0f, 500.0f)
    ));
}

// loads default map objects
void mapDefaultObjects(Map *map)
{
    assertExpr(map);
    map->obj[O_SPAWN] = spawnNew(fC(125.0f, 125.0f), 0);
}

void mapDefault(Map *map)
{
    mapDefaultSegments(map);
    mapDefaultObjects(map);
}

// attempts to load map file at mapPathArg if present
// if not present, sets map.path to ../Maps/map.bork or ../Maps/map(n).bork
// starting at n=1 and increasing until unique file path is found
Map mapLoad(char *mapPathArg)
{
    Map map = {0};
    map.path = mapPathArg ? strdup(mapPathArg) : newMapFileNum();
    if((map.file = fopen(map.path, "rb")))
        mapParseFile(&map);
    else
        mapDefault(&map);
    mapSpawnPlayer(&map);
    return map;

}

// saves map to map->path
void mapSave(Map *map)
{
    assertExpr(map && map->path && map->name);
    if(map->file)
        fclose(map->file);
    assertExpr((map->file = fopen(map->path, "wb")));
    const st nameLen = strlen(map->name);
    assertExpr(fwrite(map->name, sizeof(char), nameLen, map->file) == nameLen);
    fputc('\n', map->file);
    for(SegType type = 0; type < S_N; type++){
        Seg *seg = map->seg[type];
        while(seg){
            assertExpr(fwrite(seg, sizeof(Seg), 1, map->file) == 1);
            seg = seg->next;
        }
    }
    Seg seg = {.type = S_END};
    assertExpr(fwrite(&seg, sizeof(Seg), 1, map->file) == 1);

    for(ObjType type = 0; type < O_N; type++){
        Obj *obj = map->obj[type];
        while(obj){
            assertExpr(fwrite(obj, sizeof(Obj), 1, map->file) == 1);
            obj = obj->next;
        }
    }
    fclose(map->file);
}

#endif /* end of include guard: MAPIO_H */
