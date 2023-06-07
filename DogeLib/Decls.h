#ifndef DECLS_H
#define DECLS_H

// Coords.h
Direction dirROR(const Direction dir);
Direction dirROL(const Direction dir);
Direction dirINV(const Direction dir);
Direction dirLR(const Direction dir);
Direction dirUD(const Direction dir);
bool dirAXA(const Direction dir1, const Direction dir2);
Direction dirPOS(const Direction dir);
Direction dirNEG(const Direction dir);
int coordMin(const Coord coord);
Coord coordLeast(const Coord pos1, const Coord pos2);
int coordMax(const Coord coord);
Coord coordMost(const Coord pos1, const Coord pos2);
bool coordSame(const Coord pos1, const Coord pos2);
int clamp(const int n, const int min, const int max);
bool inBound(const int n, const int min, const int max);
bool inRange(const int n, const Range range);
int wrap(const int n, const int min, const int max);
bool coordInRange(const Coord coord, const Range range);
Coord coordClampLen(const Coord coord, const Length len);
bool coordMaxCoord(const Coord pos1, const Coord pos2);
bool coordMinCoord(const Coord pos1, const Coord pos2);
Rect coordsToRect(const Coord coord1, const Coord coord2);
bool coordNz(const Coord coord);
Coord coordMuli(const Coord coord, const int num);
Coord CoordMul(const Coord coord1, const Coord coord2);
Coord coordInv(const Coord coord);
Coord coordAddi(const Coord coord1, const int num);
Coord coordSub(const Coord coord1, const Coord coord2);
float coordfDist(const Coordf coord1, const Coordf coord2);
Coordf coordfDiv(const Coordf coord, const float num);
Coord coordMod(const Coord coord1, const Coord coord2);
Coord coordModi(const Coord coord, const int num);
Coordf coordfNormalize(const Coordf coord);
Coord coordMid(const Coord coord1, const Coord coord2);
uint coordDistSq(const Coord coord1, const Coord coord2);
Coord coordDivi(const Coord coord, const int num);
Coord coordDiv(const Coord coord1, const Coord coord2);
Coord coordWrap(const Coord coord, const Range x, const Range y);
Coord coordClamp(const Coord coord, const Range x, const Range y);
Coord coordShift(const Coord coord, const Direction dir, const int units);
Coord coordAdd(const Coord coord, const Offset off);
Coordf cfAdd(const Coordf coord, const Offsetf off);
Coord coordCenter(const Coord coord, const Length len);
Coord coordUncenter(const Coord coord, const Length len);
Rect rectify(const Coord pos, const Length len);
Rect rectCenter(Rect r);
Length unrectify(const Rect rect);
bool coordInRect(const Coord coord, const Rect rect);
int coordInRectArr(const Coord coord, Rect *const rect, const int num);
Rect rectOffset(const Rect rect, const Offset off);

// Vec.h
float cfToRad(const Coordf coord);
float radToDeg(const float rad);
float cfToDeg(const Coordf coord);
float degToRad(const float deg);
Coordf radToCf(const float rad);
Coordf degToCf(const float deg);
Coordf CCf(const Coord coord);
Coord CfC(const Coordf coordf);
float fclamp(const float n, const float min, const float max);
bool finBound(const float n, const float min, const float max);
float fwrap(const float n, const float min, const float max);
Coordf cfMul(const Coordf coord1, const Coordf coord2);
Coordf cfMulf(const Coordf coord, const float num);
Coordf cfDivf(const Coordf coord, const float num);
float cfDist(const Coordf coord1, const Coordf coord2);
float cfMag(const Coordf coord);
Coordf radMagToCf(const float rad, const float mag);
Coordf degMagToCf(const float deg, const float mag);
Coordf cfNormalize(const Coordf coord);
Coordf cfTranslate(const Coordf coord, const Vectorf vec);
bool fSameSign(const float num1, const float num2);
Coordf cfNeg(const Coordf coord);
Coordf cfAbs(const Coordf coord);
float cfCfToRad(const Coordf pos1, const Coordf pos2);
float cfCfToDeg(const Coordf pos1, const Coordf pos2);
float degReduce(const float deg);
float degInv(const float deg);
float cf3Rad(const Coordf vtx, const Coordf pos1, const Coordf pos2);
float cf3Deg(const Coordf vtx, const Coordf pos1, const Coordf pos2);
Coordf cfRotateRad(const Coordf vec, const float n);
Coordf cfRotateDeg(const Coordf vec, const float d);

