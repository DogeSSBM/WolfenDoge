#ifndef OBJECTS_H
#define OBJECTS_H

// appends tail to the end of the list (head)
Obj* objAppend(Obj *head, Obj *tail)
{
    if(!head)
        return tail;
    Obj *cur = head;
    while(cur->next)
        cur = cur->next;
    cur->next = tail;
    return head;
}

// frees obj (deep free)
Obj* objFree(Obj *obj)
{
    if(!obj)
        return NULL;
    Obj *next = obj->next;
    if(obj->type == O_MOB && obj->mob.texture)
        textureFree(obj->mob.texture);
    free(obj);
    return next;
}

// searches for del in objList, removes and frees (deep)
Obj* objDelete(Obj *objList, Obj *del)
{
    if(!del)
        return objList;
    if(!objList)
        return NULL;
    if(del == objList)
        return objFree(objList);
    Obj *cur = objList;
    while(cur && cur->next != del)
        cur = cur->next;
    if(cur)
        cur->next = objFree(cur->next);
    return objList;
}

// returns the number of objects in the list
st objListLen(Obj *objs)
{
    st len = 0;
    while(objs){
        len++;
        objs = objs->next;
    }
    return len;
}

// frees all objects in the list
Obj* objListFree(Obj *list)
{
    while(list)
        list = objFree(list);
    return NULL;
}

// duplicates object
Obj* objDup(Obj *obj)
{
    if(!obj)
        return NULL;
    Obj *dup = calloc(1, sizeof(Obj));
    memcpy(dup, obj, sizeof(Obj));
    if(obj->type == O_MOB && obj->mob.texture)
        dup->mob.texture = loadTexture(dup->mob.path);
    dup->next = NULL;
    return dup;
}

// creates a new object of type O_KEY
Obj* keyNew(const Coordf pos, const Color c)
{
    Obj *obj = calloc(1, sizeof(Obj));
    obj->type = O_KEY;
    obj->pos = pos;
    obj->key.c = c;
    return obj;
}

// creates a new object of type O_SPAWN
Obj* spawnNew(const Coordf pos, const float ang)
{
    Obj *obj = calloc(1, sizeof(Obj));
    obj->pos = pos;
    obj->type = O_SPAWN;
    obj->spawn.ang = ang;
    return obj;
}

// creates a new object of type O_MOB
Obj* mobNew(const Coordf origin, char *path)
{
    Obj *obj = calloc(1, sizeof(Obj));
    obj->pos = origin;
    obj->type = O_MOB;
    const st txtlen = strlen(path);
    assertExpr(txtlen < 127);
    memcpy(obj->mob.path, path, txtlen);
    obj->mob.texture = loadTexture(path);
    printf("loaded texture :\"%s\"\n", obj->mob.path);
    // obj->mob.texture = mobListTxtrQryLoad(mobList, path);
    obj->mob.len = ffC(200.0f);
    obj->mob.a = origin;
    obj->mob.b = fC(origin.x + 200.0f, origin.y);
    obj->mob.origin = origin;
    return obj;
}

// creates a new object
Obj* objNew(const ObjType type, const Coordf a, const Coordf b)
{
    (void)b;
    switch(type){
        case O_KEY:
            return keyNew(a, GREEN);
            break;
        case O_MOB:
            return mobNew(a, "./Assets/Doggo.png");
            break;
        case O_SPAWN:
            return spawnNew(a, cfCfToDeg(a, b));
            break;
        default:
            panic("Unspecified ObjType");
            break;
    }
    return NULL;
}

#endif /* end of include guard: OBJECTS_H */
