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

// creates a new object
Obj* objNew(const ObjType type, const Coordf pos)
{
    Obj *obj = calloc(1, sizeof(Obj));
    obj->type = type;
    obj->pos = pos;
    return obj;
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
    Obj *obj = objNew(O_KEY, pos);
    obj->key.c = c;
    return obj;
}

// creates a new object of type O_SPAWN
Obj* spawnNew(const Coordf pos, const float ang)
{
    Obj *obj = objNew(O_SPAWN, pos);
    obj->spawn.ang = ang;
    return obj;
}

// creates a new object of type O_MOB
Obj* mobNew(const Coordf origin, char *path)
{
    Obj *obj = objNew(O_MOB, origin);
    const st txtlen = strlen(path);
    assertExpr(txtlen < 127);
    memcpy(obj->mob.path, path, txtlen);
    obj->mob.texture = loadTexture(path);
    printf("loaded texture :\"%s\"\n", obj->mob.path);
    // obj->mob.texture = mobListTxtrQryLoad(mobList, path);
    obj->mob.len = ffC(200.0f);
    obj->mob.origin = origin;
    return obj;
}

// creates a new segment with type S_CONV
Obj* convNew(const ConvType type, const Coordf pos, const uint inIdA, const uint inIdB, const uint outId)
{
    Obj *obj = objNew(O_CONV, pos);
    obj->conv.type = type;
    obj->conv.inIdA = inIdA;
    obj->conv.inIdB = inIdB;
    obj->conv.outId = outId;
    obj->conv.outState = false;
    return obj;
}

// searches for next converter with specified input / output id
// returns NULL if none found
// Obj* convQueryNextId(Map *map, Obj *cur, const uint id, const bool isInput)
// {
//     if(cur == NULL)
//         cur = map->obj[0];
//     if(!cur)
//         return NULL;
//     cur = cur->next;
//     while(cur){
//         if(isInput)
//             if(cur->conv.inIdA == id || cur->conv.inIdB == id)
//                 return cur;
//         else
//             if(cur->conv.outId == id)
//                 return cur;
//         cur = cur->next;
//     }
//     return NULL;
// }
//
// bool hasChildOutId(Map *map, const uint id, Obj *obj)
// {
//     assertExpr(map && parent && parent->type == O_CONV);
//     if(!obj)
//         return false;
//     assertExpr(obj->type == O_CONV);
//     if(id == cur->conv.outId)
//         return true;
//     const uint outId = obj->outId;
//     obj = convQueryNextId(map, NULL, outId, true);
//     while(obj){
//         if(hasChildOutId(map, id, obj))
//             return true;
//         obj = convQueryNextId(map, obj, outId, true);
//     };
//     return false;
// }
//
// // verifies converters do not have cyclical dependancies
// bool hasCycle(Map *map, Obj *obj)
// {
//     assertExpr(map && obj && obj->type == O_CONV);
//     if(obj->conv.inIdA == obj->conv.outId || obj->conv.inIdB == obj->conv.outId)
//         return true;
//     return(hasChildOutId(map, obj->conv.inIdA, obj) || hasChildOutId(map, obj->conv.inIdB, obj);
// }

#endif /* end of include guard: OBJECTS_H */