// Timing.h
static inline
uint getTicks(void);
static inline
uint secondsToTicks(const uint sec);
static inline
uint getTimeIn(const uint sec);

// Graphics.h
static inline
void setWindowMode(const WindowMode mode);
static inline
void setWindowResizable(const bool resizable);
static inline
bool getWindowResizable(void);
static inline
void setWindowLen(const Length len);
static inline
Length getWindowLen(void);
static inline
bool windowResized(void);
static inline
Length getWindowMid(void);
static inline
bool coordInWindow(const Coord pos);
static inline
Length maximizeWindow(void);
static inline
Length restoreWindow(void);
static inline
void setWindowSize(const uint x, const uint y);
static inline
void setBlend(const BlendMode mode);
static inline
void drawPixel(const uint x, const uint y);
static inline
void drawPixelCoord(const Coord pos);
static inline
void drawLine(const uint x1, const uint y1, const uint x2, const uint y2);
static inline
void drawLineCoords(const Coord pos1, const Coord pos2);
static inline
void drawLineCoordPair(const CoordPair pair);
static inline
void drawHLine(const uint x, const uint y, int len);
static inline
void drawVLine(const uint x, const uint y, int len);
static inline
void drawRect(const uint x, const uint y, const uint xlen, const uint ylen);
static inline
void drawRectCentered(const uint x, const uint y, const uint xlen, const uint ylen);
static inline
void fillRect(const uint x, const uint y, const uint xlen, const uint ylen);
static inline
void fillRectCentered(const uint x, const uint y, const uint xlen, const uint ylen);
static inline
void drawRectCoords(const Coord pos1, const Coord pos2);
static inline
void drawRectCenteredCoords(const Coord pos1, const Coord pos2);
static inline
void fillRectCoords(const Coord pos1, const Coord pos2);
static inline
void fillRectCenteredCoords(const Coord pos1, const Coord pos2);
static inline
void drawRectCoordLength(const Coord pos, const Length len);
static inline
void drawRectCenteredCoordLength(const Coord pos, const Length len);
static inline
void fillRectCoordLength(const Coord pos, const Length len);
static inline
void fillRectCenteredCoordLength(const Coord pos, const Length len);
static inline
void drawRectRect(const Rect rect);
static inline
void fillRectRect(const Rect rect);
static inline
void drawSquareCoord(const Coord pos, const uint len);
static inline
void drawSquare(const uint x, const uint y, const uint len);
static inline
void fillSquare(const uint x, const uint y, const uint len);
static inline
void fillSquareResize(const uint x, const uint y, const uint len, const int resize);
static inline
void fillSquareCoord(const Coord pos, const uint len);
static inline
void fillSquareCoordResize(const Coord pos, const uint len, const int resize);
static inline
void fillBorder(uint x, uint y, uint xlen, uint ylen, int b);
static inline
void fillBorderCoords(const Coord pos, const Length len, const int b);
static inline
void fillBorderCoordSquare(const Coord pos, const uint len, const int b);
static inline
void drawCircle(const uint x, const uint y, const uint radius);
static inline
void fillCircle(const uint x, const uint y, const uint radius);
static inline
void drawCircleCoord(const Coord pos, const uint radius);
static inline
void fillCircleCoord(const Coord pos, const uint radius);
static inline
Color getColor(void);
static inline
void setColor(const Color c);
static inline
u32 colorToU32(const Color c);
static inline
Color u32ToColor(const u32 u);
static inline
void drawTri(const Coord pos1, const Coord pos2, const Coord pos3);
static inline
void fillTri(const Coord pos1, const Coord pos2, const Coord pos3);
static inline
void setRGB(const u8 r, const u8 g, const u8 b);
static inline
void setRGBA(const u8 r, const u8 g, const u8 b, const u8 a);
static inline
void fillScreen(void);
static inline
void clear(void);
static inline
void draw(void);
static inline
void gfx_quit(void);
static inline
void gfx_init(void);
void drawPoly(Coord *pos, const uint num);
void fillPoly(Coord *pos, const uint num);
void bezier(const Coord *pos, const uint numc, const uint nums);
void outlineWindow(void);
uint frameStart(void);
void frameEnd(const uint t);

