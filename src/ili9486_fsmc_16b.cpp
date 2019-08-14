/*
#####################################################################################
# File: ili9486_fsmc_16b.cpp                                                        #
# Project: SINIS3D                                                                  #
# File Created: Sunday, 16th June 2019 1:14:08 am                                   #
# Author: sk                                                                        #
# Last Modified: Tuesday, 13th August 2019 8:19:06 pm                               #
# Modified By: sk                                                                   #
# License: GPLv3                                                                    #
#####################################################################################
# CHANGELOG:                                                                        #
#####################################################################################
*/

#include "ili9486_fsmc_16b.h"

/**
 * @brief initialize
 * 
*/
void ILI9486_Display_Driver::begin() {
    // Interface Mode Control
    _WRITE_CMD(0xb0);
    _WRITE_DATA(0x00);
    // Interface Pixel Format, 16 bits / pixel
    _WRITE_CMD(0x3A);
    _WRITE_DATA(0x55); // 5D
    // PGAMCTRL(Positive Gamma Control)
    _WRITE_CMD(0xE0);
    _WRITE_DATA(0x0F);
    _WRITE_DATA(0x1F);
    _WRITE_DATA(0x1C);
    _WRITE_DATA(0x0C);
    _WRITE_DATA(0x0F);
    _WRITE_DATA(0x08);
    _WRITE_DATA(0x48);
    _WRITE_DATA(0x98);
    _WRITE_DATA(0x37);
    _WRITE_DATA(0x0A);
    _WRITE_DATA(0x13);
    _WRITE_DATA(0x04);
    _WRITE_DATA(0x11);
    _WRITE_DATA(0x0D);
    _WRITE_DATA(0x00);
    // NGAMCTRL (Negative Gamma Correction)
    _WRITE_CMD(0xE1);
    _WRITE_DATA(0x0F);
    _WRITE_DATA(0x32);
    _WRITE_DATA(0x2E);
    _WRITE_DATA(0x0B);
    _WRITE_DATA(0x0D);
    _WRITE_DATA(0x05);
    _WRITE_DATA(0x47);
    _WRITE_DATA(0x75);
    _WRITE_DATA(0x37);
    _WRITE_DATA(0x06);
    _WRITE_DATA(0x10);
    _WRITE_DATA(0x03);
    _WRITE_DATA(0x24);
    _WRITE_DATA(0x20);
    _WRITE_DATA(0x00);
    // Digital Gamma Control 1
    _WRITE_CMD(0xE2);
    _WRITE_DATA(0x0F);
    _WRITE_DATA(0x32);
    _WRITE_DATA(0x2E);
    _WRITE_DATA(0x0B);
    _WRITE_DATA(0x0D);
    _WRITE_DATA(0x05);
    _WRITE_DATA(0x47);
    _WRITE_DATA(0x75);
    _WRITE_DATA(0x37);
    _WRITE_DATA(0x06);
    _WRITE_DATA(0x10);
    _WRITE_DATA(0x03);
    _WRITE_DATA(0x24);
    _WRITE_DATA(0x20);
    _WRITE_DATA(0x00);
    // Set rotation
    setRotation(_rotation);
    // Idle mode control + Power +  Frame rate ctrl
    idleModeOff();
    // Display Inversion Control
    _WRITE_CMD(0xB4);
    _WRITE_DATA(0x02); // 2 dot invercion /-/ disabled | 0x12 to enable
    // Display Function Control
    _WRITE_CMD(0xB6);
    _WRITE_DATA(0x02);
    _WRITE_DATA(0x22);
    _WRITE_DATA(0x3B);
    // # Sleep OUT
    _WRITE_CMD(0x11);
    delay(150);
    // Display ON
    _WRITE_CMD(0x29);
}

/**
 * @brief Get current horizontal resolution value
 *
 * @return uint16_t
 */
uint16_t ILI9486_Display_Driver::getWidth() { return _width; }

/**
 * @brief Get current vertical resolution value
 *
 * @return uint16_t
 */
uint16_t ILI9486_Display_Driver::getHeight() { return _height; }

/**
 * @brief Get display rotation value
 *        0, 2 - landscape mode
 *        1, 3 - portrait mode
 *
 * @return uint8_t 
*/
uint8_t ILI9486_Display_Driver::getRotation() { return _rotation; }

/**
 * @brief Reset routine
 */
void ILI9486_Display_Driver::reset() {
  FSMC_RESET_GPIO_Port->BSRR = FSMC_RESET_Pin;
  HAL_Delay(20);
  FSMC_RESET_GPIO_Port->BSRR = (uint32_t)FSMC_RESET_Pin << 16U;
  HAL_Delay(20);
  FSMC_RESET_GPIO_Port->BSRR = FSMC_RESET_Pin;
  HAL_Delay(200);
  begin();
}

