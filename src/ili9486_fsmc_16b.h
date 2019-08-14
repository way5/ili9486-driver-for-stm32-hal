/*
#####################################################################################
# File: ili9486_fsmc_16b.h                                                          #
# Project: SINIS3D                                                                  #
# File Created: Sunday, 16th June 2019 1:12:36 am                                   #
# Author: sk                                                                        #
# Last Modified: Tuesday, 13th August 2019 8:19:06 pm                               #
# Modified By: sk                                                                   #
# License: GPLv3                                                                    #
#####################################################################################
# CHANGELOG:                                                                        #
#####################################################################################
*/

#ifndef _ILI9486_ILI9486_Display_Driver_H_
#define _ILI9486_ILI9486_Display_Driver_H_

#include "fsmc.h"
#include "spi.h"
#include "gpio.h"

#define _DISPLAY_WIDTH                 480
#define _DISPLAY_HEIGHT                320
// memory map
#define FSMC_LCD_CMD                   ((uint32_t *)0x60000000)
#define FSMC_LCD_DATA                  ((uint32_t *)0x60020000)
// MAD
#define _WRITE_CMD(x)                  (*((__IO uint16_t *)FSMC_LCD_CMD)) = x
#define _WRITE_DATA(x)                 (*((__IO uint16_t *)FSMC_LCD_DATA)) = x
#define _READ_DATA()                   (*((__IO uint16_t *)FSMC_LCD_DATA))
// utils
#define SWAP_INT16(a, b)               { int16_t t = a; a = b; b = t; }
// most used commands
#define _DISPLAY_INVOFF	    0x20
#define _DISPLAY_INVON	    0x21
#define _DISPLAY_COLADDSET  0x2A
#define _DISPLAY_PAGEADDSET 0x2B
#define _DISPLAY_RAMWR	    0x2C
#define _DISPLAY_MADCTL	    0x36
// Color definitions
#define BLACK 0x0000       /*   0,   0,   0 */
#define NAVY 0x000F        /*   0,   0, 128 */
#define DARKGREEN 0x03E0   /*   0, 128,   0 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define LIGHTGREY 0xC618   /* 192, 192, 192 */
#define DARKGREY 0x7BEF    /* 128, 128, 128 */
#define BLUE 0x001F        /*   0,   0, 255 */
#define GREEN 0x07E0       /*   0, 255,   0 */
#define CYAN 0x07FF        /*   0, 255, 255 */
#define RED 0xF800         /* 255,   0,   0 */
#define MAGENTA 0xF81F     /* 255,   0, 255 */
#define YELLOW 0xFFE0      /* 255, 255,   0 */
#define WHITE 0xFFFF       /* 255, 255, 255 */
#define ORANGE 0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define PINK 0xF81F
// touch screen controller
#define Z_THRESHOLD             20
#define TEMPQ_THRESHOLD         20000u
#define XYZQ_THRESHOLD          500u
#define X_RESIST                259.0

// use built-in fonts
// #define USE_DRIVER_FONTS    1
#ifdef USE_DRIVER_FONTS
// @CREDITS <Adafruit GFX>
typedef struct {
    uint16_t bitmapOffset;     ///< Pointer into GFXfont->bitmap
    uint8_t  width;            ///< Bitmap dimensions in pixels
    uint8_t  height;           ///< Bitmap dimensions in pixels
    uint8_t  xAdvance;         ///< Distance to advance cursor (x axis)
    int8_t   xOffset;          ///< X dist from cursor pos to UL corner
    int8_t   yOffset;          ///< Y dist from cursor pos to UL corner
} GFXglyph;

typedef struct {
    uint8_t  *bitmap;          ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;           ///< Glyph array
    uint8_t   first;           ///< ASCII extents (first char)
    uint8_t   last;            ///< ASCII extents (last char)
	uint8_t   yAdvance;        ///< Newline distance (y axis)
} GFXfont;

#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSansBold9pt7b.h"

// #define USE_FONT_K12
#ifdef USE_FONT_K12
    #include "fonts/FreeSans12pt7b.h"
    #include "fonts/FreeSansBold12pt7b.h"
#endif
// #define USE_FONT_K18
#ifdef USE_FONT_K18
    #include "fonts/FreeSans18pt7b.h"
    #include "fonts/FreeSansBold18pt7b.h"
#endif
#endif
// @CREDITS <Adafruit GFX> END

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
    uint16_t xR;
    uint16_t yR;
} TPoint;

class ILI9486_Display_Driver {
    public:
        ILI9486_Display_Driver() { setFont(); };
        void begin();
        void reset();
        void softReset();
        void setRotation(uint8_t x = 0);
        void invertDisplay(bool i);
        void canvas(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
        void setCursor ( uint16_t x, uint16_t y );
        uint16_t getWidth(),
                getHeight();
        uint8_t getRotation();
        void fillScreen(uint16_t color);
        void clearScreen(uint16_t color = BLACK);
        void setFont(uint8_t kern = 0, bool bold = false);
        void drawPixel(uint16_t x, uint16_t y, uint16_t color);
        void drawScreenPixelBuffer(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t * color_arr);
        void drawLine(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t color);
        void drawRect(int16_t x, int16_t y, int16_t x1, int16_t y1, uint16_t color, bool fill = false);
        void drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, bool fill = false);
        void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool fill = false);
        #ifdef USE_DRIVER_FONTS
        GFXfont const * getFont();
        void drawChar(uint16_t x, uint16_t y, const char *c, uint16_t color, uint16_t bkg);
        void drawString(uint16_t x, uint16_t y, const char * str, uint16_t color, uint16_t bkg, bool dirVert = false, bool dirInv = false);
        #endif
        void drawBitmap(uint16_t x, uint16_t y, const uint8_t *bmp, int16_t w, int16_t h, uint16_t color, uint16_t bg);
        void drawBitmapRGB(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
        void drawBitmapRGB(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
        uint16_t getPixelData(),
                 getPixel(uint16_t x, uint16_t y);
        uint32_t getID();
        void idleModeOn();
        void idleModeOff();
        void standbyModeOn();
        void standbyModeOff();
        void invert();
        void delay(__IO uint32_t cnt);
        TPoint * getPoint();
        bool isTouched();
        uint16_t getPressure(),
                 getCtrlTemp();
        void touchEventHandler();
    private:
        uint8_t  _rotation = 0,
                 _inverted = 0;
        uint16_t _width = _DISPLAY_WIDTH,
                 _height = _DISPLAY_HEIGHT,
                 _temp = 0;
        int16_t _xRaw = 0,
                _yRaw = 0,
                _zRaw = 0;
        volatile bool _is_touched = false;
        TPoint _tPoint;
        #ifdef USE_DRIVER_FONTS
        GFXfont const * _font;
        #endif
	    uint32_t lastTouched = 0x80000000,
                 lastTempQuery = 0x80000000;
        void fill(uint16_t color, uint32_t length);
        void clear(uint16_t x,uint16_t y,uint16_t x2, uint16_t y2, uint16_t color);
        bool checkCtrl();
        void checkCtrlTemp();
};

#endif