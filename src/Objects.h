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

// frees del
// searches for del in list, removes if found
Obj* objDelete(Obj *objs, Obj *del)
{
    if(!del)
        return objs;
    if(!objs)
        return NULL;
    if(del == objs){
        Obj *next = objs->next;
        free(objs);
        return next;
    }
    Obj *cur = objs;
    while(cur && cur->next != del)
        cur = cur->next;
    if(!cur)
        return objs;
    Obj *next = cur->next->next;
    free(cur->next);
    cur->next = next;
    return objs;
}

// returns the number of objects in the list
st objListLen(Obj *objs)
{
    uint len = 0;
    while(objs){
        len++;
        objs = objs->next;
    }
    return len;
}

// frees all objects in the list
void objListFree(Obj *list)
{
    while(list){
        Obj *next = list->next;
        free(list);
        list = next;
    }
}

// creates a new object
Obj* objNew(const ObjType type, const Coordf pos)
{
    Obj *obj = calloc(1, sizeof(Obj));
    obj->type = type;
    obj->pos = pos;
    return obj;
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
Obj* mobNew(Obj *mobList, const Coordf origin, char *path)
{
    Obj *obj = objNew(O_MOB, origin);
    const st txtlen = strlen(path);
    assertExpr(txtlen < 127);
    memcpy(obj->mob.path, path, txtlen);
    obj->mob.texture = mobListTxtrQryLoad(mobList, path);
    obj->mob.len = ffC(200.0f);
    // obj->mob.len = CCf(textureLen(obj->mob.texture));
    // printf("\"%s\": (%f,%f)\n", path, obj->mob.len.x, obj->mob.len.y);
    return obj;
}

#endif /* end of include guard: OBJECTS_H */
