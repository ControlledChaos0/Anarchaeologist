#include "mode0.h"
#include "game.h"
#include "levelSelectBackground0.h"
#include "levelSelectBackground1Left.h"
#include "levelSelectBackground1Right.h"
#include "levelSelectBackground2.h"
#include "levelSelectBackground3.h"
#include "levelSpritesheet.h"
#include "levelSelect.h"
#include "loading.h"
#include "sound.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "print.h"

#include "seasonsLevelSelect.h"

int levelsUnlocked;

int levelTimes[LEVELS];
int blendNum;

LEVEL_POSITION positions[LEVELS];
LEVEL_POSITION signPositions[LEVELS];
SPRITE smallPlayer;
enum smallPlayerFrames {SMALLPLAYERIDLE, SMALLPLAYERRIGHT = 16, SMALLPLAYERLEFT};

SPRITE tensMinute;
SPRITE onesMinute;
SPRITE middle;
SPRITE tensSecond;
SPRITE onesSecond;

unsigned short levelSign1[] = {1, 2, 2, 2, 1 + (1 << 10),
                            3, 4, 5, 6, 7,
                            15, 16, 16, 16, 15 + (1 << 10)};

unsigned short levelSign2[] = {1, 2, 2, 2, 1 + (1 << 10),
                            3, 4, 5, 8, 9,
                            15, 16, 16, 16, 15 + (1 << 10)};

unsigned short levelSign3[] = {1, 2, 2, 2, 1 + (1 << 10),
                            3, 4, 5, 10, 11,
                            15, 16, 16, 16, 15 + (1 << 10)};

unsigned short levelSign4[] = {1, 2, 2, 2, 1 + (1 << 10),
                            3, 4, 5, 12, 3 + (1 << 10),
                            15, 16, 16, 16, 15 + (1 << 10)};

unsigned short levelSign5[] = {1, 2, 2, 2, 1 + (1 << 10),
                            3, 4, 5, 13, 14,
                            15, 16, 16, 16, 15 + (1 << 10)};

unsigned short * levelSigns[] = {levelSign1, levelSign2, levelSign3, levelSign4, levelSign5};

void initLevelSelect() {
    hideSprites();
    REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | SPRITE_ENABLE;
    
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(6) | BG_4BPP | BG_SIZE_SMALL | 1;
    REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(7) | BG_4BPP | BG_SIZE_SMALL | 3;
    REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(14) | BG_4BPP | BG_SIZE_SMALL | 3;
    REG_BG3CNT = BG_CHARBLOCK(3) | BG_SCREENBLOCK(15) | BG_4BPP | BG_SIZE_SMALL | 3;

    REG_BG1HOFF = 0;
    REG_BG2HOFF = 0;
    REG_BG3HOFF = 0;

    REG_BLDCNT = BLD_BG_A(2) | BLD_BG_B(3) | BLD_STD;

    //setupInterrupts();

    DMANow(3, levelSelectBackground0Pal, PALETTE, 256);
    DMANow(3, loadingTiles, &CHARBLOCK[0], loadingTilesLen / 2);
    DMANow(3, loadingMap, &SCREENBLOCK[6], loadingMapLen / 2);
    DMANow(3, levelSelectBackground2Tiles, &CHARBLOCK[2], levelSelectBackground2TilesLen / 2);
    DMANow(3, levelSelectBackground2Map, &SCREENBLOCK[14], levelSelectBackground2MapLen / 2);
    DMANow(3, levelSelectBackground3Tiles, &CHARBLOCK[3], levelSelectBackground3TilesLen / 2);
    DMANow(3, levelSelectBackground3Map, &SCREENBLOCK[15], levelSelectBackground3MapLen / 2);

    DMANow(3, levelSpritesheetPal, SPRITEPALETTE, 16);
    DMANow(3, levelSpritesheetTiles, &CHARBLOCK[4], levelSpritesheetTilesLen / 2);

    playSoundA(seasonsLevelSelect_data, seasonsLevelSelect_length, 1);

    initPositions();

    if (level <= 2) {
        DMANow(3, levelSelectBackground1LeftTiles, &CHARBLOCK[1], levelSelectBackground1LeftTilesLen / 2);
        DMANow(3, levelSelectBackground1LeftMap, &SCREENBLOCK[7], levelSelectBackground1LeftMapLen / 2);
        blendNum = 16;
        clearPathsLeft();
    } else {
        DMANow(3, levelSelectBackground1RightTiles, &CHARBLOCK[1], levelSelectBackground1RightTilesLen / 2);
        DMANow(3, levelSelectBackground1RightMap, &SCREENBLOCK[7], levelSelectBackground1RightMapLen / 2);
        blendNum = 0;
        clearPathsRight();
    }
    REG_BLDALPHA = BLD_EVA(blendNum) | BLD_EVB(16 - blendNum);
    initSmallPlayer();
    DMANow(3, levelSelectBackground0Tiles, &CHARBLOCK[0], levelSelectBackground0TilesLen / 2);
    DMANow(3, levelSelectBackground0Map, &SCREENBLOCK[6], levelSelectBackground0MapLen / 2);
    DMANow(3, levelSelectBackground0Map, &SCREENBLOCK[6], levelSelectBackground0MapLen / 2);
    drawLevelSign();
}

