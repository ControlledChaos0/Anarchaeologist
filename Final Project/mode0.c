#include "mode0.h"

// The start of the video memory.
unsigned volatile short *videoBuffer = (unsigned short *)0x6000000;

// The start of DMA registers.
DMA *dma = (DMA *)0x40000B0;

// Set a pixel on the screen in Mode 3.
void setPixel3(int col, int row, unsigned short color) {
    videoBuffer[OFFSET(col, row, SCREENWIDTH)] = color;
}

// Set a pixel on the screen in Mode 4.
void setPixel4(int col, int row, unsigned char colorIndex) {
    volatile unsigned short pixelData = videoBuffer[OFFSET(col, row, SCREENWIDTH) / 2];
    if (col & 1) {
        pixelData &= 0x00FF;
        pixelData |= colorIndex << 8;
    } else {
        pixelData &= 0xFF00;
        pixelData |= colorIndex;
    }
    videoBuffer[OFFSET(col, row, SCREENWIDTH) / 2] = pixelData;
}

// Draw a rectangle at the specified location and size in Mode 3.
void drawRect3(int col, int row, int width, int height, volatile unsigned short color) {
    for (int r = 0; r < height; r++) {
        DMANow(3, &color, &videoBuffer[OFFSET(col, row + r, SCREENWIDTH)], DMA_SOURCE_FIXED | width);
    }
}

// Draw a rectangle at the specified location and size in Mode 4.
void drawRect4(int col, int row, int width, int height, volatile unsigned char colorIndex) {
    volatile unsigned short pixelData = colorIndex | (colorIndex << 8);
    for (int r = 0; r < height; r++) {
        // Ensure we don't DMA 0 copies.
        if (width == 1)
            setPixel4(col, row + r, colorIndex);
        else if (width == 2) {
            setPixel4(col, row + r, colorIndex);
            setPixel4(col + 1, row + r, colorIndex);
        } else if ((col & 1) && (width & 1)) { // Odd width odd col 
            setPixel4(col, row + r, colorIndex);
            DMANow(3, &pixelData, &videoBuffer[OFFSET(col + 1, row + r, SCREENWIDTH) / 2], DMA_SOURCE_FIXED | (width - 1) / 2);
        } else if (width & 1) { // Even col odd width 
            DMANow(3, &pixelData, &videoBuffer[OFFSET(col, row + r, SCREENWIDTH) / 2], DMA_SOURCE_FIXED | (width - 1) / 2);
            setPixel4(col + width - 1, row + r, colorIndex);
        } else if (col & 1) { // Odd col even width
            setPixel4(col, row + r, colorIndex);
            DMANow(3, &pixelData, &videoBuffer[OFFSET(col + 1, row + r, SCREENWIDTH) / 2], DMA_SOURCE_FIXED | (width - 2) / 2);
            setPixel4(col + width - 1, row + r, colorIndex);
        } else { // Even col even width
            DMANow(3, &pixelData, &videoBuffer[OFFSET(col, row + r, SCREENWIDTH) / 2], DMA_SOURCE_FIXED | width / 2);
        }
    }
}

// Fill the entire screen with a single color in Mode 3.
void fillScreen3(volatile unsigned short color) {
    unsigned short c = color;
    DMANow(3, &c, videoBuffer, DMA_SOURCE_FIXED | (SCREENWIDTH * SCREENHEIGHT));
}

// Fill the entire screen with a single color in Mode 4.
void fillScreen4(volatile unsigned char colorIndex) {
    volatile unsigned short pixelData = colorIndex | (colorIndex << 8);
    DMANow(3, &pixelData, videoBuffer, DMA_SOURCE_FIXED | (SCREENWIDTH * SCREENHEIGHT) / 2);
}

// Draw an image at the specified location and size in Mode 3.
void drawImage3(int col, int row, int width, int height, const unsigned short *image) {
    for (int r = 0; r < height; r++) {
        if (row + r < 0) continue;
        DMANow(3, &image[OFFSET(0, r, width)], &videoBuffer[OFFSET(col, row + r, SCREENWIDTH)], width);
    }
}

// Draw an image at the specified location and size in Mode 4 (must be even col and width).
void drawImage4(int col, int row, int width, int height, const unsigned short *image) {
    for (int i = 0; i < height; i++) {
        if (row + i < 0) continue;
        DMANow(3, &image[OFFSET(0, i, width / 2)], &videoBuffer[OFFSET(col, row + i, SCREENWIDTH) / 2], width / 2);
    }
}

