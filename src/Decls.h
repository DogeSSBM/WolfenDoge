#ifndef WDDECLS_H
#define WDDECLS_H

// WolfenDoge.h
// --------------------------------------------------------------------------------------
// scales and offsets pos into view
Coord toView(const View view, const Coordf pos, const float scale);
// returns true if pos is in view
bool inView(const View view, const Coord pos);
// returns the direction of the boundary of the segment that is intersected by the line segment ab
Direction viewBoundIntersect(Seg bounds[4], const Coordf a, const Coordf b, Coordf *at);
// if line segment ab goes out of bounds of view, sets ab to the point where the boundary is intersected
bool limitViewBounds(const View view, Coord *a, Coord *b);
// inserts ins such that list will be sorted greatest dst to least
Ray* RayInsert(Ray *list, Ray *ins);
// allocates a new Ray
Ray* rayNew(const MapPiece piece, const float dst, const float rayAng, const Coordf origin, const Coordf pos);
// frees head of list, returning next
Ray* rayFree(Ray *list);
// casts ray from origin to distantPoint and returns nearest 'solid' intersection
// where 'solid' refers to a wall segment, a fully closed door segment
Ray* castRayBase(const Coordf origin, const Coordf distantPoint, const float rayAng, Map *map);
// casts ray from origin to distantPoint and returns all intersections that are < max distance away
Ray* castRayMax(const Coordf origin, const Coordf distantPoint, const float rayAng, Map *map, const float min, const float max);
// creates ordered list of all intersections in between origin and base intersection
Ray* castRay(const Coordf origin, const Coordf distantPoint, const float rayAng, Map *map);
// frees sect, returning its pos
Coordf rayUnwrapPos(Ray *sect);
// if there are no map segments to intersect with, no rays will be allocated
// in this case we return a large float as if the ray traveled forever
// frees sect, returning its dst
float rayUnwrapDst(Ray *sect);
// draws vertical slice of object based on ray information
void drawObjSlice(const View view, const Ray *rs, const int xpos, const int ymid, const int dst, const float hsec);
// draws vertical slice of segment based on ray information
void drawSegSlice(const View view, const Ray *rs, const int xpos, const int ymid, const int dst, const float hsec, Map *map);
// draws ceiling and floor
void drawCeilFloor(const View view);
// draw first person view
void drawFp(const View view, Map *map, const Player player);
// draw birds eye view
void drawBv(const View view, Map *map, const Player player, const float scale, const Coordf off);
// moves the player
void playerMove(Map *map);

// Segments.h
// --------------------------------------------------------------------------------------
// creates a new segment with type S_WALL
Seg* wallNew(const Color c, const Coordf a, const Coordf b);
// creates a new segment with type S_WALL that has a texture
Seg* txtrNew(const Color c, const Coordf a, const Coordf b, char *path);
// creates a new segment with type S_WIND
Seg* windNew(const Color c, const Color topColor, const Coordf a, const Coordf b, const float height, const float top);
// creates a new segment with type S_DOOR
Seg* doorNew(const Color c, const Coordf a, const Coordf b, const uint id, const float pos, const bool state, const float speed, const Direction closeDir);
// creates a new segment with type S_TRIG
Seg* trigNew(const Color color, const TrigType type, const Coordf a, const Coordf b, const uint id, const Coordf c, const Coordf d);
// creates a new segment with type S_END
// (for denoting end of segment portion of map when saving / reading to file)
Seg* segEndNew(void);
// creates a new segment
Seg* segNew(const SegType type, const Coordf a, const Coordf b);
// duplicates segment (deep copy)
Seg* segDup(Seg *seg);
// appends tail to the end of the list (head)
Seg* segAppend(Seg *head, Seg *tail);
// frees segment (deep free)
Seg* segFree(Seg *seg);
// searches for del in segList, removes and frees (deep)
Seg* segDelete(Seg *segList, Seg *del);
// returns the number of segments in the list
st segListLen(Seg *segList);
// frees all segments in segList
Seg* segListFree(Seg *segList);
// honestly idk
float triSign(const Coordf a, const Coordf b, const Coordf c);
// returns true if pos is within triangle abc
bool cfInTri(const Coordf pos, const Coordf a, const Coordf b, const Coordf c);
// returns true if pos is within quad
bool cfInQuad(const Coordf pos, const Coordf a, const Coordf b, const Coordf c, const Coordf d);
// returns true if pos in trigger zone
bool cfInTrig(const Coordf pos, Seg *trig);
// returns first available trigger in map if cur is NULL
// returns first trigger after cur if cur is not NULL
// returns NULL if none left
Seg* trigQueryId(Map *map, Seg *cur, const uint id);

