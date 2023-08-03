#ifndef WDDECLS_H
#define WDDECLS_H

// Map.h
Seg* mapLoad(char *);
Coordf mapBounds(Seg *);
Coordf screenToMap(const Coord, const float, const Coord);
Coord mapToScreen(const Coord, const float, const Coordf);
float triSign(const Coordf, const Coordf, const Coordf);
bool cfInTri(Coordf, Coordf, Coordf, Coordf);
void mapUpdateIdState(Seg *, const uint, const bool);
void mapUpdateTriggers(const Coordf, const Coordf, Seg *);
void mapUpdateDynamics(Seg *);

// MapEditor.h
WallPacked* mapPack(Seg *);
Seg* mapUnpack(WallPacked *, const uint);
Coord resizeTransform(const Length, const Length, const Coord);
Coordf resizeTransformf(const Lengthf, const Lengthf, const Coordf);
Seg* posNearest(Seg *, const Coordf, Coordf **);
uint newMapFileNum(void);
void mapSave(Seg *, char *);
Seg* wallNew(const Color, const Coordf, const Coordf);
Seg* windNew(const Color, const Color, const Coordf, const Coordf, const float, const float);
Seg* doorNew(const Color, const Coordf, const Coordf, const uint, const float, const bool, const float, const Direction);
Seg* trigNew(const Color, const Coordf, const Coordf, const uint, const Coordf, const Coordf);
Seg* segAppend(Seg *, Seg *);
Seg* segDelete(Seg *, Seg *);
uint segListLen(Seg *);
void segFreeList(Seg *);
Seg* mapDefault(void);
int numKeyPressed(void);
Coord editColor(Coord, Color*);
bool checkEditorExit(void);
bool checkKeyS(Seg *, char *, bool, const float);
void checkScroll(Offset *, const Coordf, const bool, float *, float *);
void drawOriginLines(const Offset, const Length);
void drawGrid(const Offset, const Length, const float, const bool, const float);
void drawEditorMap(Seg *, const Selection, const Offset, const float);
void drawSel(const Selection, const Offset, const float);
void mlrUpdate(Minfo *, Minfo *, Selection *, const Offset, const float, const float);
Minfo mlUpdate(Minfo, Selection *, Seg *, const float, const bool, const float);
Minfo mrUpdate(Minfo, Selection *, Seg **, const Color, const bool);
Selection selNext(Selection);
Seg* updateDel(Seg *, Selection *);
Length updateResize(Length, Offset *);
Offset updatePan(Offset, Minfo *, Minfo *);
Seg* mapEdit(Seg *, char *);

// WolfenDoge.h
Coord toView(const View, const Coordf, const float);
bool lineIntersection(const Coordf, const Coordf, const Coordf, const Coordf, Coordf *);
Offset wasdKeyStateOffset(void);
Offset arrowKeyStateOffset(void);
Offset dirKeyStateOffset(void);
Offset wasdKeyPressedOffset(void);
Offset arrowKeyPressedOffset(void);
Offset dirKeyPressedOffset(void);
Ray castRayMin(const Coordf, const Coordf, Seg *, const bool, const float);
Ray castRay(const Coordf, const Coordf, Seg *, const bool);
void drawFp(const View, Seg *, const Player, const Length);
void drawBv(const View, Seg *, const Player, const float, const Coordf);
Player playerMoveMouse(Player, Seg *);
Player playerMoveKeys(Player);

#endif /* end of include guard: WDDECLS_H */
