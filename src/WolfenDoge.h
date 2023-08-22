#ifndef WOLFENDOGE_H
#define WOLFENDOGE_H

// scales and offsets pos into view
Coord toView(const View view, const Coordf pos, const float scale)
{
    return coordAdd(view.pos, iC(pos.x*scale, pos.y*scale));
}

// returns true if pos is in view
bool inView(const View view, const Coord pos)
{
    return inBound(pos.x, view.pos.x, view.pos.x+view.len.x) && inBound(pos.y, view.pos.y, view.pos.y+view.len.y);
}

// returns the direction of the boundary of the segment that is intersected by the line segment ab
Direction viewBoundIntersect(Seg bounds[4], const Coordf a, const Coordf b, Coordf *at)
{
    for(Direction d = 0; d < 4; d++){
        if(lineIntersection(bounds[d].a, bounds[d].b, a, b, at))
            return d;
    }
    return 4;
}

// if line segment ab goes out of bounds of view, sets ab to the point where the boundary is intersected
bool limitViewBounds(const View view, Coord *a, Coord *b)
{
    const Coordf fa = CCf(*a);
    const Coordf fb = CCf(*b);
    const Coordf vpos = CCf(view.pos);
    const Coordf vlen = CCf(view.len);
    Seg bounds[4] = {
        (Seg){.a = vpos,                       .b = fC(vpos.x+vlen.x, vpos.y)},
        (Seg){.a = fC(vpos.x+vlen.x, vpos.y),  .b = cfAdd(vpos, vlen)},
        (Seg){.a = fC(vpos.x, vpos.y+vlen.y),  .b = cfAdd(vpos, vlen)},
        (Seg){.a = vpos,                       .b = fC(vpos.x, vpos.y+vlen.y)}
    };

    if(!inView(view, *a)){
        Coordf na = fa;
        Direction da = viewBoundIntersect(bounds, fa, fb, &na);
        if(inView(view, *b)){
            *a = CfC(na);
            return true;
        }
        Coordf nb = fb;
        Direction db = viewBoundIntersect(bounds, fb, fa, &nb);
        if(da == 4 || db == 4)
            return false;
        *a = CfC(na);
        *b = CfC(nb);
        return true;
    }
    if(!inView(view, *b)){
        Coordf nb = fb;
        Direction db = viewBoundIntersect(bounds, fb, fa, &nb);
        if(inView(view, *a)){
            *b = CfC(nb);
            return true;
        }
        Coordf na = fa;
        Direction da = viewBoundIntersect(bounds, fa, fb, &na);
        if(da == 4 || db == 4)
            return false;
        *a = CfC(na);
        *b = CfC(nb);
        return true;
    }
    return true;
}

// inserts ins such that list will be sorted greatest dst to least
Ray* RayInsert(Ray *list, Ray *ins)
{
    if(!list)
        return ins;
    if(ins->dst > list->dst){
        ins->next = list;
        return ins;
    }
    Ray *cur = list;
    while(cur->next && cur->next->dst > ins->dst)
        cur = cur->next;
    Ray *next = cur->next;
    cur->next = ins;
    ins->next = next;
    return list;
}

// allocates a new Ray
Ray* RayNew(const MapPiece piece, const float dst, const Coordf pos)
{
    if(piece.type >= M_ANY)
        return NULL;
    Ray *rs = calloc(1, sizeof(Ray));
    rs->piece = piece;
    rs->dst = dst;
    rs->pos = pos;
    return rs;
}

// frees head of list, returning next
Ray* RayFree(Ray *list)
{
    if(!list)
        return list;
    Ray *next = list->next;
    free(list);
    return next;
}