// Objects.h
// --------------------------------------------------------------------------------------
// appends tail to the end of the list (head)
Obj* objAppend(Obj *head, Obj *tail);
// frees obj (deep free)
Obj* objFree(Obj *obj);
// searches for del in objList, removes and frees (deep)
Obj* objDelete(Obj *objList, Obj *del);
// returns the number of objects in the list
st objListLen(Obj *objs);
// frees all objects in the list
Obj* objListFree(Obj *list);
// creates a new object
Obj* objNew(const ObjType type, const Coordf pos);
// duplicates object
Obj* objDup(Obj *obj);
// creates a new object of type O_KEY
Obj* keyNew(const Coordf pos, const Color c);
// creates a new object of type O_SPAWN
Obj* spawnNew(const Coordf pos, const float ang);
// creates a new object of type O_MOB
Obj* mobNew(const Coordf origin, char *path);
// creates a new segment with type S_CONV
Obj* convNew(const ConvType type, const Coordf pos, const uint inIdA, const uint inIdB, const uint outId);

// Pieces.h
// --------------------------------------------------------------------------------------
// returns a MapPiece of type M_SEG containing seg
MapPiece segToPiece(Seg *seg);
// returns a MapPiece of type M_OBJ containing obj
MapPiece objToPiece(Obj *obj);
// returns true if piece has type indicating it does not contain a seg or obj
// panics if it should contain a piece but its respective pointer is NULL
bool pieceEmpty(const MapPiece piece);
// returns struct containing all of the pieces coords
PieceCoords pieceCoords(const MapPiece piece);
// returns struct containing all of the piece from fields coords
PieceCoords fieldCoords(const PieceFields fields);
// returns true if pos is one of the pieces coords
bool pieceContainsCoord(const MapPiece piece, Coordf *pos);
// if pos is not one of pieces coords returns -1
// otherwise returns the index of pieces PieceCoords array that contains pos
int pieceCoordIndex(const MapPiece piece, Coordf *pos);
// returns primary color of segment / object contained by piece
// panics if piece is empty
Color pieceColor(const MapPiece piece);
// removes piece from map and frees it (deep)
void pieceDelete(Map *map, MapPiece piece);
// returns the next object / segment in map (wraps)
// if piece is M_ANY, returns first available object / segment in map
// if map is empty, returns piece with type M_NONE
MapPiece pieceNext(Map *map, MapPiece piece);
// points *pos to the next available coord in the map
// returns the piece that contains that coord
MapPiece pieceNextCoord(Map *map, MapPiece piece, Coordf **pos);
// points *pos to the next available coord in the map
// returns the piece that contains that coord
MapPiece pieceNextSameCoord(Map *map, MapPiece piece, Coordf **pos);
// true if both components of pos are within the bounds of min (inclusive) and max (inclusive)
bool cfInBounds(const Coordf pos, const Coordf min, const Coordf max);
// returns true if a and b contain the same segment / object
// or if a and b are both M_NONE / M_ANY
bool pieceSame(const MapPiece a, const MapPiece b);
// returns a copy (deep) of piece
MapPiece pieceDup(const MapPiece piece);
// returns a copy (deep) of fields
PieceFields pieceFieldsDup(const PieceFields fields);
// points *nearestPos to the coord of a piece that is closest to pos
// returns the piece that contains that coord
// returns piece with type M_NONE if map is empty
MapPiece pieceNearest(Map *map, const Coordf pos, Coordf **nearestPos);
// returns total number of segments + objects in map
st pieceCountTotal(Map *map);
// returns a piece containing a new segment / object with coords a and b
MapPiece pieceNew(const NewPieceInfo pieceInfo, const Coordf a, const Coordf b);