/**
 * @brief
 *
 */
void ILI9486_Display_Driver::softReset() {
  _WRITE_CMD(0x01);
  delay(100);
  begin();
}

/**
 * @brief Clear defined area
 *
 */
void ILI9486_Display_Driver::clear(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1,
                           uint16_t color) {
  if (x >= x1 || y >= y1) return;
  uint32_t m = 0;
  canvas(x, y, x1, y1);
  m = (x1 - x) * (y1 - y);
  fill(color, m);
}

/**
 * @brief CLear whole screen
 *
 */
void ILI9486_Display_Driver::clearScreen(uint16_t color) {
  clear(0, 0, _width, _height, color);
}

/**
 * @brief Set region dimensions for further action
 * 
 * @param x 
 * @param y 
 * @param x1 
 * @param y1 
*/
void ILI9486_Display_Driver::canvas(int16_t x, int16_t y, int16_t x1,
                                   int16_t y1) {
    if (x >= x1 || y >= y1) return;
    _WRITE_CMD(_DISPLAY_COLADDSET);
    _WRITE_DATA(x >> 8);
    _WRITE_DATA(x & 0xff); // START
    _WRITE_DATA(x1 >> 8);
    _WRITE_DATA(x1 & 0xff); // END

    _WRITE_CMD(_DISPLAY_PAGEADDSET);
    _WRITE_DATA(y >> 8);
    _WRITE_DATA(y & 0xff); // START
    _WRITE_DATA(y1 >> 8);
    _WRITE_DATA(y1 & 0xff); // END
}

/**
 * @brief Change screen rotation
 *
 * @param rotation 
*/
void ILI9486_Display_Driver::setRotation(uint8_t rotation) {
  _rotation = rotation & 3;
  _WRITE_CMD(_DISPLAY_MADCTL);
  switch (_rotation) {
  case 0:
    // |ˉˉˉˉˉˉˉˉˉ(X)
    // |
    // |
    // (Y)
    _WRITE_DATA(0xA8);
    _width = _DISPLAY_WIDTH;
    _height = _DISPLAY_HEIGHT;
    break;
  case 1:
    // (Y) ˉˉˉˉˉˉˉ|
    //            |
    //            |
    //           (X)
    _WRITE_DATA(0x08);
    _width = _DISPLAY_HEIGHT;
    _height = _DISPLAY_WIDTH;
    break;
  case 2:
    //           (Y)
    //            |
    //            |
    // (X)________|
    _WRITE_DATA(0x68);
    _width = _DISPLAY_WIDTH;
    _height = _DISPLAY_HEIGHT;
    break;
  case 3:
    // (X)
    // |
    // |
    // |________(Y)
    _WRITE_DATA(0xC8);
    _width = _DISPLAY_HEIGHT;
    _height = _DISPLAY_WIDTH;
    break;
  }
  canvas(0, 0, _width, _height);
}

/**
 * @brief Set color for "length" bytes
 *
 * @param color 
 * @param length 
*/
void ILI9486_Display_Driver::fill(uint16_t color, uint32_t length) {
    uint16_t i = 0, n = 0, m = 0;
    n = length % 16;
    m = length - n;
    _WRITE_CMD(_DISPLAY_RAMWR);
    do {
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);

        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        _WRITE_DATA(color);
        i += 16;
    } while (i <= m);
    // remainders
    if(n > 0) {
        i = 0;
        do {
            _WRITE_DATA(color);
            i++;
        } while(i <= n);
    }
}

/**
 * @brief Define location for 1x1 address window
 *
 */
void ILI9486_Display_Driver::setCursor(uint16_t x, uint16_t y) {
  canvas(x, y, x + 1, y + 1);
}

/**
 * @brief Change display Font
 *        Calling without parameters resets the font
 * 
 * @param kern 
 * @param bold 
*/
void ILI9486_Display_Driver::setFont(uint8_t kern, bool bold) {
    #ifdef USE_DRIVER_FONTS
    switch (kern) {
    case 0x53: // 'S'
        // symbol glyphs _font = ...;
        break;
    case 7:
        #ifdef USE_FONT_K7
            // if(!bold) _font = ...;
            // else _font = ...;
        #endif
        break;
    default:
    case 9:
        if(!bold) _font = &FreeSans9pt7b;
        else _font = &FreeSansBold9pt7b;
        break;
    case 12:
        #ifdef USE_FONT_K12
            if(!bold) _font = &FreeSans12pt7b;
            else _font = &FreeSansBold12pt7b;
        #endif
        break;
    case 18:
        #ifdef USE_FONT_K18
            if(!bold) _font = &FreeSans18pt7b;
            else _font = &FreeSansBold18pt7b;
        #endif
        break;
    }
    #endif
}