// casts ray from origin to distantPoint and returns nearest 'solid' intersection
// where 'solid' refers to a wall segment, a fully closed door segment
Ray* castRayBase(const Coordf origin, const Coordf distantPoint, Map *map)
{
    Seg *seg = map->seg[S_WALL];
    if(!seg)
        if(!(seg = pieceNext(map, (MapPiece){.type = M_SEG}).seg))
            return NULL;
    float dst = 60000.0f;
    Coordf pos = distantPoint;
    for(SegType type = 0; type < S_N; type++){
        if(type == S_CONV || type == S_TRIG || type == S_WIND)
            continue;
        Seg *curSeg = map->seg[type];
        while(curSeg){
            if(curSeg->type == S_DOOR && curSeg->door.pos < 1.0f){
                curSeg = curSeg->next;
                continue;
            }
            float curDst = 0;
            Coordf curPos = {0};
            if(lineIntersection(origin, distantPoint, curSeg->a, curSeg->b, &curPos) &&
            (curDst = cfDist(origin, curPos)) < dst){
                seg = curSeg;
                dst = curDst;
                pos = curPos;
            }
            curSeg = curSeg->next;
        }
    }
    return RayNew((MapPiece){.type = seg?M_SEG:M_NONE, .seg = seg}, dst, pos);
}

// casts ray from origin to distantPoint and returns all intersections that are < max distance away
Ray* castRayMax(const Coordf origin, const Coordf distantPoint, Map *map, const float max)
{
    Ray *list = NULL;
    for(SegType type = 0; type < S_N; type++){
        if(type == S_CONV || type == S_TRIG)
            continue;
        Seg *seg = map->seg[type];
        while(seg){
            float dst = 0;
            Coordf pos = {0};
            if(
                lineIntersection(origin, distantPoint, seg->a, seg->b, &pos) &&
                (dst = cfDist(origin, pos)) < max
            )
                list = RayInsert(list, RayNew((MapPiece){.type = M_SEG, .seg = seg}, dst, pos));
            seg = seg->next;
        }
    }

    Obj *obj = map->obj[O_MOB];
    while(obj){
        float dst = 0;
        Coordf pos = {0};
        if(
            lineIntersection(origin, distantPoint, obj->mob.a, obj->mob.b, &pos) &&
            (dst = cfDist(origin, pos)) < max
        )
            list = RayInsert(list, RayNew((MapPiece){.type = M_OBJ, .obj = obj}, dst, pos));
        obj = obj->next;
    }
    return list;
}

// creates ordered list of all intersections in between origin and base intersection
Ray* castRay(const Coordf origin, const Coordf distantPoint, Map *map)
{
    Ray *ray = castRayBase(origin, distantPoint, map);
    Ray *list = castRayMax(origin, distantPoint, map, ray ? ray->dst : rayUnwrapDst(ray));
    list = RayInsert(list, ray);
    return list;
}

// frees sect, returning its pos
Coordf rayUnwrapPos(Ray *sect)
{
    const Coordf pos = sect->pos;
    free(sect);
    return pos;
}

// if there are no map segments to intersect with, no rays will be allocated
// in this case we return a large float as if the ray traveled forever
// frees sect, returning its dst
float rayUnwrapDst(Ray *sect)
{
    if(!sect)
        return 999999999.0f;
    const float dst = sect->dst;
    free(sect);
    return dst;
}

// draws vertical slice of object based on ray information
void drawObjSlice(const View view, const Ray *rs, const int xpos, const int ymid, const int dst, const float hsec)
{
    if(!rs)
        return;
    const int height = (view.len.y*120) / imax(dst, 1);
    assertExpr(rs->piece.type == M_OBJ && rs->piece.obj->type == O_MOB);
    const Length txtrlen = textureLen(rs->piece.obj->mob.texture);
    const float walllen = cfDist(rs->piece.obj->mob.a, rs->piece.obj->mob.b);
    const float poslen = cfDist(rs->piece.obj->mob.a, rs->pos);
    const float xdst = poslen/walllen;
    const Rect r = {
        .x = (int)((float)txtrlen.x * xdst),
        .y = 0,
        .w = 1,
        .h = txtrlen.y
    };
    Coord p = iC(xpos-hsec/2, ymid-height/2);
    const Length l = iC(hsec+1, height);
    drawTextureRectCoordResize(rs->piece.obj->mob.texture, r, p, l);
    return;
}