// Fields.h
// --------------------------------------------------------------------------------------
// prints field along with its label
void fieldPrint(const MapPiece piece);
// draws field with type MapPieceType
Coord fieldDrawMapPieceType(const MapPieceType val, char *label, Coord pos, const uint selected);
// draws field with type SegType
Coord fieldDrawSegType(const SegType val, char *label, Coord pos, const uint selected);
// draws field with type ObjType
Coord fieldDrawObjType(const ObjType val, char *label, Coord pos, const uint selected);
// draws field with type ConvType
Coord fieldDrawConvType(const ConvType val, char *label, Coord pos, const uint selected);
// draws field with type TrigType
Coord fieldDrawTrigType(const TrigType val, char *label, Coord pos, const uint selected);
// draws field with type Coordf
Coord fieldDrawCoordf(const Coordf val, char *label, Coord pos, const uint selected);
// draws field with type Color
Coord fieldDrawColor(const Color val, char *label, Coord pos, const uint selected);
// draws field with type Path
Coord fieldDrawPath(char *val, char *label, Coord pos, const uint selected);
// draws field with type Float
Coord fieldDrawFloat(const float val, char *label, Coord pos, const uint selected);
// draws field with type Uint
Coord fieldDrawUint(const uint val, char *label, Coord pos, const uint selected);
// draws field with type Bool
Coord fieldDrawBool(const bool val, char *label, Coord pos, const uint selected);
// draws field with type Dir
Coord fieldDrawDir(const Direction val, char *label, Coord pos, const uint selected);
// draws field, if selected > 0, highlights field as selected
// for fields with multiple entries, highlights entry in index of selected+1
Coord fieldDraw(const Field field, const Coord origin, const uint selected);
// prints all of the pieces of the map
void fieldPrintMap(Map *map);

// GetFields.h
// --------------------------------------------------------------------------------------
// fills in fields for segment type pieces
PieceFields segFields(const MapPiece piece);
// fills in fields for object type pieces
PieceFields objFields(const MapPiece piece);
// fills in fields for given piece
PieceFields pieceFields(const MapPiece piece);

// Map.h
// --------------------------------------------------------------------------------------
// returns max coord of segList bounding box
Coordf segListBoundMax(Seg *segList);
// returns max coord of map segments bounding box
Coordf mapSegBoundMax(Seg *seg[S_N]);
// returns min coord of segList bounding box
Coordf segListBoundMin(Seg *segList);
// returns min coord of map segments bounding box
Coordf mapSegBoundMin(Seg *seg[S_N]);
// returns length of map segments bounding box
Coordf mapSegBoundLen(Seg *seg[S_N]);
// converts a coordinate relative to the window to a map coordinate
Coordf screenToMap(const Coord off, const float scale, const Coord pos);
// converts a map coordinate to a coordinate relative to the window
Coord mapToScreen(const Coord off, const float scale, const Coordf pos);
// sets state of all doors with matching id
void mapDoorSet(Map *map, const uint id, const bool state);
// resets all door states to false
void mapDoorReset(Map *map);
// returns true if any triggers with matching id are true
bool mapTrigQuery(Map *map, const uint id);
// updates all doors on map
void mapDoorUpdate(Map *map);
// updates all map triggers
void mapTrigUpdate(Map *map);
// updates all map objects
void mapObjUpdate(Map *map);
// updates map
void mapUpdateDynamics(Map *map);
// adds piece to map
void mapAddPiece(Map *map, const MapPiece piece);