/**
 * @brief Draw a pixel
 * 
 * @param x 
 * @param y 
 * @param color 
*/
void ILI9486_Display_Driver::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
  if ((x >= 0 && x < _width) && (y >= 0 && y < _height)) {
    canvas(x, y, x + 1, y + 1);
    fill(color, 1);
  }
}

/**
 * @brief This method attempts to fill with color_arr[x] the display region
 *        by groups of pixels
 *
 * @param x1 Region start X
 * @param y1 Region start Y
 * @param x2 Region end X
 * @param y2 Region end Y
 * @param color array of pixel colors
*/
void ILI9486_Display_Driver::drawScreenPixelBuffer(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t * color) {
    if((x1 >= x2) || (y1 >= y2)) return;
    // set canvas to write
    canvas(x1, y1, x2, y2);
    uint16_t i = 0;
    uint16_t l = ((x2 + 1) - x1) * ((y2 + 1) - y1);
    _WRITE_CMD(_DISPLAY_RAMWR);
    // 150 cycles
    do {
        _WRITE_DATA(*color);
        _WRITE_DATA(*(color + 1));
        _WRITE_DATA(*(color + 2));
        _WRITE_DATA(*(color + 3));
        _WRITE_DATA(*(color + 4));
        _WRITE_DATA(*(color + 5));
        _WRITE_DATA(*(color + 6));
        _WRITE_DATA(*(color + 7));

        _WRITE_DATA(*(color + 8));
        _WRITE_DATA(*(color + 9));
        _WRITE_DATA(*(color + 10));
        _WRITE_DATA(*(color + 11));
        _WRITE_DATA(*(color + 12));
        _WRITE_DATA(*(color + 13));
        _WRITE_DATA(*(color + 14));
        _WRITE_DATA(*(color + 15));

        _WRITE_DATA(*(color + 16));
        _WRITE_DATA(*(color + 17));
        _WRITE_DATA(*(color + 18));
        _WRITE_DATA(*(color + 19));
        _WRITE_DATA(*(color + 20));
        _WRITE_DATA(*(color + 21));
        _WRITE_DATA(*(color + 22));
        _WRITE_DATA(*(color + 23));

        _WRITE_DATA(*(color + 24));
        _WRITE_DATA(*(color + 25));
        _WRITE_DATA(*(color + 26));
        _WRITE_DATA(*(color + 27));
        _WRITE_DATA(*(color + 28));
        _WRITE_DATA(*(color + 29));
        _WRITE_DATA(*(color + 30));
        _WRITE_DATA(*(color + 31));
        i += 32;
        color += 32;
    } while(i < l);
}

/**
 * @brief draw line
 *
 */
void ILI9486_Display_Driver::drawLine(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2,
                              uint16_t color) {
  uint16_t i;
  uint16_t usX_Current, usY_Current;
  int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance;
  int32_t lIncrease_X, lIncrease_Y;

    if(x > x2 || y > y2) {
        SWAP_INT16(x, x2);
        SWAP_INT16(y, y2);
    }
    if(x == x2 && y == y2) return;

  lDelta_X = x2 - x;
  lDelta_Y = y2 - y;
  usX_Current = x;
  usY_Current = y;

  if (lDelta_X > 0) {
    lIncrease_X = 1;
  } else if (lDelta_X == 0) {
    lIncrease_X = 0;
  } else {
    lIncrease_X = -1;
    lDelta_X = -lDelta_X;
  }

  if (lDelta_Y > 0) {
    lIncrease_Y = 1;
  } else if (lDelta_Y == 0) {
    lIncrease_Y = 0;
  } else {
    lIncrease_Y = -1;
    lDelta_Y = -lDelta_Y;
  }

  if (lDelta_X > lDelta_Y) {
    lDistance = lDelta_X;
  } else {
    lDistance = lDelta_Y;
  }

  for (i = 0; i <= lDistance + 1; i++) {
    drawPixel(usX_Current, usY_Current, color);
    lError_X += lDelta_X;
    lError_Y += lDelta_Y;
    if (lError_X > lDistance) {
      lError_X -= lDistance;
      usX_Current += lIncrease_X;
    }
    if (lError_Y > lDistance) {
      lError_Y -= lDistance;
      usY_Current += lIncrease_Y;
    }
  }
}

/**
 * @brief Define rectangle and fill it with color
 *
 */
