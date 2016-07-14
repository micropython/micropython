/*
 * Copyright (c) 2015 Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2015 Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This program was originally contributed by community member "Fleck" as
 * the winning entry in the December 2014 demo competition.
 *
 * Some minor changes have been made by the uGFX maintainers.
 */

#include "gfx.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "tetris.h"

#define SEVEN_SEG_HEIGHT            SEVEN_SEG_SIZE*3
#define SEVEN_SEG_WIDTH             SEVEN_SEG_HEIGHT*3
#define SEVEN_SEG_CHAR_WIDTH        ((SEVEN_SEG_SIZE*4)+(SEVEN_SEG_HEIGHT*2)+SEVEN_SEG_WIDTH)
#define SEVEN_SEG_CHAR_HEIGHT       ((SEVEN_SEG_SIZE*4)+(SEVEN_SEG_HEIGHT*3)+(SEVEN_SEG_WIDTH*2))
#define TETRIS_SEVEN_SEG_SCORE_X    (TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)-(strlen(pps_str)*SEVEN_SEG_CHAR_WIDTH)+(SEVEN_SEG_CHAR_WIDTH*i)

// bit0 = A
// bit1 = B
// bit2 = C
// bit3 = D
// bit4 = E
// bit5 = F
// bit6 = G
// 7segment number array, starting from 0 (array index matches number)
// 7segment number:
/*
   A
F     B
   G
E     C
   D
*/
const uint8_t   sevenSegNumbers[10]                                   = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // 0,1,2,3,4,5,6,7,8,9
const color_t   tetrisShapeColors[9]                                  = {Black, HTML2COLOR(0x009000), Red, Blue, Magenta, SkyBlue, Orange, HTML2COLOR(0xBBBB00), White}; // shape colors
// default tetris shapes
const int       tetrisShapes[TETRIS_SHAPE_COUNT][4][2]                = {
                                                                          {{4, 17},{4, 16},{5, 16},{4, 15}},
                                                                          {{4, 16},{5, 16},{4, 15},{5, 15}},
                                                                          {{4, 17},{5, 16},{4, 16},{5, 15}},
                                                                          {{5, 17},{5, 16},{4, 16},{4, 15}},
                                                                          {{5, 17},{5, 16},{5, 15},{4, 15}},
                                                                          {{4, 17},{4, 16},{4, 15},{5, 15}},
                                                                          {{4, 18},{4, 17},{4, 16},{4, 15}}
                                                                        };

int             tetrisField[TETRIS_FIELD_HEIGHT][TETRIS_FIELD_WIDTH];        // main tetris field array
unsigned int    tetrisGameSpeed                                       = 500; // game auto-move speed in ms
unsigned int    tetrisKeySpeed                                        = 140; // game key repeat speed in ms
systemticks_t   tetrisPreviousGameTime                                = 0;
systemticks_t   tetrisPreviousKeyTime                                 = 0;
int             tetrisCurrentShape[4][2];
int             tetrisNextShape[4][2];
int             tetrisOldShape[4][2];
int             tetrisNextShapeNum, tetrisOldShapeNum;
unsigned long   tetrisLines                                           = 0;
unsigned long   tetrisScore                                           = 0;
bool_t          tetrisKeysPressed[5]                                  = {FALSE, FALSE, FALSE, FALSE, FALSE}; // left/down/right/up/pause
bool_t          tetrisPaused                                          = FALSE;
bool_t          tetrisGameOver                                        = FALSE;
font_t          font16;
font_t          font12;

GEventMouse     ev;

// static void initRng(void) { //STM32 RNG hardware init
//   rccEnableAHB2(RCC_AHB2ENR_RNGEN, 0);
//   RNG->CR |= RNG_CR_RNGEN;
// }

static void initRng(void) {
  srand(gfxSystemTicks());
}