// Text.h
Img* textImg(const char *text);
Texture* textTexture(const char *text);
Length drawText(const char *text, const int x, const int y);
Coord drawTextCoord(const char *text, const Coord pos);
Length drawTextCentered(const char *text, const int x, const int y);
Length drawTextCenteredCoord(const char *text, const Coord pos);
Length getTextLength(const char *text);
int getTextXLen(const char *text);
int getTextYLen(const char *text);
bool strEndsWith(const char *str, const char *end);
Coord* spanTextListCoords(Coord *coords, const Coord start, const Coord stop, const uint num);
Coord* spanTextListCoordsCentered(Coord *coords, const Coord start, const Coord stop, const uint num);
Rect* getTextListRect(const char **textList, Rect *const rect, const Coord start, const Coord stop, const uint num);
Rect* getTextListRectCentered(const char **textList, Rect *const rect, const Coord start, const Coord stop, const uint num);
int coordInRectList(const Coord coord, Rect *const rect, const int num);
int coordInTextList(const Coord coord, const Coord start, const Coord stop, const uint num, const char **textList);
void spanTextList(const char **textList, const Coord start, const Coord stop, const uint num);
void spanTextListCentered(const char **textList, const Coord start, const Coord stop, const uint num);
void setTextSize(const uint size);
int getTextSize(void);
void setTextColor(const Color c);
Color getTextColor(void);
void text_quit(void);
void text_init(void);

// Image.h
static inline
void freeImg(Img *image);
static inline
void freeTexture(Texture *t);
static inline
Img* loadImg(const char *imgFile);
static inline
Texture* imgTexture(Img *img);
static inline
Texture* loadTexture(const char *imgFile);
static inline
void drawImg(Img *image);
static inline
void loadDrawImg(const char *imgFile);
static inline
Length textureLen(Texture *texture);
static inline
Rect textureRect(Texture *texture, const Coord pos);
static inline
Length drawTexture(Texture *texture, const int x, const int y);
static inline
Length drawTextureCoord(Texture *texture, const Coord pos);
static inline
Length drawTextureCentered(Texture *texture, const int x, const int y);
static inline
Length drawTextureCenteredCoord(Texture *texture, const Coord pos);
static inline
Length drawTextureResize(Texture *texture, const int x, const int y, const int xlen, const int ylen);
static inline
Length drawTextureCoordResize(Texture *texture, const Coord pos, const Length len);
static inline
Length drawTextureCenteredResize(Texture *texture, const int x, const int y, const int xlen, const int ylen);
static inline
Length drawTextureCenteredCoordResize(Texture *texture, const Coord pos, const Length len);
void img_quit(void);
void img_init(void);

// Input.h
bool keyState(const Scancode key);
bool keyPressed(const Scancode key);
bool keyHeld(const Scancode key);
bool keyChanged(const Scancode key);
bool keyReleased(const Scancode key);
bool mouseBtnPressed(const u32 mouseBtn);
bool mouseBtnState(const u32 mouseBtn);
bool mouseBtnHeld(const u32 mouseBtn);
bool mouseBtnReleased(const u32 mouseBtn);
bool mouseBtnChanged(const u32 mouseBtn);
bool mouseScrolled(const u32 mouseWheel);
bool mouseMoving(void);
bool mouseMoveStart(void);
bool mouseMoveStop(void);
Length mouseMovement(void);
void input_init(void);

// Events.h
void events(const uint endOfFrame);

// File.h
uint fileLen(const char *filePath);
char* fileReadText(const char *filePath);

// DogeUtil.h
void clearTerminal(void);
float randRange(float a, float b);
void init(void);

// Poly.h
Poly polyCreate(const uint numVertex, ...);
void polyDestroy(const Poly p);
Rect polyBbRect(const Poly p);
CoordPair polyBbMinMax(const Poly p);

// Display.h
int getWinDisplayIndex(void);
Rect getWinDisplayRect(void);
Length getWinDisplayLen(void);
Length getDisplayIndexLen(const uint i);
Rect getDisplayIndexRect(const uint i);
Coord getWinDisplayOffset(void);
Coord getDisplayIndexOffset(const uint i);
uint getDisplayNum(void);
Length getDisplayTotalLen(void);

#endif /* end of include guard: DECLS_H */