void ILI9486_Display_Driver::drawRect(int16_t x, int16_t y, int16_t x1, int16_t y1,
                              uint16_t color, bool fill) {
  if (fill) {
    clear(x, y, x1, y1, color);
  } else {
    drawLine(x, y, x + x1 - 1, y, color);
    drawLine(x, y + y1 - 1, x + x1 - 1, y + y1 - 1, color);
    drawLine(x, y, x, y + y1 - 1, color);
    drawLine(x + x1 - 1, y, x + x1 - 1, y + y1 - 1, color);
  }
}

/**
 * @brief Draw circle
 *
 * @param x circle center x coordinate
 * @param y circle center y coordinate
 * @param r cicle radius
 * @param color if fill=0 color of circle line, otherwise circle fill color
 * @param fill make circle filled with color
 */
void ILI9486_Display_Driver::drawCircle(uint16_t x, uint16_t y, uint16_t r,
                                uint16_t color, bool fill) {
  int16_t x_curr, curr_y;
  int16_t e;

  x_curr = 0;
  curr_y = r;
  e = 3 - (r << 1);

  while (x_curr <= curr_y) {
    int16_t cntr = 0;
    if (fill) {
      for (cntr = x_curr; cntr <= curr_y; cntr++) {
        drawPixel(x + x_curr, y + cntr, color);
        drawPixel(x - x_curr, y + cntr, color);
        drawPixel(x - cntr, y + x_curr, color);
        drawPixel(x - cntr, y - x_curr, color);
        drawPixel(x - x_curr, y - cntr, color);
        drawPixel(x + x_curr, y - cntr, color);
        drawPixel(x + cntr, y - x_curr, color);
        drawPixel(x + cntr, y + x_curr, color);
      }
    } else {
      drawPixel(x + x_curr, y + curr_y, color);
      drawPixel(x - x_curr, y + curr_y, color);
      drawPixel(x - curr_y, y + x_curr, color);
      drawPixel(x - curr_y, y - x_curr, color);
      drawPixel(x - x_curr, y - curr_y, color);
      drawPixel(x + x_curr, y - curr_y, color);
      drawPixel(x + curr_y, y - x_curr, color);
      drawPixel(x + curr_y, y + x_curr, color);
    }

    x_curr++;

    if (e < 0) {
      e += (4 * x_curr + 6);
    } else {
      e += (10 + 4 * (x_curr - curr_y));
      curr_y--;
    }
  }
}

/**
 * @brief
 * @credits <Adafruit GFX>
 *
 */
void ILI9486_Display_Driver::drawTriangle(int16_t x0, int16_t y0, int16_t x1,
                                  int16_t y1, int16_t x2, int16_t y2,
                                  uint16_t color, bool fill) {
    if (!fill) {
        drawLine(x0, y0, x1, y1, color);
        drawLine(x1, y1, x2, y2, color);
        drawLine(x2, y2, x0, y0, color);
    } else {
        int16_t a, b, y, last;
        // Sort coordinates by Y order (y2 >= y1 >= y0)
        if (y0 > y1) {
            SWAP_INT16(y0, y1); SWAP_INT16(x0, x1);
        }
        if (y1 > y2) {
            SWAP_INT16(y2, y1); SWAP_INT16(x2, x1);
        }
        if (y0 > y1) {
            SWAP_INT16(y0, y1); SWAP_INT16(x0, x1);
        }

        if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
            a = b = x0;
            if(x1 < a)      a = x1;
            else if(x1 > b) b = x1;
            if(x2 < a)      a = x2;
            else if(x2 > b) b = x2;
            drawLine(a, y0, b+1, y0, color);
            return;
        }

        int16_t
        dx01 = x1 - x0,
        dy01 = y1 - y0,
        dx02 = x2 - x0,
        dy02 = y2 - y0,
        dx12 = x2 - x1,
        dy12 = y2 - y1;
        int32_t
        sa   = 0,
        sb   = 0;

        // For upper part of triangle, find scanline crossings for segments
        // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
        // is included here (and second loop will be skipped, avoiding a /0
        // error there), otherwise scanline y1 is skipped here and handled
        // in the second loop...which also avoids a /0 error here if y0=y1
        // (flat-topped triangle).
        if(y1 == y2) last = y1;   // Include y1 scanline
        else         last = y1-1; // Skip it

        for(y=y0; y<=last; y++) {
            a   = x0 + sa / dy01;
            b   = x0 + sb / dy02;
            sa += dx01;
            sb += dx02;
            /* longhand:
            a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
            b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
            */
            if(a > b) SWAP_INT16(a,b);
            drawLine(a, y, b+1, y, color);
        }

        // For lower part of triangle, find scanline crossings for segments
        // 0-2 and 1-2.  This loop is skipped if y1=y2.
        sa = (int32_t)dx12 * (y - y1);
        sb = (int32_t)dx02 * (y - y0);
        for(; y<=y2; y++) {
            a   = x1 + sa / dy12;
            b   = x0 + sb / dy02;
            sa += dx12;
            sb += dx02;
            /* longhand:
            a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
            */
            if(a > b) SWAP_INT16(a,b);
            drawLine(a, y, b+1, y, color);
        }
    }
}