static int uitoa(unsigned int value, char * buf, int max) {
  int n = 0;
  int i = 0;
  int tmp = 0;

  if (!buf) return -3;
  if (2 > max) return -4;
  i=1;
  tmp = value;
  if (0 > tmp) {
    tmp *= -1;
    i++;
  }
  for (;;) {
    tmp /= 10;
    if (0 >= tmp) break;
    i++;
  }
  if (i >= max) {
    buf[0] = '?';
    buf[1] = 0x0;
    return 2;
  }
  n = i;
  tmp = value;
  if (0 > tmp) {
    tmp *= -1;
  }
  buf[i--] = 0x0;
  for (;;) {
    buf[i--] = (tmp % 10) + '0';
    tmp /= 10;
    if (0 >= tmp) break;
  }
  if (-1 != i) {
    buf[i--] = '-';
  }
  return n;
}

static void sevenSegDraw(int x, int y, uint8_t number, color_t color) {
  if (number & 0x01) gdispFillArea(x+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, y, SEVEN_SEG_WIDTH, SEVEN_SEG_HEIGHT, color); // A
  if (number & 0x02) gdispFillArea(x+SEVEN_SEG_HEIGHT+(SEVEN_SEG_SIZE*2)+SEVEN_SEG_WIDTH, y+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // B
  if (number & 0x04) gdispFillArea(x+SEVEN_SEG_HEIGHT+(SEVEN_SEG_SIZE*2)+SEVEN_SEG_WIDTH, y+(SEVEN_SEG_HEIGHT*2)+SEVEN_SEG_WIDTH+(SEVEN_SEG_SIZE*3), SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // C
  if (number & 0x08) gdispFillArea(x+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, y+(SEVEN_SEG_HEIGHT*2)+(SEVEN_SEG_WIDTH*2)+(SEVEN_SEG_SIZE*4), SEVEN_SEG_WIDTH, SEVEN_SEG_HEIGHT, color); // D
  if (number & 0x10) gdispFillArea(x, y+(SEVEN_SEG_HEIGHT*2)+SEVEN_SEG_WIDTH+(SEVEN_SEG_SIZE*3), SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // E
  if (number & 0x20) gdispFillArea(x, y+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // F
  if (number & 0x40) gdispFillArea(x+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, y+SEVEN_SEG_HEIGHT+SEVEN_SEG_WIDTH+(SEVEN_SEG_SIZE*2), SEVEN_SEG_WIDTH, SEVEN_SEG_HEIGHT, color); // G
}

static void drawShape(uint8_t color) {
  int i;
  for (i = 0; i <= 3; i++) {
    if (tetrisCurrentShape[i][1] <= 16 || tetrisCurrentShape[i][1] >= 19) {
      gdispFillArea((tetrisCurrentShape[i][0]*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(tetrisCurrentShape[i][1]*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-2, TETRIS_CELL_HEIGHT-2, tetrisShapeColors[color]);
       if (color != 0) {
         gdispDrawBox((tetrisCurrentShape[i][0]*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(tetrisCurrentShape[i][1]*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[8]);
       } else {
         gdispDrawBox((tetrisCurrentShape[i][0]*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(tetrisCurrentShape[i][1]*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[0]);
       }
    }
  }
}

// static uint32_t randomInt(uint32_t max) { //getting random number from STM32 hardware RNG
//   static uint32_t new_value=0;
//   while ((RNG->SR & RNG_SR_DRDY) == 0) { }
//   new_value=RNG->DR % max;
//   return new_value;
// }

static uint32_t randomInt(uint32_t max) {
  return rand() % max;
}

static void createShape(void) {
  int i;
  memcpy(tetrisNextShape, tetrisShapes[tetrisNextShapeNum], sizeof(tetrisNextShape)); // assign from tetrisShapes arr;
  memcpy(tetrisCurrentShape, tetrisNextShape, sizeof(tetrisCurrentShape));            // tetrisCurrentShape = tetrisNextShape;
  memcpy(tetrisOldShape, tetrisNextShape, sizeof(tetrisOldShape));                    // tetrisOldShape = tetrisNextShape;
  for (i = 0; i <= 3; i++) {
    tetrisCurrentShape[i][0] += 7;
    tetrisCurrentShape[i][1] -= 4;
  }
  drawShape(0);
  tetrisOldShapeNum = tetrisNextShapeNum;
  tetrisNextShapeNum = randomInt(TETRIS_SHAPE_COUNT);
  memcpy(tetrisNextShape, tetrisShapes[tetrisNextShapeNum], sizeof(tetrisNextShape)); // assign from tetrisShapes arr;
  memcpy(tetrisCurrentShape, tetrisNextShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisNextShape;
  for (i = 0; i <= 3; i++) {
    tetrisCurrentShape[i][0] += 7;
    tetrisCurrentShape[i][1] -= 4;
  }
  drawShape(tetrisNextShapeNum+1);
  memcpy(tetrisCurrentShape, tetrisOldShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisOldShape;
}

static void tellScore(uint8_t color) {
  char pps_str[12];
  uint8_t i;
  uitoa(tetrisLines, pps_str, sizeof(pps_str));
  gdispFillArea((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+5, gdispGetHeight()-50, gdispGetStringWidth(pps_str, font16)+4,  gdispGetCharWidth('A', font16)+2, tetrisShapeColors[0]);
  gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+5, gdispGetHeight()-50, pps_str, font16, tetrisShapeColors[color]);
  uitoa(tetrisScore, pps_str, sizeof(pps_str));
  gdispFillArea(0, 0, gdispGetWidth(),  gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-6, tetrisShapeColors[0]);
  for (i = 0; i < strlen(pps_str); i++) {
    if (pps_str[i] == '0') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[0], Lime);
    if (pps_str[i] == '1') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[1], Lime);
    if (pps_str[i] == '2') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[2], Lime);
    if (pps_str[i] == '3') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[3], Lime);
    if (pps_str[i] == '4') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[4], Lime);
    if (pps_str[i] == '5') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[5], Lime);
    if (pps_str[i] == '6') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[6], Lime);
    if (pps_str[i] == '7') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[7], Lime);
    if (pps_str[i] == '8') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[8], Lime);
    if (pps_str[i] == '9') sevenSegDraw(TETRIS_SEVEN_SEG_SCORE_X, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[9], Lime);
  }
}

static void initField(void) {
  int i,j;
  tellScore(0); // clear score
  for (i = 0; i < TETRIS_FIELD_HEIGHT; i++) {
    for (j = 0; j < TETRIS_FIELD_WIDTH; j++) {
      tetrisField[i][j] = 0;
    }
  }
  createShape();
  drawShape(tetrisOldShapeNum+1);
  tetrisLines = 0;
  tetrisScore = 0;
  tellScore(8);
}

static void drawCell(int x, int y, uint8_t color) {
  gdispFillArea((x*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(y*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-2, TETRIS_CELL_HEIGHT-2, tetrisShapeColors[color]);
  if (color != 0) {
    gdispDrawBox((x*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(y*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[8]);
  } else {
    gdispDrawBox((x*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(y*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[0]);
  }
}

static void printText(uint8_t color) {
  gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT), "Next", font16, tetrisShapeColors[color]);
  gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+5,   gdispGetHeight()-70, "Lines", font16, tetrisShapeColors[color]);
}

static void printPaused(void) {
  if (tetrisPaused) gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, "Paused!", font12, tetrisShapeColors[2]);
  else gdispFillArea((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, gdispGetStringWidth("Paused!", font12)+4,  gdispGetCharWidth('A', font12)+2, tetrisShapeColors[0]);
}

static void printGameOver(void) {
  if (tetrisGameOver) gdispDrawString(((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)/2)-(gdispGetStringWidth("Game Over!", font12)/2), gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, "Game Over!", font12, tetrisShapeColors[2]);
  else gdispFillArea(((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)/2)-(gdispGetStringWidth("Game Over!", font12)/2), gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, gdispGetStringWidth("Game Over!", font12)+4,  gdispGetCharWidth('A', font12)+2, tetrisShapeColors[0]);
}

static void printTouchAreas(void) {
  gdispDrawStringBox(0, 0, gdispGetWidth(), gdispGetFontMetric(font16, fontHeight), "Touch Area's", font16, White, justifyCenter);
  gdispDrawStringBox(0, 0, gdispGetWidth(), gdispGetHeight()/4, "Pause", font16, Grey, justifyCenter);
  gdispDrawStringBox(0, gdispGetHeight()/4, gdispGetWidth(), gdispGetHeight()/2, "Rotate", font16, Grey, justifyCenter);
  gdispDrawStringBox(0, gdispGetHeight()-(gdispGetHeight()/4), gdispGetWidth()/4, gdispGetHeight()/4, "Left", font16, Grey, justifyCenter);
  gdispDrawStringBox(gdispGetWidth()/4, gdispGetHeight()-(gdispGetHeight()/4), gdispGetWidth()/2, gdispGetHeight()/4, "Down", font16, Grey, justifyCenter);
  gdispDrawStringBox(gdispGetWidth()-(gdispGetWidth()/4), gdispGetHeight()-(gdispGetHeight()/4), gdispGetWidth()/4, gdispGetHeight()/4, "Right", font16, Grey, justifyCenter);
  gdispDrawLine(0, gdispGetHeight()/4, gdispGetWidth()-1, gdispGetHeight()/4, Grey);
  gdispDrawLine(0, gdispGetHeight()-gdispGetHeight()/4, gdispGetWidth()-1, gdispGetHeight()-gdispGetHeight()/4, Grey);
  gdispDrawLine(gdispGetWidth()/4, gdispGetHeight()-gdispGetHeight()/4, gdispGetWidth()/4, gdispGetHeight()-1, Grey);
  gdispDrawLine(gdispGetWidth()-(gdispGetWidth()/4), gdispGetHeight()-gdispGetHeight()/4, gdispGetWidth()-(gdispGetWidth()/4), gdispGetHeight()-1, Grey);
}

static bool_t stay(bool_t down) {
  int sk, k;
  bool_t stay;
  if (down == TRUE) sk = 1; else sk = 0;
  stay = FALSE;
  for (k = 0; k <= 3; k++) {
    if (tetrisCurrentShape[k][1] == 0) {
      return TRUE;
    }
  }
  for (k = 0; k <= 3; k++) {
    if ((tetrisCurrentShape[k][0] < 0) || (tetrisCurrentShape[k][0] > 9)) return TRUE;
    if (tetrisCurrentShape[k][1] <= 16)
      if (tetrisField[tetrisCurrentShape[k][1]-sk][tetrisCurrentShape[k][0]] != 0) return TRUE;
  }
  return stay;
}

static void clearCompleteLines(void) {
  bool_t t;
  uint8_t reiz = 0;
  int l,k,j;
  l = 0;
  while (l <= 16) {
    t = TRUE;
    for (j = 0; j <= 9; j++)
      if (tetrisField[l][j] == 0) t = FALSE;
    if (t == TRUE) {
      for (j = 4; j >= 0; j--) { // cheap & dirty line removal animation :D
        drawCell(j,l, 0);
        drawCell(9-j,l, 0);
        gfxSleepMilliseconds(40);
      }
      reiz++;
      for (k = 0; k <= 9; k++) {
        for (j = l; j < 16; j++) {
          tetrisField[j][k] = tetrisField[j+1][k];
          drawCell(k,j, tetrisField[j][k]);
        }
      }
      for (j = 0; j <= 9; j++) {
        tetrisField[16][j] = 0;
        drawCell(j,16,0);
      }
    } else {
      l++;
    }
  }
  if (reiz > 0) {
    tetrisLines += reiz;
    tetrisScore += (reiz*10)*reiz;
    tellScore(8);
  }
}

static void goDown(void) {
  int i;
  if (stay(TRUE) == FALSE) {
    drawShape(0);
    for (i = 0; i <= 3; i++) {
      tetrisCurrentShape[i][1]--;
    }
    drawShape(tetrisOldShapeNum+1);
  } else {
    for (i = 0; i <= 3; i++) {
      if (tetrisCurrentShape[i][1] >=17) {
        tetrisGameOver = TRUE;
        return;
      } else {
       tetrisField[tetrisCurrentShape[i][1]][tetrisCurrentShape[i][0]] = tetrisOldShapeNum+1;
      }
    }
    clearCompleteLines();
    createShape();
    if (stay(FALSE) == TRUE) {
      tetrisGameOver = TRUE;
      return;
    }
    drawShape(tetrisOldShapeNum+1);
  }
}

static void clearField(void) {
  int j, k;
  for (k = 16; k >= 0; k--) {
    for (j = 0; j <= 9; j++) {
      drawCell(j,16-k, randomInt(8)+1);
      gfxSleepMilliseconds(10);
    }
  }
  for (k = 0; k <= 16; k++) {
    for (j = 0; j <= 9; j++) {
      drawCell(j,16-k, tetrisShapeColors[0]);
      gfxSleepMilliseconds(10);
    }
  }
}

static void rotateShape(void) {
  int i, ox, oy, tx, ty;
  ox = tetrisCurrentShape[1][0];
  oy = tetrisCurrentShape[1][1];
  memcpy(tetrisOldShape, tetrisCurrentShape, sizeof(tetrisOldShape)); // tetrisOldShape = tetrisCurrentShape;
  for (i = 0; i <= 3; i++) {
    tx = tetrisCurrentShape[i][0];
    ty = tetrisCurrentShape[i][1];
    tetrisCurrentShape[i][0] = ox+(round((tx-ox)*cos(90*(3.14/180))-(ty-oy)*sin(90*(3.14/180))));
    tetrisCurrentShape[i][1] = oy+(round((tx-ox)*sin(90*(3.14/180))+(ty-oy)*cos(90*(3.14/180))));
  }
  if (stay(FALSE) == FALSE) {
    memcpy(tetrisNextShape, tetrisCurrentShape, sizeof(tetrisNextShape)); // tetrisNextShape = tetrisCurrentShape;
    memcpy(tetrisCurrentShape, tetrisOldShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisOldShape;
    drawShape(0);
    memcpy(tetrisCurrentShape, tetrisNextShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisNextShape;
    drawShape(tetrisOldShapeNum+1);
  } else {
    memcpy(tetrisCurrentShape, tetrisOldShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisOldShape;
  }
}

static bool_t checkSides(bool_t left) {
  int sk,k;
  if (left == TRUE) sk = 1; else sk = -1;
  for (k = 0; k <= 3; k++) {
    if ((tetrisCurrentShape[k][0]+sk < 0) || (tetrisCurrentShape[k][0]+sk > 9)) return TRUE;
    if (tetrisCurrentShape[k][1] <= 16)
      if (tetrisField[tetrisCurrentShape[k][1]][tetrisCurrentShape[k][0]+sk] != 0) return TRUE;
  }
  return FALSE;
}

static void goRight(void) {
  int i;
  if (checkSides(TRUE) == FALSE) {
    drawShape(0);
    for (i = 0; i <= 3; i++) {
      tetrisCurrentShape[i][0]++;
    }
    drawShape(tetrisOldShapeNum+1);
  }
}

static void goLeft(void) {
  int i;
  if (checkSides(FALSE) == FALSE) {
    drawShape(0);
    for (i = 0; i <= 3; i++) {
      tetrisCurrentShape[i][0]--;
    }
    drawShape(tetrisOldShapeNum+1);
  } 
}

static DECLARE_THREAD_FUNCTION(thdTetris, arg) {
  (void)arg;
  uint8_t i;
  while (!tetrisGameOver) {
    // key handling
    if (gfxSystemTicks() - tetrisPreviousKeyTime >= gfxMillisecondsToTicks(tetrisKeySpeed) || gfxSystemTicks() <= gfxMillisecondsToTicks(tetrisKeySpeed)) {
      for (i = 0; i < sizeof(tetrisKeysPressed); i++) {
        if (tetrisKeysPressed[i] == TRUE) {
          tetrisKeysPressed[i] = FALSE;
        }
      }
      tetrisPreviousKeyTime = gfxSystemTicks();
    }
    // auto-move part :D
    ginputGetMouseStatus(0, &ev);
    if (gfxSystemTicks() - tetrisPreviousGameTime >= gfxMillisecondsToTicks(tetrisGameSpeed) || gfxSystemTicks() <= gfxMillisecondsToTicks(tetrisGameSpeed)) {
      if ((!(ev.buttons & GINPUT_MOUSE_BTN_LEFT) || ((ev.buttons & GINPUT_MOUSE_BTN_LEFT) && !(ev.x > gdispGetWidth()/4 && ev.x <= gdispGetWidth()-(gdispGetWidth()/4) && ev.y >= gdispGetHeight()-(gdispGetHeight()/4)))) && !tetrisPaused) goDown(); // gives smooth move_down when down button pressed! :D
      tetrisPreviousGameTime = gfxSystemTicks();
    }
    if (!(ev.buttons & GINPUT_MOUSE_BTN_LEFT)) continue;
    if (ev.x <= gdispGetWidth()/4 && ev.y >= gdispGetHeight()-(gdispGetHeight()/4) && tetrisKeysPressed[0] == FALSE && !tetrisPaused) {
      goLeft();
      tetrisKeysPressed[0] = TRUE;
      tetrisPreviousKeyTime = gfxSystemTicks();
    }
    if (ev.x > gdispGetWidth()-(gdispGetWidth()/4) && ev.y >= gdispGetHeight()-(gdispGetHeight()/4) && tetrisKeysPressed[2] == FALSE && !tetrisPaused) {
      goRight();
      tetrisKeysPressed[2] = TRUE;
      tetrisPreviousKeyTime = gfxSystemTicks();
    }
    if (ev.y > gdispGetHeight()/4 && ev.y < gdispGetHeight()-(gdispGetHeight()/4) && tetrisKeysPressed[3] == FALSE && !tetrisPaused) {
      rotateShape();
      tetrisKeysPressed[3] = TRUE;
      tetrisPreviousKeyTime = gfxSystemTicks();
    }
    if (ev.x > gdispGetWidth()/4 && ev.x <= gdispGetWidth()-(gdispGetWidth()/4) && ev.y >= gdispGetHeight()-(gdispGetHeight()/4) && tetrisKeysPressed[1] == FALSE && !tetrisPaused) {
      goDown();
      tetrisKeysPressed[1] = TRUE;
      tetrisPreviousKeyTime = gfxSystemTicks();
    }
    if (ev.y <= gdispGetHeight()/4 && tetrisKeysPressed[4] == FALSE) {
      tetrisKeysPressed[4] = TRUE;
      tetrisPaused = !tetrisPaused;
      printPaused();
      tetrisPreviousKeyTime = gfxSystemTicks();
    }
  }
  return (threadreturn_t)0;
}

static void tetrisDeinit(void) {
  gdispCloseFont(font16);
  gdispCloseFont(font12);
}

void tetrisStart(void) {
  // Show the help first
  gdispClear(Black);
  printTouchAreas();
  gfxSleepMilliseconds(3000);

  // Draw the board
  gdispClear(Black);
  gdispDrawBox(0, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-5, (TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+3, (TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)+3, White);
  printText(8);

  // Away we go
  initField();
  tetrisGameOver = FALSE;
  printGameOver(); // removes "Game Over!" if tetrisGameOver == FALSE
  tetrisPreviousGameTime = gfxSystemTicks();
  gfxThreadCreate(0, 1024, NORMAL_PRIORITY, thdTetris, 0);
  while (!tetrisGameOver) {
    gfxSleepMilliseconds(1000);
  }
  clearField();
  printGameOver();
  tetrisDeinit();
}

void tetrisInit(void) {
  initRng();
  tetrisNextShapeNum = randomInt(TETRIS_SHAPE_COUNT);
  font16 = gdispOpenFont("DejaVuSans16");
  font12 = gdispOpenFont("DejaVuSans12");
}