void updateLevelSelect() {
    updateSmallPlayer();
    updateTime();
}

void clearPathsLeft() {
    switch (levelsUnlocked) {
        case 1:
            removeFirstPath();
        case 2:
            removeSecondPath();
        case 3:
            removeThirdPath();
    }
}

void clearPathsRight() {
    switch (levelsUnlocked) {
        case 4:
            removeFourthPath();
    }
}

void removeFirstPath() {
    for (int i = (fmin((positions[0].x >> 8), (positions[1].x >> 8)) / 8) + 1; i < (fmax((positions[0].x >> 8), (positions[1].x >> 8)) / 8) - 1; i++) {
        for (int j = (fmin((positions[0].y >> 8), (positions[1].y >> 8)) / 8) + 1; j <= (fmax((positions[0].y >> 8), (positions[1].y >> 8)) / 8) + 1; j++) {
            //mgba_printf("TileX: %d, TileY: %d", i, j);
            SCREENBLOCK[7].tilemap[OFFSET(i, j, 32)] = 0;
        }
    }
}

void removeSecondPath() {
    for (int i = (fmin((positions[1].x >> 8), (positions[2].x >> 8)) / 8) + 1; i < (fmax((positions[1].x >> 8), (positions[2].x >> 8)) / 8) - 1; i++) {
        for (int j = (fmin((positions[1].y >> 8), (positions[2].y >> 8)) / 8) + 1; j <= (fmax((positions[1].y >> 8), (positions[2].y >> 8)) / 8) + 1; j++) {
            //mgba_printf("TileX: %d, TileY: %d", i, j);
            SCREENBLOCK[7].tilemap[OFFSET(i, j, 32)] = 0;
        }
    }
}

void removeThirdPath() {
    for (int i = (fmin((positions[2].x >> 8), (positions[3].x >> 8)) / 8) + 1; i < (fmax((positions[2].x >> 8), (positions[3].x >> 8)) / 8) - 1; i++) {
        for (int j = (fmin((positions[2].y >> 8), (positions[3].y >> 8)) / 8) + 1; j <= (fmax((positions[2].y >> 8), (positions[3].y >> 8)) / 8) + 1; j++) {
            //mgba_printf("TileX: %d, TileY: %d", i, j);
            SCREENBLOCK[7].tilemap[OFFSET(i, j, 32)] = 0;
        }
    }
}

void removeFourthPath() {
    for (int i = (fmin((positions[3].x >> 8), (positions[4].x >> 8)) / 8) + 1; i < (fmax((positions[3].x >> 8), (positions[4].x >> 8)) / 8) - 1; i++) {
        for (int j = (fmin((positions[3].y >> 8), (positions[4].y >> 8)) / 8) + 1; j <= (fmax((positions[3].y >> 8), (positions[4].y >> 8)) / 8) + 1; j++) {
            //mgba_printf("TileX: %d, TileY: %d", i, j);
            SCREENBLOCK[7].tilemap[OFFSET(i, j, 32)] = 0;
        }
    }
}

drawLevelSign() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            mgba_printf("TileX: %d, TileY: %d", signPositions[level].x + i, signPositions[level].y + j);
            SCREENBLOCK[6].tilemap[OFFSET(signPositions[level].x + i, signPositions[level].y + j, 32)] = levelSigns[level][OFFSET(i, j, 5)];
        }
    }
    initTime();
}

eraseLevelSign() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            mgba_printf("TileX: %d, TileY: %d", signPositions[level].x + i, signPositions[level].y + j);
            SCREENBLOCK[6].tilemap[OFFSET(signPositions[level].x + i, signPositions[level].y + j, 32)] = 0;
        }
    }
    hideTime();
}

void initPositions() {
    positions[0].x = (25 << 8);
    positions[0].y = (85 << 8);
    signPositions[0].x = ((positions[0].x >> 8) / 8) - 2;
    signPositions[0].y = ((positions[0].y >> 8) / 8) + 2;
    positions[1].x = (73 << 8);
    positions[1].y = (101 << 8);
    signPositions[1].x = ((positions[1].x >> 8) / 8) - 2;
    signPositions[1].y = ((positions[1].y >> 8) / 8) + 2;
    positions[2].x = (129 << 8);
    positions[2].y = (85 << 8);
    signPositions[2].x = ((positions[2].x >> 8) / 8) - 2;
    signPositions[2].y = ((positions[2].y >> 8) / 8) + 2;
    positions[3].x = (161 << 8);
    positions[3].y = (85 << 8);
    signPositions[3].x = ((positions[3].x >> 8) / 8) - 2;
    signPositions[3].y = ((positions[3].y >> 8) / 8) + 2;
    positions[4].x = (193 << 8);
    positions[4].y = (109 << 8);
    signPositions[4].x = ((positions[4].x >> 8) / 8) - 2;
    signPositions[4].y = ((positions[4].y >> 8) / 8) + 2;
}