#ifdef USE_DRIVER_FONTS
/**
 * @brief Get current font
 *
 * @return GFXfont
*/
GFXfont const * ILI9486_Display_Driver::getFont() {
    return _font;
}


/**
 * @brief Draw a char
 * @credits <Adafruit GFX>
 *
 * @param x
 * @param y
 * @param c
 * @param bkg
 * @param fgr
 * @param font
 */
void ILI9486_Display_Driver::drawChar(uint16_t x, uint16_t y, const char *c,
                              uint16_t color, uint16_t bkg) {
    uint8_t chr = 0, row, col, bcntr = 0;
    uint8_t size_x = 1, size_y = 1; // TODO
    int16_t xo16 = 0, yo16 = 0;
    uint8_t tmp;

    tmp = *c - (uint8_t)_font->first;
    GFXglyph *glyph  = (_font->glyph + tmp);
    uint8_t  *bitmap = _font->bitmap;

    uint16_t bo = (*(const unsigned short *)&glyph->bitmapOffset);
    uint8_t  w  = (*(const unsigned char *)&glyph->width),
             h  = (*(const unsigned char *)&glyph->height);
    int8_t   xo = (*(const unsigned char *)&glyph->xOffset),
             yo = (*(const unsigned char *)&glyph->yOffset);

    if(size_x > 1 || size_y > 1) {
        xo16 = xo;
        yo16 = yo;
    }

    for (row = 0; row < h; row++) {
        for (col = 0; col < w; col++) {
            if(!(bcntr++ & 7)) {
                chr = (*(const unsigned char *)&bitmap[bo++]);
            }
            if(chr & 0x80) {
                if(size_x == 1 && size_y == 1) {
                    drawPixel(x+xo+col, y+yo+row, color);
                } else {
                    clear(x+(xo16+col)*size_x, y+(yo16+row)*size_y,
                        size_x, size_y, color);
                }
            }
            chr <<= 1;
        }
    }
}

/**
 * @brief Draw a string
 *
 * @param x
 * @param y
 * @param c
 * @param bkg
 * @param fgr
 * @param font
 */
void ILI9486_Display_Driver::drawString(uint16_t x, uint16_t y, const char * str,
                                uint16_t color, uint16_t bkg, bool dirVert, bool dirInv) {
    uint8_t chr = 0, row, col, bcntr = 0;
    uint8_t size_x = 1, size_y = 1; // TODO
    int16_t xo16 = 0, yo16 = 0;
    uint8_t tmp;
    uint16_t curX = 0,  // cursor coordinate X
             curY = 0;  // cursor coordinate Y

    uint16_t bo;
    uint8_t w, h, adx;
    int8_t xo, yo;
    GFXglyph * glyph;
    uint8_t  * bitmap;

    while ( *str != '\0' ) {
        tmp = *str - (uint8_t)_font->first;
        glyph  = (_font->glyph + tmp);
        bitmap = _font->bitmap;

        bo = (*(const unsigned short *)&glyph->bitmapOffset);
        w  = (*(const unsigned char *)&glyph->width);
        h  = (*(const unsigned char *)&glyph->height);
        adx = (*(const unsigned char *)&glyph->xAdvance);
        xo = (*(const unsigned char *)&glyph->xOffset);
        yo = (*(const unsigned char *)&glyph->yOffset);

        if(size_x > 1 || size_y > 1) {
            xo16 = xo;
            yo16 = yo;
        }

        for (row = 0; row < h; row++) {
            for (col = 0; col < w; col++) {
                if(!(bcntr++ & 7)) {
                    chr = (*(const unsigned char *)&bitmap[bo++]);
                }
                if(chr & 0x80) {
                    if(size_x == 1 && size_y == 1) {
                        drawPixel(curX+x+xo+col, curY+y+yo+row, color);
                    } else {
                        clear(curX+x+(xo16+col)*size_x, curY+y+(yo16+row)*size_y,
                            size_x, size_y, color);
                    }
                }
                chr <<= 1;
            }
        }
        // direction
        if(dirVert) {
            curY = dirInv ? curY - h + yo : curY + h + yo;
        } else {
            curX = dirInv ? curX - adx : curX + adx;
        }
        // reset
        bcntr = 0;
        str++;
    }
}
#endif

