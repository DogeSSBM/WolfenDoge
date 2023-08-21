#ifndef WDDECLS_H
#define WDDECLS_H

// MapEditorSelection.h
Selection* selNew(Coord *, Coordf *, const PieceFields);
Selection* selAppend(Selection *, Selection *);
st selLen(Selection *);
Selection* selFree(Selection *);
Selection* selFreeList(Selection *);
Selection* selPosNearest(Map *, Coord *, const Coordf);
bool selPosSelected(Selection *, Coordf *);
bool selPieceSelected(Selection *, const MapPiece);

// Fields.h
void fieldPrint(const MapPiece);
Coord fieldDraw(const Field, const Coord, const uint);
void fieldPrintMap(Map *);
Coord drawPieceFields(const PieceFields, const Coord, const Coord);

// Pieces.h
PieceCoords pieceCoords(const MapPiece);
Color pieceColor(const MapPiece);
MapPiece pieceNext(Map *, MapPiece);
bool pieceSame(const MapPiece, const MapPiece);
MapPiece pieceNearest(Map *, const Coordf, Coordf **);
st pieceCountTotal(Map *);
MapPiece pieceNew(const NewPieceInfo, const Coordf, const Coordf);

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

// Segments.h
Seg* wallNew(const Color, const Coordf, const Coordf);
Seg* txtrNew(const Color, const Coordf, const Coordf, char *);
Seg* windNew(const Color, const Color, const Coordf, const Coordf, const float, const float);
Seg* doorNew(const Color, const Coordf, const Coordf, const uint, const float, const bool, const float, const Direction);
Seg* trigNew(const Color, const Coordf, const Coordf, const uint, const Coordf, const Coordf);
Seg* convNew(const Color, const Coordf, const Coordf, const uint, const uint);
Seg* segEndNew(void);
Seg* segNew(const SegType, const Coordf, const Coordf);
Seg* segAppend(Seg *, Seg *);
Seg* segDelete(Seg *, Seg *);
st segListLen(Seg *);
Seg* segListFree(Seg *);

// Objects.h
Obj* objAppend(Obj *, Obj *);
Obj* objDelete(Obj *, Obj *);
st objListLen(Obj *);
Obj* objListFree(Obj *);
Obj* objNew(const ObjType, const Coordf);
Obj* keyNew(const Coordf, const Color);
Obj* spawnNew(const Coordf, const float);
Obj* mobNew(const Coordf, char *);

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
void mapFree(Map *);
Map mapLoad(char *);
void mapSave(Map *);
void mapDefault(Map*);

// WolfenDoge.h
Coord toView(const View, const Coordf, const float);
bool inView(const View, const Coord);
Direction viewBoundIntersect(Seg[4], const Coordf, const Coordf, Coordf *);
bool limitViewBounds(const View, Coord *, Coord *);
Ray* RayInsert(Ray *, Ray *);
Ray* RayNew(const MapPiece, const float, const Coordf);
Ray* RayFree(Ray *);
Ray* castRayBase(const Coordf, const Coordf, Map *);
Ray* castRayMax(const Coordf, const Coordf, Map *, const float);
Ray* castRay(const Coordf, const Coordf, Map *);
Coordf rayUnwrapPos(Ray *);
float rayUnwrapDst(Ray *);
void drawObjSlice(const View, const Ray *, const int, const int, const int, const float);
void drawSegSlice(const View, const Ray *, const int, const int, const int, const float);
void drawFp(const View, Map *, const Player);
void drawBv(const View, Map *, const Player, const float, const Coordf);
void playerMove(Map*);

#endif /* end of include guard: WDDECLS_H */