void initSmallPlayer() {
    smallPlayer.height = 8;
    smallPlayer.width = 8;
    smallPlayer.aniState = SMALLPLAYERIDLE;
    smallPlayer.prevAniState = smallPlayer.aniState;
    smallPlayer.aniCounter = 0;
    smallPlayer.curFrame = 0;
    smallPlayer.numFrames = 4;
    smallPlayer.cdel = 0;
    smallPlayer.rdel = 0;
    smallPlayer.worldCol = positions[level].x;
    smallPlayer.worldRow = positions[level].y;
    smallPlayer.jump = 0;

    shadowOAM[PLAYEROAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | (smallPlayer.worldRow >> 8);
    shadowOAM[PLAYEROAM].attr1 = ATTR1_TINY | (smallPlayer.worldCol >> 8);
    shadowOAM[PLAYEROAM].attr2 = (0 + (32 * smallPlayer.curFrame) + (32 * smallPlayer.aniState)) | (2 << 10);
}

void initTime() {
    tensMinute.worldCol = (positions[level].x >> 8) - 8;
    tensMinute.worldRow = (positions[level].y >> 8) + 26;
    tensMinute.hide = 0;
    onesMinute.worldCol = (positions[level].x >> 8) - 3;
    onesMinute.worldRow = (positions[level].y >> 8) + 26;
    onesMinute.hide = 0;
    middle.worldCol = (positions[level].x >> 8) + 1;
    middle.worldRow = (positions[level].y >> 8) + 25;
    middle.hide = 0;
    middle.curFrame = 31 + (10 * 32);
    tensSecond.worldCol = (positions[level].x >> 8) + 5;
    tensSecond.worldRow = (positions[level].y >> 8) + 26;
    tensSecond.hide = 0;
    onesSecond.worldCol = (positions[level].x >> 8) + 10;
    onesSecond.worldRow = (positions[level].y >> 8) + 26;
    onesSecond.hide = 0;

    if (levelTimes[level] == -1) {
        tensMinute.curFrame = 31 + (12 * 32);
        onesMinute.curFrame = 31 + (12 * 32);
        tensSecond.curFrame = 31 + (12 * 32);
        onesSecond.curFrame = 31 + (12 * 32);
    } else {
        int sec = levelTimes[level] % 60;
        int min = (levelTimes[level] - sec) / 60;
        tensSecond.curFrame = 31 + ((sec / 10) * 32);
        onesSecond.curFrame = 31 + ((sec % 10) * 32);
        tensMinute.curFrame = 31 + ((min / 10) * 32);
        onesMinute.curFrame = 31 + ((min % 10) * 32);
    }

    shadowOAM[10].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | tensMinute.worldRow;
    shadowOAM[10].attr1 = ATTR1_TINY | tensMinute.worldCol;
    shadowOAM[10].attr2 = tensMinute.curFrame;

    shadowOAM[11].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | onesMinute.worldRow;
    shadowOAM[11].attr1 = ATTR1_TINY | onesMinute.worldCol;
    shadowOAM[11].attr2 = onesMinute.curFrame;

    shadowOAM[12].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | middle.worldRow;
    shadowOAM[12].attr1 = ATTR1_TINY | middle.worldCol;
    shadowOAM[12].attr2 = middle.curFrame;

    shadowOAM[13].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | tensSecond.worldRow;
    shadowOAM[13].attr1 = ATTR1_TINY | tensSecond.worldCol;
    shadowOAM[13].attr2 = tensSecond.curFrame;

    shadowOAM[14].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | onesSecond.worldRow;
    shadowOAM[14].attr1 = ATTR1_TINY | onesSecond.worldCol;
    shadowOAM[14].attr2 = onesSecond.curFrame;
}

void updateSmallPlayer() {
    smallPlayer.prevAniState = smallPlayer.aniState;

    if (BUTTON_PRESSED(BUTTON_RIGHT) && smallPlayer.jump == 0 && (levelsUnlocked - 1) - level != 0) {
        smallPlayer.jump = 1;
        smallPlayer.cdel = (positions[level + 1].x - positions[level].x) / 32;
        smallPlayer.rdel = (positions[level + 1].y - positions[level].y) / 32;
        smallPlayer.aniState = SMALLPLAYERRIGHT;
        eraseLevelSign();
        level++;
    }
    if (BUTTON_PRESSED(BUTTON_LEFT) && smallPlayer.jump == 0 && level != 0) {
        smallPlayer.jump = 1;
        smallPlayer.cdel = (positions[level - 1].x - positions[level].x) / 32;
        smallPlayer.rdel = (positions[level - 1].y - positions[level].y) / 32;
        smallPlayer.aniState = SMALLPLAYERLEFT;
        eraseLevelSign();
        level--;
    }

    if (smallPlayer.jump) {
        smallPlayer.worldCol += smallPlayer.cdel;
        smallPlayer.worldRow += smallPlayer.rdel;
    } else {
        smallPlayer.aniState = SMALLPLAYERIDLE;
        if (smallPlayer.prevAniState == SMALLPLAYERRIGHT || smallPlayer.prevAniState == SMALLPLAYERLEFT) {
            drawLevelSign();
        }
    }

    if (smallPlayer.worldCol > positions[2].x && smallPlayer.worldCol < positions[3].x) {
        if (smallPlayer.aniState == SMALLPLAYERRIGHT && blendNum != 0) {
            blendNum -= 2;
            if (blendNum == 0) {
                DMANow(3, levelSelectBackground1RightTiles, &CHARBLOCK[1], levelSelectBackground1RightTilesLen / 2);
                DMANow(3, levelSelectBackground1RightMap, &SCREENBLOCK[7], levelSelectBackground1RightMapLen / 2);
                clearPathsRight();
            }
        }
        if (smallPlayer.aniState == SMALLPLAYERLEFT && blendNum != 16) {
            blendNum += 2;
            if (blendNum == 16) {
                DMANow(3, levelSelectBackground1LeftTiles, &CHARBLOCK[1], levelSelectBackground1LeftTilesLen / 2);
                DMANow(3, levelSelectBackground1LeftMap, &SCREENBLOCK[7], levelSelectBackground1LeftMapLen / 2);
            }
        }
        REG_BLDALPHA = BLD_EVA(blendNum) | BLD_EVB(16 - blendNum);
    }

    if (smallPlayer.worldCol == positions[level].x && smallPlayer.worldRow == positions[level].y) {
        smallPlayer.jump = 0;
    }

    if ((smallPlayer.aniCounter / 30) == 1) {
        smallPlayer.curFrame = (smallPlayer.curFrame + 1) % smallPlayer.numFrames;
        smallPlayer.aniCounter = 0;
    }

    smallPlayer.aniCounter++;


    shadowOAM[PLAYEROAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | (smallPlayer.worldRow >> 8);
    shadowOAM[PLAYEROAM].attr1 = ATTR1_TINY | (smallPlayer.worldCol >> 8);
    shadowOAM[PLAYEROAM].attr2 = (0 + (32 * smallPlayer.curFrame) + smallPlayer.aniState) | (2 << 10);
}

void updateTime() {
    if (!tensMinute.hide) {
        shadowOAM[10].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | tensMinute.worldRow;
        shadowOAM[10].attr1 = ATTR1_TINY | tensMinute.worldCol;
        shadowOAM[10].attr2 = tensMinute.curFrame;
    } else {
        shadowOAM[10].attr0 = ATTR0_HIDE;
    }

    if (!onesMinute.hide) {
        shadowOAM[11].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | onesMinute.worldRow;
        shadowOAM[11].attr1 = ATTR1_TINY | onesMinute.worldCol;
        shadowOAM[11].attr2 = onesMinute.curFrame;
    } else {
        shadowOAM[11].attr0 = ATTR0_HIDE;
    }

    if (!middle.hide) {
        shadowOAM[12].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | middle.worldRow;
        shadowOAM[12].attr1 = ATTR1_TINY | middle.worldCol;
        shadowOAM[12].attr2 = middle.curFrame;
    } else {
        shadowOAM[12].attr0 = ATTR0_HIDE;
    }

    if (!tensSecond.hide) {
        shadowOAM[13].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | tensSecond.worldRow;
        shadowOAM[13].attr1 = ATTR1_TINY | tensSecond.worldCol;
        shadowOAM[13].attr2 = tensSecond.curFrame;
    } else {
        shadowOAM[13].attr0 = ATTR0_HIDE;
    }

    if (!onesSecond.hide) {
        shadowOAM[14].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | onesSecond.worldRow;
        shadowOAM[14].attr1 = ATTR1_TINY | onesSecond.worldCol;
        shadowOAM[14].attr2 = onesSecond.curFrame;
    } else {
        shadowOAM[14].attr0 = ATTR0_HIDE;
    }
}

void hideTime() {
    tensMinute.hide = 1;
    onesMinute.hide = 1;
    middle.hide = 1;
    tensSecond.hide = 1;
    onesSecond.hide = 1;
}