// draws vertical slice of segment based on ray information
void drawSegSlice(const View view, const Ray *rs, const int xpos, const int ymid, const int dst, const float hsec)
{
    if(!rs)
        return;
    const int height = (view.len.y*120) / imax(dst, 1);
    if(!rs->piece.seg){
        setColor(BLACK);
        fillRectCenteredCoordLength(
            iC(xpos, ymid),
            iC(hsec+1, imax(view.len.y/64, 1))
        );
        return;
    }
    if(rs->piece.seg->type == S_WALL && rs->piece.seg->wall.path[0] != '\0'){
        const Length txtrlen = textureLen(rs->piece.seg->wall.texture);
        const float walllen = cfDist(rs->piece.seg->a, rs->piece.seg->b);
        const float poslen = cfDist(rs->piece.seg->a, rs->pos);
        const float xdst = poslen/walllen;
        const Rect r = {
            .x = (int)((float)txtrlen.x * xdst),
            .y = 0,
            .w = 1,
            .h = txtrlen.y
        };
        const Coord p = iC(xpos-hsec/2, ymid-height/2);
        const Length l = iC(hsec+1, height);
        drawTextureRectCoordResize(rs->piece.seg->wall.texture, r, p, l);
        return;
    }
    setColor((const Color){
        .r = clamp(rs->piece.seg->color.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
        .g = clamp(rs->piece.seg->color.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
        .b = clamp(rs->piece.seg->color.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
    });
    if(rs->piece.seg->type == S_WIND){
        const int boundL = xpos-hsec/2;
        const int boundR = boundL+hsec+1.0f;
        const int boundBotLower = ymid+height/2;
        const int boundBotUpper = boundBotLower - (int)(rs->piece.seg->wind.height * (float)height);
        fillRectCoords(iC(boundL, boundBotLower), iC(boundR, boundBotUpper));

        setColor((const Color){
            .r = clamp(rs->piece.seg->wind.topColor.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .g = clamp(rs->piece.seg->wind.topColor.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .b = clamp(rs->piece.seg->wind.topColor.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
        });
        const int boundTopUpper = ymid-height/2;
        const int boundTopLower = boundTopUpper + (int)(rs->piece.seg->wind.height * (float)height);
        fillRectCoords(iC(boundL, boundTopUpper), iC(boundR, boundTopLower));
        return;
    }
    if(rs->piece.seg->type == S_DOOR){
        if(rs->piece.seg->door.pos == 0.0f)
            return;
        if(dirUD(rs->piece.seg->door.closeDir)){
            const int boundL = xpos-hsec/2;
            const int boundR = boundL+hsec+1.0f;
            const int doorHeight = height * rs->piece.seg->door.pos;
            const int boundTop = ymid-height/2 + (rs->piece.seg->door.closeDir == DIR_U * (height - doorHeight));
            const int boundBot = boundTop + doorHeight;
            fillRectCoords(iC(boundL, boundTop), iC(boundR, boundBot));
        }
        return;
    }
    const int ypos = ymid;
    const int ylen = imin(view.len.y, height);
    fillRectCenteredCoordLength(
        iC(xpos, ypos),
        iC(hsec+1, ylen)
    );
}

// draws ceiling and floor
void drawCeilFloor(const View view)
{
    setColor(GREY2);
    fillRectCoordLength(view.pos, iC(view.len.x,view.len.y/2));
    setColor(GREY3);
    fillRectCoordLength(iC(view.pos.x,view.len.y/2), iC(view.len.x,view.len.y/2));
}

// draw first person view
void drawFp(const View view, Map *map, const Player player)
{
    drawCeilFloor(view);
    if(pieceNext(map, (MapPiece){.type = M_ANY}).type >= M_ANY)
        return;
    const Coordf startingPos = cfAdd(player.pos, cfRotateDeg((const Coordf){.x=2048.0f,.y=-2048.0f}, player.ang));
    const float scanAng = degReduce(player.ang+90.0f);
    const float hsec = (float)view.len.x/FOV_NUM_RAYS;
    const int ymid = view.pos.y+view.len.y/2;
    for(int i = 0; i < FOV_NUM_RAYS; i++){
        const Coordf farpos = cfAdd(startingPos, degMagToCf(scanAng, ((float)i/(float)FOV_NUM_RAYS)*4096.0f));
        const float viewTan = (0.5f-i/(float)FOV_NUM_RAYS) / 0.5f;
        const int xpos = view.pos.x+hsec/2+i*hsec;
        Ray *list = castRay(player.pos, farpos, map);
        while(list){
            const int corDst = (int)(list->dst/sqrtf(viewTan*viewTan+1.0f));
            if(list->piece.type == M_SEG)
                drawSegSlice(view, list, xpos, ymid, corDst, hsec);
            else
                drawObjSlice(view, list, xpos, ymid, corDst, hsec);
            list = RayFree(list);
        }
    }
}

// draw birds eye view
void drawBv(const View view, Map *map, const Player player, const float scale, const Coordf off)
{
    setColor(BLACK);
    fillRectCoordLength(view.pos, view.len);
    (void)off;
    const Length hlen = coordDivi(view.len, 2);
    for(SegType type = 0; type < S_N; type++){
        if(type == S_TRIG)
            continue;
        Seg *cur = map->seg[type];
        while(cur){
            Coord a = coordAdd(toView(view, cfSub(cur->a, player.pos), scale), hlen);
            Coord b = coordAdd(toView(view, cfSub(cur->b, player.pos), scale), hlen);
            setColor(cur->color);
            if(limitViewBounds(view, &a, &b))
                drawLineCoords(a, b);
            cur = cur->next;
        }

        Coord ppos = coordAdd(view.pos, hlen);
        Ray *rr = castRayBase(player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang + 45.0f), 6000.0f)), map);
        Ray *rl = castRayBase(player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang - 45.0f), 6000.0f)), map);
        Coord rpos = coordAdd(toView(view, cfSub(rr ? rayUnwrapPos(rr) : player.pos, player.pos), scale), hlen);
        Coord lpos = coordAdd(toView(view, cfSub(rl ? rayUnwrapPos(rl) : player.pos, player.pos), scale), hlen);
        limitViewBounds(view, &ppos, &rpos);
        limitViewBounds(view, &ppos, &lpos);
        setColor(YELLOW);
        drawLineCoords(ppos, rpos);
        drawLineCoords(ppos, lpos);
        fillCircleCoord(ppos, 2);
    }

    Obj *obj = map->obj[O_MOB];
    while(obj){
        Coord a = coordAdd(toView(view, cfSub(obj->mob.a, player.pos), scale), hlen);
        Coord b = coordAdd(toView(view, cfSub(obj->mob.b, player.pos), scale), hlen);
        setColor(BLUE);
        if(limitViewBounds(view, &a, &b))
            drawLineCoords(a, b);
        obj = obj->next;
    }
}

// moves the player
void playerMove(Map *map)
{
    map->player.ang = degReduce(map->player.ang + (float)(mouse.vec.x*2)/3.0f);
    map->player.ang = degReduce(map->player.ang + (keyState(SC_RIGHT) - keyState(SC_LEFT)));
    if(rayUnwrapDst(castRayBase(
        map->player.pos,
        cfAdd(map->player.pos, cfRotateDeg(cfMulf(CCf(wasdKeyStateOffset()), 6000.0f), map->player.ang+90.0f)),
        map
    )) > 10.0f)
        map->player.pos = cfAdd(map->player.pos, cfRotateDeg(CCf(coordMuli(wasdKeyStateOffset(), 2)), map->player.ang+90.0f));
}

#endif /* end of include guard: WOLFENDOGE_H */
