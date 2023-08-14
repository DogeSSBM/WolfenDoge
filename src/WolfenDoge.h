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

// casts ray from origin to distantPoint and returns the nearest intersection that is > min distance away
Ray castRayMin(const Coordf origin, const Coordf distantPoint, Map *map, const bool solidOnly, const float min)
{
    Ray ray = {
        .wall = map->seg[S_WALL],
        .dst = 60000.0f,
        .pos = distantPoint
    };
    for(SegType type = 0; type < S_N; type++){
        if(type == S_CONV || type == S_TRIG || (solidOnly && type == S_WIND))
            continue;
        Seg *seg = map->seg[type];
        while(seg){
            if(solidOnly && seg->type == S_DOOR && seg->door.pos < 1.0f){
                seg = seg->next;
                continue;
            }
            float curDst = 0;
            Coordf curPos = {0};
            if(
                lineIntersection(origin, distantPoint, seg->a, seg->b, &curPos) &&
                (curDst = cfDist(origin, curPos)) > min && curDst < ray.dst
            ){
                ray.wall = seg;
                ray.dst = curDst;
                ray.pos = curPos;
            }
            seg = seg->next;
        }
    }
    if(ray.dst == 60000.0f)
        ray.wall = NULL;
    return ray;
}

// casts ray from origin to distantPoint and returns the nearest intersection
Ray castRay(const Coordf origin, const Coordf distantPoint, Map *map, const bool solidOnly)
{
    return castRayMin(origin, distantPoint, map, solidOnly, -1.0f);
}

