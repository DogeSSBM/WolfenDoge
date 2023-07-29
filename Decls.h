#ifndef WDDECLS_H
#define WDDECLS_H

// Map.h
Wall* mapLoad(char *);
Coordf mapBounds(Wall *);
Coordf screenToMap(const Coord, const float, const Coord);
Coord mapToScreen(const Coord, const float, const Coordf);
float triSign(const Coordf, const Coordf, const Coordf);
bool cfInTri(Coordf, Coordf, Coordf, Coordf);
void mapUpdateIdState(Wall *, const uint, const bool);
void mapUpdateTriggers(const Coordf, const Coordf, Wall *);
void mapUpdateDynamics(Wall *);

// MapEditor.h
WallPacked* mapPack(Wall *);
Wall* mapUnpack(WallPacked *, const uint);
Coord resizeTransform(const Length, const Length, const Coord);
Coordf resizeTransformf(const Lengthf, const Lengthf, const Coordf);
Wall* posNearest(Wall *, const Coordf, Coordf **);
void defaultMapFileName(char *);
void mapSave(Wall *, char *);
Wall* wallNew(const Color, const Coordf, const Coordf);
Wall* wallAppend(Wall *, Wall *);
Wall* wallDelete(Wall *, Wall *);
uint wallListLen(Wall *);
void wallFreeList(Wall *);
Wall* mapDefault(void);
int numKeyPressed(void);
Coord editColor(Coord, Color*);
bool checkEditorExit(void);
bool checkKeyS(Wall *, char *, bool, const float);
void checkScroll(Offset *, const Coordf, const bool, float *, float *);
void drawOriginLines(const Offset, const Length);
void drawGrid(const Offset, const Length, const float, const bool, const float);
void drawEditorMap(Wall *, const Selection, const Offset, const float);
void drawSel(const Selection, const Offset, const float);
void mlrUpdate(Minfo *, Minfo *, Selection *, const Offset, const float, const float);
Minfo mlUpdate(Minfo, Selection *, Wall *, const float, const bool, const float);
Minfo mrUpdate(Minfo, Selection *, Wall **, const Color, const bool);
Selection selCheckRev(Selection);
Wall* updateDel(Wall *, Selection *);
Length updateResize(Length, Offset *);
Offset updatePan(Offset, Minfo *, Minfo *);
Wall* mapEdit(Wall *, char *);

// WolfenDoge.h
Coord toView(const View, const Coordf, const float);
bool lineIntersection(const Coordf, const Coordf, const Coordf, const Coordf, Coordf *);
Offset wasdKeyStateOffset(void);
Offset arrowKeyStateOffset(void);
Offset dirKeyStateOffset(void);
Offset wasdKeyPressedOffset(void);
Offset arrowKeyPressedOffset(void);
Offset dirKeyPressedOffset(void);
Ray castRayMin(const Coordf, const Coordf, Wall *, const bool, const float);
Ray castRay(const Coordf, const Coordf, Wall *, const bool);
void drawFp(const View, Wall *, const Player, const Length);
void drawBv(const View, Wall *, const Player, const float, const Coordf);
Player playerMoveMouse(Player, Wall *);
Player playerMoveKeys(Player);

#endif /* end of include guard: WDDECLS_H */