// MapIO.h
// --------------------------------------------------------------------------------------
// gets length of map name, allocates memory for it, parses it
// returns its strlen
st mapParseName(Map *map);
// resets player health / speed
// sets player pos / ang to random spawn from map->obj[O_SPAWN]
// if there are no spawn objs, places at random ang and pos in map bounding box
void mapSpawnPlayer(Map *map);
// returns already loaded texture with matching path if exists
// else returns newly allocated texture
Texture* wallListTxtrQryLoad(Seg *wallList, char *path);
// returns already loaded texture with matching path if exists
// else returns newly allocated texture
Texture* mobListTxtrQryLoad(Obj *mobList, char *path);
// parses map segments into their respective SegType index
void mapParseSegments(Map *map);
// parses map objects into their respective ObjType index
void mapParseObjects(Map *map);
// parses map file
void mapParseFile(Map *map);
// attempts to open ../Maps/map.bork then ../Maps/map(n).bork with n starting at 1 and increasing
// once a file name that doesnt exist is found sets map.name to it
void newMapFileNum(Map *map);
// sets seg's texture to NULL, checks to see if any other segments in map have
// same texture, if none do, the texture is freed
Seg* wallListTxtrCleanup(Seg *wallList, Seg *seg);
// applys a texture to all wall segments that match path
Seg* wallListTxtrApply(Seg *wallList, Texture *txtr, char *path);
// Loads default map segments
void mapDefaultSegments(Map *map);
// loads default map objects
void mapDefaultObjects(Map *map);
// loads default map
void mapDefault(Map *map);
// frees contents of map
void mapFree(Map *map);
// attempts to load map file at ./Maps/name if present
// if not present, sets map.path to ../Maps/map.bork or ../Maps/map(n).bork
// starting at n=1 and increasing until unique file path is found
Map mapLoad(char *name);
// saves map to map->path
void mapSave(Map *map);

// MapEditor.h
// --------------------------------------------------------------------------------------
// initial struct data for editor state
EditorState editorInitState(void);
// main loop while in map editor. on save, map is saved to map->path
EditorState mapEdit(Map *map, EditorState state);

// MapEditorDraw.h
// --------------------------------------------------------------------------------------
// draws X and Y axis lines if visible
void editorDrawOriginLines(const Offset off, const Length wlen);
// draws grid lines with snapLen distance between each line
void editorDrawGrid(const Offset off, const Length wlen, const float scale, const float snapLen);
// when only a single piece is selected, draws its fields
void editorDrawPieceFields(Selection *sel);
// draw or fill in a circle at pos depending on selected state
void circleCoord(const Coord pos, const bool selected);
// draws a piece in the editor
void editorDrawPiece(const MapPiece piece, const Offset off, const float scale, Selection *sel);
// draws the total number of pieces
void editorDrawPieceCount(Map *map, const Coord wlen, Selection *list);
// draws x / y axies through origin and snap grid if enabled
void editorDrawLines(const Snap snap, const Camera cam);
// draws whole map
void editorDrawMap(Map *map, const Offset off, const float scale, Selection *sel);
// when no pieces are selected, displays the type of piece that will be created upon new piece creation
void editorDrawNewPieceType(const NewPieceInfo pieceInfo, const Length wlen);
// draws circles / lines / selection box when dragging right mouse
void editorDrawNewPiecePos(const MouseWin wmouse);