/**
 * @brief Display bitmap from monochrome source
 * @credits taken from <Adafruit GFX>
 *
 * @param x
 * @param y
 * @param bmp
 * @param w
 * @param h
 * @param color
 * @param bg
 */
void ILI9486_Display_Driver::drawBitmap(uint16_t x, uint16_t y, const uint8_t *bmp,
                                int16_t w, int16_t h, uint16_t color,
                                uint16_t bg) {
  int16_t byteWidth = (w + 7) / 8;
  uint8_t byte = 0;

  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7) {
        byte <<= 1;
      } else {
        byte = (bmp[j * byteWidth + i / 8]);
      }
      drawPixel(x + i, y, (byte & 0x80) ? color : bg);
    }
  }
}

/**
 * @brief Display RGB bitmap graphic
 * @credits <Adafruit GFX>
 * 
 * @param x 
 * @param y 
 * @param bitmap 
 * @param w 
 * @param h 
*/
void ILI9486_Display_Driver::drawBitmapRGB(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            drawPixel(x+i, y, bitmap[j * w + i]);
        }
    }
}

/**
 * @brief Display RGB bitmap graphic with 1bit mask
 * @credits <Adafruit GFX>
 * 
 * @param x 
 * @param y 
 * @param bitmap 
 * @param mask 
 * @param w 
 * @param h 
*/
void ILI9486_Display_Driver::drawBitmapRGB(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {
    int16_t bw   = (w + 7) / 8; // Bitmask scanline pad = whole byte
    uint8_t byte = 0;
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = mask[j * bw + i / 8];
            if(byte & 0x80) {
                drawPixel(x+i, y, bitmap[j * w + i]);
            }
        }
    }
}

/**
 * @brief Fill whole screen with color
 *
 * @param color
 */
void ILI9486_Display_Driver::fillScreen(uint16_t color) {
    canvas(0, 0, _width, _height);
    fill(color, (_width * _height));
}

/**
 * @brief Read current pixel data
 * @see canvas
 *
 * @return uint16_t
 */
uint16_t ILI9486_Display_Driver::getPixelData() {
    uint16_t r = 0, g = 0, b = 0;
    _WRITE_CMD(0x2E);
    r = _READ_DATA();
    r = _READ_DATA(); // R
    b = _READ_DATA(); // B
    g = _READ_DATA(); // G
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}

/**
 * @brief Define pixel coordinates and read its data
 *
 * @param x
 * @param y
 */
uint16_t ILI9486_Display_Driver::getPixel(uint16_t x, uint16_t y) {
    uint16_t data;
    canvas(x, y, x + 1, y + 1);
    data = getPixelData();
    return data;
}

/**
 * @brief Read Display Module Identification
 *
 */