// draws vertical slice of wall based on ray information
void drawWall(const View view, const Ray ray, const int xpos, const int ymid, const int dst, const float hsec)
{
    const int height = (view.len.y*120) / fmax(dst, .01f);
    if(!ray.wall){
        setColor(BLACK);
        fillRectCenteredCoordLength(
            iC(xpos, ymid),
            iC(hsec+1, imax(view.len.y/64, 1))
        );
        return;
    }
    if(ray.wall->type == S_WALL && ray.wall->wall.path[0] != '\0'){
        const Length txtrlen = textureLen(ray.wall->wall.texture);
        const float walllen = cfDist(ray.wall->a, ray.wall->b);
        const float poslen = cfDist(ray.wall->a, ray.pos);
        const float xdst = poslen/walllen;
        const Rect r = {
            .x = (int)((float)txtrlen.x * xdst),
            .y = 0,
            .w = 1,
            .h = txtrlen.y
        };
        const Coord p = iC(xpos, ymid-height/2);
        const Length l = iC(hsec+1, height);
        drawTextureRectCoordResize(ray.wall->wall.texture, r, p, l);
        return;
    }
    setColor((const Color){
        .r = clamp(ray.wall->color.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
        .g = clamp(ray.wall->color.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
        .b = clamp(ray.wall->color.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
    });
    if(ray.wall->type == S_WIND){
        const int boundL = xpos-hsec/2;
        const int boundR = boundL+hsec+1.0f;
        const int boundBotLower = ymid+height/2;
        const int boundBotUpper = boundBotLower - (int)(ray.wall->wind.height * (float)height);
        fillRectCoords(iC(boundL, boundBotLower), iC(boundR, boundBotUpper));

        setColor((const Color){
            .r = clamp(ray.wall->wind.topColor.r-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .g = clamp(ray.wall->wind.topColor.g-(((dst*1.2f)/2000.0f)*255), 0, 256),
            .b = clamp(ray.wall->wind.topColor.b-(((dst*1.2f)/2000.0f)*255), 0, 256)
        });
        const int boundTopUpper = ymid-height/2;
        const int boundTopLower = boundTopUpper + (int)(ray.wall->wind.height * (float)height);
        fillRectCoords(iC(boundL, boundTopUpper), iC(boundR, boundTopLower));
        return;
    }
    if(ray.wall->type == S_DOOR){
        if(ray.wall->door.pos == 0.0f)
            return;
        if(dirUD(ray.wall->door.closeDir)){
            const int boundL = xpos-hsec/2;
            const int boundR = boundL+hsec+1.0f;
            const int doorHeight = height * ray.wall->door.pos;
            const int boundTop = ymid-height/2 + (ray.wall->door.closeDir == DIR_U * (height - doorHeight));
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

// draw first person view
void drawFp(const View view, Map *map, const Player player, const Length wlen)
{
    setColor(GREY2);
    fillRectCoordLength(view.pos, iC(view.len.x,view.len.y/2));
    setColor(GREY3);
    fillRectCoordLength(iC(view.pos.x,view.len.y/2), iC(view.len.x,view.len.y/2));

    const Coordf startingPos = cfAdd(player.pos, cfRotateDeg((const Coordf){.x=2048.0f,.y=-2048.0f}, player.ang));
    const float scanAng = degReduce(player.ang+90.0f);
    const float hsec = (float)view.len.x/FOV_NUM_RAYS;
    const int ymid = view.pos.y+view.len.y/2;
    for(int i = 0; i < FOV_NUM_RAYS; i++){
        const Coordf farpos = cfAdd(startingPos, degMagToCf(scanAng, ((float)i/(float)FOV_NUM_RAYS)*4096.0f));
        const Ray ray = castRay(player.pos, farpos, map, true);
        const float viewTan = (0.5-i/(float)FOV_NUM_RAYS) / 0.5;
        const int correctedDst = (int)(ray.dst/sqrtf(viewTan*viewTan+1.0f));
        const int xpos = view.pos.x+hsec/2+i*hsec;
        drawWall(view, ray, xpos, ymid, correctedDst, hsec);
        const Ray wray1 = castRay(player.pos, farpos, map, false);
        if(wray1.wall && wray1.wall->type != S_WALL){
            const Ray wray2 = castRayMin(player.pos, farpos, map, false, wray1.dst+1.0f);
            if(wray2.wall && wray2.wall->type != S_WALL)
                drawWall(view, wray2, xpos, ymid, (int)(wray2.dst/sqrtf(viewTan*viewTan+1.0f)), hsec);
            drawWall(view, wray1, xpos, ymid, (int)(wray1.dst/sqrtf(viewTan*viewTan+1.0f)), hsec);
        }
    }
    char buf[32] = {0};
    sprintf(buf, "%+14.6f, %+14.6f", player.pos.x, player.pos.y);
    const uint tscale = (wlen.x/3)/10;
    setTextSize(tscale);
    Texture *texture = textTexture(buf);
    setColor(BLACK);
    setTextColor(WHITE);
    const Coord pos = coordSub(wlen, textureLen(texture));
    fillRectCoordLength(pos, textureLen(texture));
    drawTextureCoord(texture, pos);
    textureFree(texture);
}

// draw birds eye view
void drawBv(const View view, Map *map, const Player player, const float scale, const Coordf off)
{
    setColor(BLACK);
    fillRectCoordLength(view.pos, view.len);
    (void)off;
    for(SegType type = 0; type < S_N; type++){
        if(type == S_TRIG)
            continue;
        Seg *cur = map->seg[type];
        const Length hlen = coordDivi(view.len, 2);
        while(cur){
            Coord a = coordAdd(toView(view, cfSub(cur->a, player.pos), scale), hlen);
            Coord b = coordAdd(toView(view, cfSub(cur->b, player.pos), scale), hlen);
            setColor(cur->color);
            if(limitViewBounds(view, &a, &b))
                drawLineCoords(a, b);
            cur = cur->next;
        }

        Coord ppos = coordAdd(view.pos, hlen);
        Coord rpos = coordAdd(toView(view, cfSub(castRay(
            player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang + 45.0f), 6000.0f)),
            map, false
        ).pos, player.pos), scale), hlen);
        Coord lpos = coordAdd(toView(view, cfSub(castRay(
            player.pos, cfAdd(player.pos, degMagToCf(degReduce(player.ang - 45.0f), 6000.0f)),
            map, false
        ).pos, player.pos), scale), hlen);
        limitViewBounds(view, &ppos, &rpos);
        limitViewBounds(view, &ppos, &lpos);
        setColor(YELLOW);
        drawLineCoords(ppos, rpos);
        drawLineCoords(ppos, lpos);
        fillCircleCoord(ppos, 2);
    }
}

// moves the player
void playerMove(Map *map)
{
    map->player.ang = degReduce(map->player.ang + (float)(mouse.vec.x*2)/3.0f);
    map->player.ang = degReduce(map->player.ang + (keyState(SC_RIGHT) - keyState(SC_LEFT)));
    if(castRay(
        map->player.pos,
        cfAdd(map->player.pos, cfRotateDeg(cfMulf(CCf(wasdKeyStateOffset()), 10.0f), map->player.ang+90.0f)),
        map, true
    ).dst > 10.0f)
        map->player.pos = cfAdd(map->player.pos, cfRotateDeg(CCf(coordMuli(wasdKeyStateOffset(), 2)), map->player.ang+90.0f));
}

#endif /* end of include guard: WOLFENDOGE_H */