// Fill the entire screen with an image in Mode 3.
void drawFullscreenImage3(const unsigned short *image) {
    DMANow(3, image, videoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

// Fill the entire screen with an image in Mode 4.
void drawFullscreenImage4(const unsigned short *image) {
    DMANow(3, image, videoBuffer, SCREENWIDTH * SCREENHEIGHT / 2);
}

void drawChar0(int col, int row, char str, unsigned char shadowOAMIndex) {
    unsigned char initial = 64 + shadowOAMIndex;
    shadowOAM[initial].attr0 = row | ATTR0_4BPP | ATTR0_REGULAR | ATTR0_SQUARE;
    shadowOAM[initial].attr1 = col | ATTR1_TINY;
    shadowOAM[initial].attr2 = (23 * 32) + ((str / 30) * 32) + (str % 30);
}

void drawString0(int col, int row, char* str, unsigned char shadowOAMIndex) {
    unsigned short i = 0;
    while (*str != '\0') {
        drawChar0(col + (i * 6), row, *str, shadowOAMIndex + i);
        str++;
        i++;
    }
}

// Pause code execution until vertical blank begins.
void waitForVBlank() {
    while (SCANLINECOUNTER > 160);
    while (SCANLINECOUNTER < 160);
}

// Flips the page.
void flipPage() {
    if (REG_DISPCTL & DISP_BACKBUFFER) {
        videoBuffer = BACKBUFFER;
    } else {
        videoBuffer = FRONTBUFFER;
    }
    REG_DISPCTL ^= DISP_BACKBUFFER;
}

// Set up and begin a DMA transfer.
void DMANow(int channel, volatile const void *src, volatile void *dst, unsigned int cnt) {
    // Turn DMA off.
    dma[channel].cnt = 0;

    // Set source and destination.
    dma[channel].src = src;
    dma[channel].dst = dst;

    // Set control and begin.
    dma[channel].cnt = cnt | DMA_ON;
}

// Return true if the two rectangular areas are overlapping.
int collision(int colA, int rowA, int widthA, int heightA, int colB, int rowB, int widthB, int heightB) {
    return rowA < rowB + heightB - 1 
    && rowA + heightA - 1 > rowB 
    && colA < colB + widthB - 1 
    && colA + widthA - 1 > colB;
}

// Hides all sprites in the shadowOAM; Must DMA the shadowOAM into the OAM after calling this function.
void hideSprites() {
    // add this from lab07!!
    for (int i = 0; i < 128; i++) {
        shadowOAM[i].attr0 = ATTR0_HIDE;
    }
    waitForVBlank();
    DMANow(3, shadowOAM, OAM, 128 * 4);
}

const short sin_lut_fixed8[] = {
  0,  // 0
  4,  // 1
  8,  // 2
  13,  // 3
  17,  // 4
  22,  // 5
  26,  // 6
  31,  // 7
  35,  // 8
  40,  // 9
  44,  // 10
  48,  // 11
  53,  // 12
  57,  // 13
  61,  // 14
  66,  // 15
  70,  // 16
  74,  // 17
  79,  // 18
  83,  // 19
  87,  // 20
  91,  // 21
  95,  // 22
  100,  // 23
  104,  // 24
  108,  // 25
  112,  // 26
  116,  // 27
  120,  // 28
  124,  // 29
  128,  // 30
  131,  // 31
  135,  // 32
  139,  // 33
  143,  // 34
  146,  // 35
  150,  // 36
  154,  // 37
  157,  // 38
  161,  // 39
  164,  // 40
  167,  // 41
  171,  // 42
  174,  // 43
  177,  // 44
  181,  // 45
  184,  // 46
  187,  // 47
  190,  // 48
  193,  // 49
  196,  // 50
  198,  // 51
  201,  // 52
  204,  // 53
  207,  // 54
  209,  // 55
  212,  // 56
  214,  // 57
  217,  // 58
  219,  // 59
  221,  // 60
  223,  // 61
  226,  // 62
  228,  // 63
  230,  // 64
  232,  // 65
  233,  // 66
  235,  // 67
  237,  // 68
  238,  // 69
  240,  // 70
  242,  // 71
  243,  // 72
  244,  // 73
  246,  // 74
  247,  // 75
  248,  // 76
  249,  // 77
  250,  // 78
  251,  // 79
  252,  // 80
  252,  // 81
  253,  // 82
  254,  // 83
  254,  // 84
  255,  // 85
  255,  // 86
  255,  // 87
  255,  // 88
  255,  // 89
  256,  // 90
  255,  // 91
  255,  // 92
  255,  // 93
  255,  // 94
  255,  // 95
  254,  // 96
  254,  // 97
  253,  // 98
  252,  // 99
  252,  // 100
  251,  // 101
  250,  // 102
  249,  // 103
  248,  // 104
  247,  // 105
  246,  // 106
  244,  // 107
  243,  // 108
  242,  // 109
  240,  // 110
  238,  // 111
  237,  // 112
  235,  // 113
  233,  // 114
  232,  // 115
  230,  // 116
  228,  // 117
  226,  // 118
  223,  // 119
  221,  // 120
  219,  // 121
  217,  // 122
  214,  // 123
  212,  // 124
  209,  // 125
  207,  // 126
  204,  // 127
  201,  // 128
  198,  // 129
  196,  // 130
  193,  // 131
  190,  // 132
  187,  // 133
  184,  // 134
  181,  // 135
  177,  // 136
  174,  // 137
  171,  // 138
  167,  // 139
  164,  // 140
  161,  // 141
  157,  // 142
  154,  // 143
  150,  // 144
  146,  // 145
  143,  // 146
  139,  // 147
  135,  // 148
  131,  // 149
  128,  // 150
  124,  // 151
  120,  // 152
  116,  // 153
  112,  // 154
  108,  // 155
  104,  // 156
  100,  // 157
  95,  // 158
  91,  // 159
  87,  // 160
  83,  // 161
  79,  // 162
  74,  // 163
  70,  // 164
  66,  // 165
  61,  // 166
  57,  // 167
  53,  // 168
  48,  // 169
  44,  // 170
  40,  // 171
  35,  // 172
  31,  // 173
  26,  // 174
  22,  // 175
  17,  // 176
  13,  // 177
  8,  // 178
  4,  // 179
  0,  // 180
  -4,  // 181
  -8,  // 182
  -13,  // 183
  -17,  // 184
  -22,  // 185
  -26,  // 186
  -31,  // 187
  -35,  // 188
  -40,  // 189
  -44,  // 190
  -48,  // 191
  -53,  // 192
  -57,  // 193
  -61,  // 194
  -66,  // 195
  -70,  // 196
  -74,  // 197
  -79,  // 198
  -83,  // 199
  -87,  // 200
  -91,  // 201
  -95,  // 202
  -100,  // 203
  -104,  // 204
  -108,  // 205
  -112,  // 206
  -116,  // 207
  -120,  // 208
  -124,  // 209
  -127,  // 210
  -131,  // 211
  -135,  // 212
  -139,  // 213
  -143,  // 214
  -146,  // 215
  -150,  // 216
  -154,  // 217
  -157,  // 218
  -161,  // 219
  -164,  // 220
  -167,  // 221
  -171,  // 222
  -174,  // 223
  -177,  // 224
  -181,  // 225
  -184,  // 226
  -187,  // 227
  -190,  // 228
  -193,  // 229
  -196,  // 230
  -198,  // 231
  -201,  // 232
  -204,  // 233
  -207,  // 234
  -209,  // 235
  -212,  // 236
  -214,  // 237
  -217,  // 238
  -219,  // 239
  -221,  // 240
  -223,  // 241
  -226,  // 242
  -228,  // 243
  -230,  // 244
  -232,  // 245
  -233,  // 246
  -235,  // 247
  -237,  // 248
  -238,  // 249
  -240,  // 250
  -242,  // 251
  -243,  // 252
  -244,  // 253
  -246,  // 254
  -247,  // 255
  -248,  // 256
  -249,  // 257
  -250,  // 258
  -251,  // 259
  -252,  // 260
  -252,  // 261
  -253,  // 262
  -254,  // 263
  -254,  // 264
  -255,  // 265
  -255,  // 266
  -255,  // 267
  -255,  // 268
  -255,  // 269
  -256,  // 270
  -255,  // 271
  -255,  // 272
  -255,  // 273
  -255,  // 274
  -255,  // 275
  -254,  // 276
  -254,  // 277
  -253,  // 278
  -252,  // 279
  -252,  // 280
  -251,  // 281
  -250,  // 282
  -249,  // 283
  -248,  // 284
  -247,  // 285
  -246,  // 286
  -244,  // 287
  -243,  // 288
  -242,  // 289
  -240,  // 290
  -238,  // 291
  -237,  // 292
  -235,  // 293
  -233,  // 294
  -232,  // 295
  -230,  // 296
  -228,  // 297
  -226,  // 298
  -223,  // 299
  -221,  // 300
  -219,  // 301
  -217,  // 302
  -214,  // 303
  -212,  // 304
  -209,  // 305
  -207,  // 306
  -204,  // 307
  -201,  // 308
  -198,  // 309
  -196,  // 310
  -193,  // 311
  -190,  // 312
  -187,  // 313
  -184,  // 314
  -181,  // 315
  -177,  // 316
  -174,  // 317
  -171,  // 318
  -167,  // 319
  -164,  // 320
  -161,  // 321
  -157,  // 322
  -154,  // 323
  -150,  // 324
  -146,  // 325
  -143,  // 326
  -139,  // 327
  -135,  // 328
  -131,  // 329
  -128,  // 330
  -124,  // 331
  -120,  // 332
  -116,  // 333
  -112,  // 334
  -108,  // 335
  -104,  // 336
  -100,  // 337
  -95,  // 338
  -91,  // 339
  -87,  // 340
  -83,  // 341
  -79,  // 342
  -74,  // 343
  -70,  // 344
  -66,  // 345
  -61,  // 346
  -57,  // 347
  -53,  // 348
  -48,  // 349
  -44,  // 350
  -40,  // 351
  -35,  // 352
  -31,  // 353
  -26,  // 354
  -22,  // 355
  -17,  // 356
  -13,  // 357
  -8,  // 358
  -4,  // 359
  0  // 360
};