uint32_t ILI9486_Display_Driver::getID() {
    uint16_t buf[4];
    _WRITE_CMD(0x04);
    buf[0] = _READ_DATA();          // Dummy data
    buf[1] = _READ_DATA() & 0x00ff; // Manufacturer
    buf[2] = _READ_DATA() & 0x00ff; // Driver version
    buf[3] = _READ_DATA() & 0x00ff; // Driver id
    return (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

/**
 * @brief Enable idle mode (8 colors)
 *        In the idle on mode, color expression is reduced. The primary
 *        and the secondary colors using MSB of each R, G and B in the
 *        Frame Memory, 8 color depth data is displayed.
 *
 */
void ILI9486_Display_Driver::idleModeOn() {
    _WRITE_CMD(0x39);
    // frame rate ctrl
    _WRITE_CMD(0xB2);
    // Division Ratio (default: fosc)
    _WRITE_DATA(0x00);
    // Clock per Line (default: 17 clk cycles)
    _WRITE_DATA(0x11);
    // Power Control 4 (For Idle Mode)
    _WRITE_CMD(0xC3);
    _WRITE_DATA(0x32); // default: 0x33
}

/**
 * @brief Disable ILI9486 idle mode (Full color)
 *        display panel can display maximum 262,144 colors
 *
 */
void ILI9486_Display_Driver::idleModeOff() {
    _WRITE_CMD(0x38);
    // frame rate ctrl
    _WRITE_CMD(0xB1);
    // Frame rate(Hz) (default: 70kHz) /-/ Division Ratio (default: fosc)
    _WRITE_DATA(0xB0);
    // Clock per Line (default: 17 clk cycles)
    _WRITE_DATA(0x11);
    // Power Control 3 (For Normal Mode)
    _WRITE_CMD(0xC2);
    _WRITE_DATA(0x55); // 44
}

/**
 * @brief Enable Deep standby mode
 *        Deep Standby Mode when DSTB is set to high (“1”)
 */
void ILI9486_Display_Driver::standbyModeOn() {
    _WRITE_CMD(0xB7);
    _WRITE_DATA(0xCE);
}

/**
 * @brief Disable deep standby mode
 *        1 way: Exit Deep Standby Mode by pull down CSX to low (“0”) 6 times.
 *        another way: Input a RESX pulse with effective low level duration to start up
 *        the inside logic regulator and makes a transition to the initial state
 *
 */
void ILI9486_Display_Driver::standbyModeOff() {
    GPIOD->BSRR = (uint32_t)GPIO_PIN_7 << 16U;
    delay(10);
    GPIOD->BSRR = GPIO_PIN_7;
    delay(10);
    GPIOD->BSRR = (uint32_t)GPIO_PIN_7 << 16U;
    delay(10);
    GPIOD->BSRR = GPIO_PIN_7;
    delay(10);
    GPIOD->BSRR = (uint32_t)GPIO_PIN_7 << 16U;
    delay(10);
    GPIOD->BSRR = GPIO_PIN_7;
    delay(10);
    GPIOD->BSRR = (uint32_t)GPIO_PIN_7 << 16U;
    delay(10);
    GPIOD->BSRR = GPIO_PIN_7;
    delay(10);
    GPIOD->BSRR = (uint32_t)GPIO_PIN_7 << 16U;
    delay(10);
    GPIOD->BSRR = GPIO_PIN_7;
    delay(10);
    GPIOD->BSRR = (uint32_t)GPIO_PIN_7 << 16U;
    delay(10);
    GPIOD->BSRR = GPIO_PIN_7;
    delay(10);
}

/**
 * @brief Invert display colors
 *
 */
void ILI9486_Display_Driver::invert() {
    _WRITE_CMD(_inverted ? _DISPLAY_INVOFF : _DISPLAY_INVON);
    _inverted = !_inverted;
}

/**
 * @brief
 *
 */
void ILI9486_Display_Driver::delay(__IO uint32_t cnt) {
    cnt *= 1000;
    for (; cnt != 0; cnt--);
}

/*==========================================================

--.--,---..   .,---.|   |    ,---.
  |  |   ||   ||    |---|    `---.,---.,---.,---.,---.,---.
  |  |   ||   ||    |   |        ||    |    |---'|---'|   |
  `  `---'`---'`---'`   '    `---'`---'`    `---'`---'`   '

============================================================*/

/**
 * @brief Return point data
 *
 * @return TPoint
*/
TPoint * ILI9486_Display_Driver::getPoint() {
    return &_tPoint;
}

/**
 * @brief Just a simple check if display was touched
 *
 * @return true 
 * @return false
*/
bool ILI9486_Display_Driver::isTouched() {
    return checkCtrl();
}

/**
 * @brief This method updating coordinates and return pressure amount
 *
 * @return true 
 * @return false
*/
uint16_t ILI9486_Display_Driver::getPressure() {
	return _zRaw;
}

/**
 * @brief Returns controller temperature last measurement
 * 
 * @return uint16_t 
*/
uint16_t ILI9486_Display_Driver::getCtrlTemp() {
    checkCtrlTemp();
    return _temp;
}

/**
 * @brief
 * @credits PaulStoffregen, Touchscreen library for XPT2046
 *
 * @param x
 * @param y
 * @param z
 * @return int16_t
*/
static int16_t besttwoavg( int16_t x, int16_t y, int16_t z ) {
    int16_t da = 0, db = 0, dc = 0;
    int16_t reta = 0;
    if ( x > y ) da = x - y; else da = y - x;
    if ( x > z ) db = x - z; else db = z - x;
    if ( z > y ) dc = z - y; else dc = y - z;

    if ( da <= db && da <= dc ) reta = (x + y);
    else if ( db <= da && db <= dc ) reta = (x + z);
    else reta = (y + z);

    return (reta);
}

/**
 * @brief This method communicates with touch controller over SPI
 *
*/
bool ILI9486_Display_Driver::checkCtrl() {
    // initial conditions to prevent repetitive measurements
    if(!_is_touched) return false;
    uint32_t now = HAL_GetTick();
    if((now - lastTouched) <= XYZQ_THRESHOLD) {
        //_is_touched = false;
        return false;
    }
    lastTouched = now;

    int16_t d0, d1, d2, d3, d4, d5;
    uint16_t p_z1 = 0xB1,           // 10110001
            p_z2  = 0xC1,           // 11000001
            p_x   = 0xD1,           // 11010001
            p_y   = 0x91,           // 10010001
            p_yd  = 0x90,           // 10010000
            p_d   = 0xA0;           // 10100000
    int16_t z1 = 0,
            z2 = 0,
            xp = 0;

    HAL_SPI_DC_SELECT();
    // check if it was a touch
    while(HAL_SPI_TxRx((uint8_t *)&p_x,  (uint8_t *)&xp, 2U) != HAL_OK){};
    while(HAL_SPI_TxRx((uint8_t *)&p_z1, (uint8_t *)&z1, 2U) != HAL_OK){};
    while(HAL_SPI_TxRx((uint8_t *)&p_z2, (uint8_t *)&z2, 2U) != HAL_OK){};
    z1 = z1 >> 4;
    z2 = z2 >> 4;
    xp = xp >> 4;
    _zRaw = -(X_RESIST * (xp/4096.0) * ((z2/z1) - 1.0));
    // check if the pressure is acceptable to count it as touch,
    // if not - exit
    if (_zRaw >= Z_THRESHOLD) {
        // request x, y, z parameters
        while(HAL_SPI_TxRx((uint8_t *)&p_x, (uint8_t *)&d0, 2U) != HAL_OK){};
        while(HAL_SPI_TxRx((uint8_t *)&p_y, (uint8_t *)&d1, 2U) != HAL_OK){};
        while(HAL_SPI_TxRx((uint8_t *)&p_x, (uint8_t *)&d2, 2U) != HAL_OK){};
        while(HAL_SPI_TxRx((uint8_t *)&p_y, (uint8_t *)&d3, 2U) != HAL_OK){};
        while(HAL_SPI_TxRx((uint8_t *)&p_x, (uint8_t *)&d4, 2U) != HAL_OK){};
        // last Y measurement and power down
        while(HAL_SPI_TxRx((uint8_t *)&p_yd, (uint8_t *)&d5, 2U) != HAL_OK){};
        d0 = d0 >> 3;
        d1 = d1 >> 3;
        d2 = d2 >> 3;
        d3 = d3 >> 3;
        d4 = d4 >> 3;
        d5 = d5 >> 3;

        // Average values
        int16_t x = besttwoavg( d0, d2, d4 ) >> 1;
        int16_t y = besttwoavg( d1, d3, d5 ) >> 1;
        _rotation &= 3;
        switch(_rotation) {
            // |ˉˉˉˉˉˉˉˉˉ(X)
            // |
            // |
            // (Y)
            case 0:
                _xRaw = x;
                _yRaw = 4095 - y;
            // (Y) ˉˉˉˉˉˉˉ|
            //            |
            //            |
            //           (X)
            case 1:
                _xRaw = y;
                _yRaw = 4095 - x;
                break;
            //           (Y)
            //            |
            //            |
            // (X)________|
            case 2:
                _xRaw = 4095 - x;
                _yRaw = y;
                break;
            // (X)
            // |
            // |
            // |________(Y)
            case 3:
                _xRaw = x;
                _yRaw = y;
                break;
        }
        _tPoint.x = ((_xRaw * _width)/4095.0);
        _tPoint.y = ((_yRaw * _height)/4095.0);
        _tPoint.z = _zRaw;
        _tPoint.xR = _xRaw;
        _tPoint.yR = _yRaw;
    } else {
        // Power down
        while(HAL_SPI_TxRx((uint8_t *)&p_d, (uint8_t *)&d5, 2U) != HAL_OK){};
        HAL_SPI_DC_DESELECT();
        return false;
    }

    HAL_SPI_DC_DESELECT();
    return true;
}

/**
 * @brief This method handles PENIRQ interrupt and
 *        collects an event data
 *
*/
void ILI9486_Display_Driver::touchEventHandler() {
    _is_touched = ((GPIOC->IDR & PENIRQ_Pin) == (uint32_t)GPIO_PIN_RESET);
}

/**
 * @brief Returns temperature of the chip, using
 *        internal components states
 *
*/
void ILI9486_Display_Driver::checkCtrlTemp() {
    uint32_t now = HAL_GetTick();
    if((now - lastTempQuery) <= TEMPQ_THRESHOLD) return;

    uint16_t p_t  = 0x85,           // 10000101
             p_tp = 0x84;           // 10000100
    int16_t t0 = 0, t1 = 0, t2 = 0;

    HAL_SPI_DC_SELECT();
    HAL_SPI_TxRx((uint8_t *)&p_t, (uint8_t *)&t0, 2U);
    HAL_SPI_TxRx((uint8_t *)&p_t, (uint8_t *)&t1, 2U);
    // ... and power off
    HAL_SPI_TxRx((uint8_t *)&p_tp, (uint8_t *)&t2, 2U);
    HAL_SPI_DC_DESELECT();
    // no shifting, let's see high precision
    // t0 = t0 >> 4;
    // t1 = t1 >> 4;
    // t2 = t2 >> 4;
    _temp = (t0 + t1 + t2) / 3;
    lastTempQuery = now;
}