#ifndef WDDECLS_H
#define WDDECLS_H

WallPacked* mapPack(Wall *);
Wall* mapUnpack(WallPacked *, const uint);
Coordf mapBounds(Wall *);
Coord resizeTransform(const Length, const Length, const Coord);
Coordf resizeTransformf(const Lengthf, const Lengthf, const Coordf);
Coordf screenToMap(const Coord, const float, const Coord);
Coord mapToScreen(const Coord, const float, const Coordf);
Coordf absOff(const Coord, const Coord);
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
int editColor(Color *, int, Wall *);
bool checkEditorExit(void);
bool checkKeyS(Wall *, char *, bool, const float);
void checkScroll(Offset *, const Coordf, const bool, float *, float *);
void drawOriginLines(const Offset, const Length);
void drawGrid(const Offset, const Length, const float, const bool, const float);
void drawColor(const Length, Color, const int);
void drawEditorMap(Wall *, const Offset, const float);
void drawSel(const Selection, const Offset, const float);
void mlrUpdate(Minfo *, Minfo *, Selection *, const Offset, const float, const float);
Minfo mlUpdate(Minfo, Selection *, Wall *, const float, const bool, const float);
Minfo mrUpdate(Minfo, Selection *, Wall **, const Color, const bool);
Selection selCheckRev(Selection);
Wall* updateDel(Wall *, Selection *);
Length updateResize(Length, Offset *);
Offset updatePan(Offset, Minfo *, Minfo *);
Wall* mapEdit(Wall *, char *);

Coordf fC(const float, const float);
float fmost(const float, const float);
float cfMax(const Coordf);
bool cfSame(const Coordf, const Coordf);
Coordf cfMod(const Coordf, const float);
Coordf cfSub(const Coordf, const Coordf);
Coordf cfAddf(const Coordf, const float);
Coordf cfSnap(const Coordf, const float);
Coordf cfSnapMid(const Coordf, const float);
Coord coordAbs(const Coord);
u8* colorIndex(Color *, const int);
bool keyCtrlState(void);
bool keyShiftState(void);
bool checkCtrlKey(const Scancode);
Coord toView(const View, const Coordf, const float);
bool lineIntersection(const Coordf, const Coordf, const Coordf, const Coordf, Coordf *);
Offset wasdKeyStateOffset(void);
Offset arrowKeyStateOffset(void);
Offset dirKeyStateOffset(void);
Offset wasdKeyPressedOffset(void);
Offset arrowKeyPressedOffset(void);
Offset dirKeyPressedOffset(void);
Ray castRay(const Coordf, const Coordf, Wall *);
void drawFp(const View, Wall *, const Player, const Length);
void drawBv(const View, Wall *, const Player, const float, const Coordf);
Player playerMoveMouse(Player, Wall *);
Player playerMoveKeys(Player);
Wall* mapLoad(char *);

#endif /* end of include guard: WDDECLS_H */
