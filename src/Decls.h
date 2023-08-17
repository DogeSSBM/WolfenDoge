#ifndef WDDECLS_H
#define WDDECLS_H

// Map.h
Coordf mapBoundMax(Seg *);
Coordf mapBoundMin(Seg *);
Coordf mapLength(Seg *);
Coordf screenToMap(const Coord, const float, const Coord);
Coord mapToScreen(const Coord, const float, const Coordf);
Seg* mapQueryObjId(Map *, Seg *, const uint);
float triSign(const Coordf, const Coordf, const Coordf);
bool cfInTri(Coordf, Coordf, Coordf, Coordf);
bool cfInQuad(const Coordf, const Coordf, const Coordf, const Coordf, const Coordf);
void mapUpdateIdState(Map *, const uint, const bool);
Update* upQueryId(Update *, const uint);
Update* upAppend(Update *, Update *);
Update* upNew(const uint, const bool);
Update* upUpdate(Update *, const uint, const bool);
Update* mapQueueUpdates(const Coordf, const Coordf, Map *);
void mapApplyUpdates(Map *, Update *);
void mapUpdateDynamics(Map *);

// MapEditorSegments.h
Seg* wallNew(const Color, const Coordf, const Coordf);
Seg* txtrNew(Seg *, const Color, const Coordf, const Coordf, char *);
Seg* windNew(const Color, const Color, const Coordf, const Coordf, const float, const float);
Seg* doorNew(const Color, const Coordf, const Coordf, const uint, const float, const bool, const float, const Direction);
Seg* trigNew(const Color, const Coordf, const Coordf, const uint, const Coordf, const Coordf);
Seg* convNew(const Color, const Coordf, const Coordf, const uint, const uint);
Seg* segAppend(Seg *, Seg *);
Seg* segDelete(Seg *, Seg *);
st segListLen(Seg *);
void segListFree(Seg *);

// MapEditorUpdate.h
void updateMouseCommon(Minfo *, Minfo *, Selection *, const Offset, const float, const float);
Minfo updateMouseL(Minfo, Selection *, Map *, const float, const bool, const float);
Minfo updateMouseR(Minfo, Selection *, Map *, const Color, const bool);
Selection updateSelCursor(Selection);
Selection updateSelNext(Selection, Map *);
bool updateDel(Map *, Selection *);
Length updateResize(Length, Offset *);
Offset updatePan(Offset, Minfo *, Minfo *);

// Objects.h
Obj* objAppend(Obj *, Obj *);
Obj* objDelete(Obj *, Obj *);
st objListLen(Obj *);
void objListFree(Obj *);
Obj* objNew(const ObjType, const Coordf);
Obj* keyNew(const Coordf, const Color);
Obj* spawnNew(const Coordf, const float);
Obj* mobNew(Obj *, const Coordf, char *);

// MapEditor.h
Coord resizeTransform(const Length, const Length, const Coord);
Coordf resizeTransformf(const Lengthf, const Lengthf, const Coordf );
MapPiece posNearest(Map *, const Coordf, const MapPieceType, Coordf **);
MapPiece coordNext(MapPiece, Coordf **);
MapPiece coordNextWrap(Map *, MapPiece, Coordf **);
MapPiece posNext(Map *, MapPiece, Coordf **);
int numKeyPressed(void);
Coord editColor(Coord, Color *);
uint editUint(uint);
float editFloat(float);
bool checkEditorExit(void);
bool checkKeyS(Map*, const bool, const float);
void checkScroll(Offset *, const Coordf, const bool, float *, float *);
void mapEdit(Map *);

// MapIO.h
Texture* wallListTxtrQryLoad(Seg *, char *);
Seg* txtrCleanup(Seg *, Seg *);
Seg* txtrApply(Seg *, Texture *, char *);
void newMapFileNum(Map *);
Map mapLoad(char *);
void mapSave(Map *);
SegPacked mapPack(Seg *);
Seg* mapUnpack(SegPacked);
void mapDefault(Map*);

// MapEditorDraw.h
void drawOriginLines(const Offset, const Length);
void drawGrid(const Offset, const Length, const float, const bool, const float);
void drawEditorMap(Map *, const Selection, const Offset, const float);
Coord drawSegType(const Coord, const SegType, const bool);
Coord drawColor(Coord, const uint, Color, const bool, const int);
Coord drawCoordf(const Coord, const char *, const Coordf, const bool);
Coord drawf(const Coord, const char *, const float, const bool);
Coord drawstr(const Coord, const char *, const bool);
Coord drawu(const Coord, const char *, const uint, const bool);
Coord drawSelCommon(const Selection);
void drawSelWall(const Selection, Coord);
void drawSelWind(const Selection, Coord);
void drawSelDoor(const Selection, Coord);
void drawSelTrig(const Selection, Coord);
void drawSelConv(const Selection, Coord);
void drawSel(const Selection, const Offset, const float);

// WolfenDoge.h
Coord toView(const View, const Coordf, const float);
bool inView(const View, const Coord);
Direction viewBoundIntersect(Seg[4], const Coordf, const Coordf, Coordf *);
bool limitViewBounds(const View, Coord *, Coord *);
Ray castRayMin(const Coordf, const Coordf, Map *, const bool, const float);
Ray castRay(const Coordf, const Coordf, Map *, const bool);
void drawWall(const View, const Ray, const int, const int, const int, const float);
void drawFp(const View, Map *, const Player, const Length);
void drawBv(const View, Map *, const Player, const float, const Coordf);
void playerMove(Map*);

#endif /* end of include guard: WDDECLS_H */
