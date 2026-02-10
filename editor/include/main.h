#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include <stdbool.h>

//-----------------------------------------------------------
// Constants
//-----------------------------------------------------------
#define MAX_MAP_WIDTH       64
#define MAX_MAP_HEIGHT      64
#define MAX_PATH_LEN        512
#define MAX_FILENAME_LEN    256

#define TOOLBAR_HEIGHT      30      // base values – multiplied by uiScale at runtime
#define SPRITE_PANEL_WIDTH  300
#define DEFAULT_CELL_SIZE   32
#define MIN_CELL_SIZE       8
#define MAX_CELL_SIZE       128

//-----------------------------------------------------------
// Global UI scale (computed once at startup from monitor DPI)
//-----------------------------------------------------------
extern float uiScale;

// Convenience: scale an integer constant
#define S(v) ((int)((v) * uiScale))

//-----------------------------------------------------------
// Dialog Types
//-----------------------------------------------------------
typedef enum {
    DIALOG_NONE = 0,
    DIALOG_NEW_MAP,
    DIALOG_TILE_SIZE,
} DialogType;

//-----------------------------------------------------------
// Editor Map  (mirrors engine map_t but editor-friendly)
//   map.txt format (extended):
//     line 1 : sprite_filename tile_w tile_h
//     line 2 : map_w map_h
//     line 3 : start_x start_y
//     lines 4+: row data (space-separated ints)
//-----------------------------------------------------------
typedef struct {
    int  data[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];
    int  width;
    int  height;
    int  startX;
    int  startY;
    char spriteFile[MAX_FILENAME_LEN];
    int  spriteTileW;
    int  spriteTileH;
    bool active;
} EditorMap;

//-----------------------------------------------------------
// Sprite Sheet
//-----------------------------------------------------------
typedef struct {
    Texture2D texture;
    int  tileWidth;
    int  tileHeight;
    int  cols;
    int  rows;
    int  totalTiles;
    int  selectedTile;   // 0 = eraser / none, 1+ = tile index
    bool loaded;
    char filePath[MAX_PATH_LEN];
    char fileName[MAX_FILENAME_LEN];
} SpriteSheet;

#endif // MAIN_H