// MapEditorInput.h
// --------------------------------------------------------------------------------------
// returns true if escape pressed with no active selection
bool editorInputExit(Map *map, Selection *sel);
// updates camera on window resize
void editorInputResizeWindow(Camera *cam);
// if there is a selection, pressing escape will clear it
void editorInputClearSelection(Selection **sel);
// toggles snap on or off
// on ctrl + scroll, changes snap len
void editorInputSnap(Snap *snap);
// sets mouse map / win positions
void editorInputMouseMove(const Camera cam, Mouse *mouse, Snap *snap);
// sets ldown / rdown map / win positions on left / right click
void editorInputMouseBtns(Mouse *mouse, Snap *snap);
// performs single selection on left click when current selection inactive
// left click while holding ctrl for multiple selections
void editorInputSelect(Map *map, const Coordf pos, Coord *cursor, Selection **sel);
// changes most recent selection to next available map piece with same coords as current selected pos
void editorInputNextSelection(Map *map, Selection *sel);
// while selection is active, changes the field currently highlighted by the cursor
void editorInputMoveCursor(Selection *sel);
// saves map when ctrl s is pressed
void editorInputSave(Map *map);
// Changed the type of new piece
void editorInputNewPiece(Selection *sel, NewPieceInfo *pieceInfo);
// zooms editor in or out focused on cursor
void editorInputZoom(Camera *cam, const Mouse mouse);
// pans the map on mouse movement while holding middle mouse / left shift
void editorInputPan(Offset *off);
// function that returns true when done editing current field
bool done(void);
// returns the int corrosponding to the number key pressed
// -1 if no number keys were pressed
int numKeyPressed(void);
// main loop while editing a text field
void mapEditText(Map *map, EditorState *state, char *text);
// main loop while editing a text field
void mapEditFloat(Map *map, EditorState *state, float *f);
// main loop while editing a uint field
void mapEditUint(Map *map, EditorState *state, uint *u);
// main loop while editing a u8 field
void mapEditU8(Map *map, EditorState *state, u8 *u);

// MapEditorUpdate.h
// --------------------------------------------------------------------------------------
// deleted all selected pieces
void editorUpdateDeleteSelection(Map *map, Selection **sel);
// moves all selected coords
void editorUpdateMoveSelection(const Camera cam, const Snap snap, const MouseMap mmouse, Selection *sel);
// creates a new map piece once right mouse button is released
void editorUpdateNewPiece(Map *map, const NewPieceInfo pieceInfo, const Snap snap, const Mouse mouse);
// selects the correct edit function given the cursor position and its corrosponding field
void editorUpdateSelectionVal(Map *map, EditorState *state);
// adds all coords within selection box to selection
void editorUpdateBoxSelect(Map *map, Coord *cursor, const Mouse mouse, Selection **list);
// creates a copy of all selected pieces, deselects whole selection list
// and selects the copies
void editorUpdateSelectionDup(Map *map, const Coordf pos, Selection **list);

// MapEditorSelection.h
// --------------------------------------------------------------------------------------
// allocates and returns a new selection
Selection* selNew(Coord *cursor, Coordf *pos, const PieceFields fields);
// appends selection to end of list
Selection* selAppend(Selection *head, Selection *tail);
// returns number of selections in list
st selListLen(Selection *list);
// frees single selection
// returns pointer to next
// does not free piece contained in selection
Selection* selFree(Selection *list);
// frees selection list
// does not free piece contained in selection
Selection* selFreeList(Selection *sel);
// returns a selection containing the piece and pos nearest to pos
Selection* selPosNearest(Map *map, Coord *cursor, const Coordf pos);
// returns last selection in list
Selection* selLast(Selection *sel);
// returns selection containing pos in selection list else NULL
Selection* selPosSelected(Selection *list, Coordf *pos);
// returns first selection containing piece in selection list else NULL
Selection* selPieceSelected(Selection *list, const MapPiece piece);
// searches for piece in selection list, if found returns selection else NULL
Selection* selPieceSelection(Selection *list, const MapPiece piece);
// returns piece if sel piece occurs in list up to sel (exclusive);
bool selPieceFieldsBefore(Selection *list, Selection *sel);
// replaces all occurences of a with b
void selListSetPiece(Selection *list, const PieceFields a, const PieceFields b);
// ensures all positions of each piece in the selection list is selected
// adds any missing position selections to the head of the list
// returns head of list
Selection* selListAddAllPiecePos(Selection *list);
// duplicates each unique piece in the list, swapping them out for the old ones
void selListDupAddUniquePieces(Map *map, Selection *list);
// prints the selection list
void selPrint(Selection *list);

#endif /* end of include guard: WDDECLS_H */
