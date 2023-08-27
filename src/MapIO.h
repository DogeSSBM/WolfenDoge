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

    if(map->name)
        free(map->name);
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

// returns already loaded texture with matching path if exists
// else returns newly allocated texture
Texture* mobListTxtrQryLoad(Obj *mobList, char *path)
{
    while(mobList){
        assertExpr(mobList->type == O_MOB);
        if(mobList->mob.texture && !strcmp(mobList->mob.path, path)){
            printf("Found texture: \"%s\"\n", path);
            return mobList->mob.texture;
        }
        mobList = mobList->next;
    }
    printf("Loading texture: \"%s\"\n", path);
    return loadTexture(path);
}

// parses map segments into their respective SegType index
void mapParseSegments(Map *map)
{
    printf("Parsing segments -\n");
    Seg *seg = NULL;
    do{
        seg = calloc(1, sizeof(Seg));
        assertExpr(fread(seg, sizeof(Seg), 1, map->file) == 1);
        seg->next = NULL;
        if(seg->type != S_END){
            map->seg[seg->type] = segAppend(map->seg[seg->type], seg);
            // printf("read a segment with type %s\n", SegTypeStr[seg->type]);
            if(seg->type == S_WALL && strlen(seg->wall.path)){
                seg->wall.texture = NULL;
                seg->wall.texture = wallListTxtrQryLoad(map->seg[S_WALL], seg->wall.path);
            }
        }
    }while(seg->type != S_END);
    free(seg);
    printf("\tDone!\n");
}

// parses map objects into their respective ObjType index
void mapParseObjects(Map *map)
{
    printf("Parsing objects -\n");
    Obj *obj = NULL;
    while(!feof(map->file)){
        obj = calloc(1, sizeof(Obj));
        if(fread(obj, sizeof(Obj), 1, map->file) == 1){
            if(obj->type == O_MOB){
                obj->mob.texture = NULL;
                obj->mob.texture = mobListTxtrQryLoad(map->obj[O_MOB], obj->mob.path);
            }
            obj->next = NULL;
            map->obj[obj->type] = objAppend(map->obj[obj->type], obj);
            // printf("read an object with type %s\n", ObjTypeStr[obj->type]);
        }else{
            assertExpr(feof(map->file));
            break;
        }
    }
    free(obj);
    printf("\tDone!\n");
}

// parses map file
void mapParseFile(Map *map)
{
    printf("Parsing map -\n\tName:\"%s\"\n\tPath: \"%s\"\n", map->name, map->path);
    assertExpr(map && map->file);
    assertExpr(mapParseName(map) > 0);
    // printf("parsed map name: \"%s\"\n", map->name);
    mapParseSegments(map);
    mapParseObjects(map);
    fclose(map->file);
    map->file = NULL;
}

// attempts to open ../Maps/map.bork then ../Maps/map(n).bork with n starting at 1 and increasing
// once a file name that doesnt exist is found sets map.name to it
void newMapFileNum(Map *map)
{
    uint n = 0;
    File *file = NULL;
    char name[128] = "map";
    char path[256] = "./Maps/map.bork";
    while(1){
        if(n > 0){
            sprintf(name, "map(%u)", n);
            sprintf(path, "./Maps/%s.bork", name);
        }
        if(!(file = fopen(path, "rb")))
            break;
        fclose(file);
        n++;
        sprintf(path, "./Maps/map(%u).bork", n);
    }

    printf("No map path supplied, this map will be named: \"%s\" and saved to: \"%s\"\n", name, path);
    map->name = strdup(name);
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
    map->seg[S_WALL] = segAppend(map->seg[S_WALL], txtrNew(WHITE,
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
    map->obj[O_MOB] = mobNew(ffC(600.0f), "./Assets/Doggo.png");
}

// loads default map
void mapDefault(Map *map)
{
    mapDefaultSegments(map);
    mapDefaultObjects(map);
}

// frees contents of map
void mapFree(Map *map)
{
    assertExpr(map);
    if(map->name){
        free(map->name);
        map->name = NULL;
    }
    if(map->path){
        free(map->path);
        map->path = NULL;
    }
    if(map->file){
        fclose(map->file);
        map->file = NULL;
    }
    for(st i = 0; i < S_N; i++)
        if(map->seg[i])
            map->seg[i] = segListFree(map->seg[i]);
    for(st i = 0; i < O_N; i++)
        if(map->obj[i])
            map->obj[i] = objListFree(map->obj[i]);
}

// attempts to load map file at ./Maps/name if present
// if not present, sets map.path to ../Maps/map.bork or ../Maps/map(n).bork
// starting at n=1 and increasing until unique file path is found
Map mapLoad(char *name)
{
    Map map = {0};
    if(name)
        map.name = strdup(name);
    else
        newMapFileNum(&map);
    char path[256] = {0};
    sprintf(path, "./Maps/%s.bork", map.name);
    map.path = strdup(path);
    // printf("name: \"%s\"\npath: \"%s\"\n", map.name, map.path);
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
    printf("Saving map\n");
    assertExpr(map && map->path && map->name);
    printf("Map name: \"%s\"\nMap path: \"%s\"\n", map->name, map->path);
    if(map->file)
        fclose(map->file);
    assertExpr((map->file = fopen(map->path, "wb")));
    const st nameLen = strlen(map->name);
    printf("Writing map name to file\n");
    assertExpr(fwrite(map->name, sizeof(char), nameLen, map->file) == nameLen);
    fputc('\n', map->file);
    for(SegType type = 0; type < S_N; type++){
        Seg *seg = map->seg[type];
        while(seg){
            fieldPrint(segToPiece(seg));
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
    map->file = NULL;
}

#endif /* end of include guard: MAPIO_H */
