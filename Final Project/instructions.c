#include "mode0.h"
#include "instructions.h"
#include "instructionScreen1Background0.h"
#include "instructionScreen1Background1.h"
#include "instructionScreen2Background0.h"
#include "instructionScreen2Background1.h"
#include "levelSpritesheet.h"
#include "sound.h"

#include "cocking.h"

ANISPRITE instructArrow;
ANISPRITE aButton;
ANISPRITE bButton;
ANISPRITE upDown;
ANISPRITE leftRight;
int instructScreenNum;

void initInstruct() {
    REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | SPRITE_ENABLE;
    REG_BG1HOFF = 0;
    REG_BG2HOFF = 0;
    REG_BG3HOFF = 0;
    if (instructScreenNum == 1) {
        initScreen1();
    } else if (instructScreenNum == 2) {
        initScreen2();
    }
    
}

void initScreen1() {
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(29) | BG_4BPP | BG_SIZE_SMALL | 1;
    REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(30) | BG_4BPP | BG_SIZE_SMALL | 3;

    DMANow(3, instructionScreen1Background0Pal, PALETTE, 256);
    DMANow(3, instructionScreen1Background0Tiles, &CHARBLOCK[0], instructionScreen1Background0TilesLen / 2);
    DMANow(3, instructionScreen1Background0Map, &SCREENBLOCK[29], instructionScreen1Background0MapLen / 2);
    DMANow(3, instructionScreen1Background1Tiles, &CHARBLOCK[1], instructionScreen1Background1TilesLen / 2);
    DMANow(3, instructionScreen1Background1Map, &SCREENBLOCK[30], instructionScreen1Background1MapLen / 2);

    DMANow(3, levelSpritesheetPal, SPRITEPALETTE, 16);
    DMANow(3, levelSpritesheetTiles, &CHARBLOCK[4], levelSpritesheetTilesLen / 2);

    hideSprites();

    initInstructArrow1();
}

void initScreen2() {
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(29) | BG_4BPP | BG_SIZE_SMALL | 1;
    REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(30) | BG_4BPP | BG_SIZE_SMALL | 3;

    DMANow(3, instructionScreen2Background0Pal, PALETTE, 256);
    DMANow(3, instructionScreen2Background0Tiles, &CHARBLOCK[0], instructionScreen1Background0TilesLen / 2);
    DMANow(3, instructionScreen2Background0Map, &SCREENBLOCK[29], instructionScreen1Background0MapLen / 2);
    DMANow(3, instructionScreen2Background1Tiles, &CHARBLOCK[1], instructionScreen1Background1TilesLen / 2);
    DMANow(3, instructionScreen2Background1Map, &SCREENBLOCK[30], instructionScreen1Background1MapLen / 2);

    DMANow(3, levelSpritesheetPal, SPRITEPALETTE, 16);
    DMANow(3, levelSpritesheetTiles, &CHARBLOCK[4], levelSpritesheetTilesLen / 2);

    hideSprites();

    initInstructArrow2();
    initAButton();
    initBButton();
    initLeftRight();
    initUpDown();
}

void updateInstruct() {
    if (instructScreenNum == 1) {
        updateScreen1();
    } else if (instructScreenNum == 2) {
        updateScreen2();
    }

    if (BUTTON_PRESSED(BUTTON_A) && !instructArrow.flip) {
        instructScreenNum--;
        if (instructScreenNum == 1 || instructScreenNum == 2) {
            initInstruct();
        }
    } else if (BUTTON_PRESSED(BUTTON_A) && instructArrow.flip) {
        instructScreenNum++;
        if (instructScreenNum == 1 || instructScreenNum == 2) {
            initInstruct();
        }
    }
}

void updateScreen1() {
    updateInstructArrow1();
}

void updateScreen2() {
    updateInstructArrow2();
    updateAButton();
    updateBButton();
    updateLeftRight();
    updateUpDown();
}

void initInstructArrow1() {
    instructArrow.col = 31;
    instructArrow.row = 135;
    instructArrow.height = 8;
    instructArrow.width = 8;
    instructArrow.flip = 0;

    shadowOAM[INSTRUCTARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | instructArrow.row;
    shadowOAM[INSTRUCTARROWOAM].attr1 = ATTR1_TINY | instructArrow.col;
    shadowOAM[INSTRUCTARROWOAM].attr2 = 30;
}

void initInstructArrow2() {
    instructArrow.col = 31;
    instructArrow.row = 140;
    instructArrow.height = 8;
    instructArrow.width = 8;
    instructArrow.flip = 0;

    shadowOAM[INSTRUCTARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | instructArrow.row;
    shadowOAM[INSTRUCTARROWOAM].attr1 = ATTR1_TINY | instructArrow.col;
    shadowOAM[INSTRUCTARROWOAM].attr2 = 30;
}

void initAButton() {
    aButton.col = 32;
    aButton.row = 32;
    aButton.height = 16;
    aButton.width = 16;
    aButton.aniCounter = 0;
    aButton.curFrame = 0;
    aButton.numFrames = 3;

    shadowOAM[ABUTTONOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | aButton.row;
    shadowOAM[ABUTTONOAM].attr1 = ATTR1_SMALL | aButton.col;
    shadowOAM[ABUTTONOAM].attr2 = 9 + (aButton.curFrame * 64);
}

void initBButton() {
    bButton.col = 84;
    bButton.row = 32;
    bButton.height = 16;
    bButton.width = 16;
    bButton.aniCounter = 0;
    bButton.curFrame = 0;
    bButton.numFrames = 3;

    shadowOAM[BBUTTONOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | bButton.row;
    shadowOAM[BBUTTONOAM].attr1 = ATTR1_SMALL | bButton.col;
    shadowOAM[BBUTTONOAM].attr2 = 11 + (bButton.curFrame * 64);
}

void initUpDown() {
    upDown.col = 184;
    upDown.row = 16;
    upDown.height = 32;
    upDown.width = 32;
    upDown.aniCounter = 0;
    upDown.curFrame = 0;
    upDown.numFrames = 3;

    shadowOAM[UPDOWNOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | upDown.row;
    shadowOAM[UPDOWNOAM].attr1 = ATTR1_MEDIUM | upDown.col;
    shadowOAM[UPDOWNOAM].attr2 = 5 + (upDown.curFrame * 128);
}

void initLeftRight() {
    leftRight.col = 128;
    leftRight.row = 16;
    leftRight.height = 32;
    leftRight.width = 32;
    leftRight.aniCounter = 0;
    leftRight.curFrame = 0;
    leftRight.numFrames = 3;

    shadowOAM[LEFTRIGHTOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | leftRight.row;
    shadowOAM[LEFTRIGHTOAM].attr1 = ATTR1_MEDIUM | leftRight.col;
    shadowOAM[LEFTRIGHTOAM].attr2 = 1 + (leftRight.curFrame * 128);
}

void updateInstructArrow1() {
    if (BUTTON_PRESSED(BUTTON_RIGHT) && instructArrow.flip == 0) {
        instructArrow.col = 168;
        instructArrow.flip = 1;
        playSoundB(cocking_data, cocking_length, 0);
    }
    if (BUTTON_PRESSED(BUTTON_LEFT) && instructArrow.flip == 1) {
        instructArrow.col = 31;
        instructArrow.flip = 0;
        playSoundB(cocking_data, cocking_length, 0);
    }

    shadowOAM[INSTRUCTARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | instructArrow.row;
    shadowOAM[INSTRUCTARROWOAM].attr1 = ATTR1_TINY | instructArrow.col;
    shadowOAM[INSTRUCTARROWOAM].attr2 = 30;
}

void updateInstructArrow2() {
    if (BUTTON_PRESSED(BUTTON_RIGHT) && instructArrow.flip == 0) {
        instructArrow.col = 161;
        instructArrow.flip = 1;
        playSoundB(cocking_data, cocking_length, 0);
    }
    if (BUTTON_PRESSED(BUTTON_LEFT) && instructArrow.flip == 1) {
        instructArrow.col = 31;
        instructArrow.flip = 0;
        playSoundB(cocking_data, cocking_length, 0);
    }

    shadowOAM[INSTRUCTARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | instructArrow.row;
    shadowOAM[INSTRUCTARROWOAM].attr1 = ATTR1_TINY | instructArrow.col;
    shadowOAM[INSTRUCTARROWOAM].attr2 = 30;
}

void updateAButton() {
    aButton.aniCounter++;
    if (aButton.aniCounter / 30 == 1) {
        aButton.aniCounter = 0;
        aButton.curFrame = (aButton.curFrame + 1) % aButton.numFrames;
    }

    shadowOAM[ABUTTONOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | aButton.row;
    shadowOAM[ABUTTONOAM].attr1 = ATTR1_SMALL | aButton.col;
    shadowOAM[ABUTTONOAM].attr2 = 9 + (aButton.curFrame * 64);
}

void updateBButton() {
    bButton.aniCounter++;
    if (bButton.aniCounter / 30 == 1) {
        bButton.aniCounter = 0;
        bButton.curFrame = (bButton.curFrame + 1) % bButton.numFrames;
    }

    shadowOAM[BBUTTONOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | bButton.row;
    shadowOAM[BBUTTONOAM].attr1 = ATTR1_SMALL | bButton.col;
    shadowOAM[BBUTTONOAM].attr2 = 11 + (bButton.curFrame * 64);
}

void updateUpDown() {
    upDown.aniCounter++;
    if (upDown.aniCounter / 30 == 1) {
        upDown.aniCounter = 0;
        upDown.curFrame = (upDown.curFrame + 1) % upDown.numFrames;
    }

    shadowOAM[UPDOWNOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | upDown.row;
    shadowOAM[UPDOWNOAM].attr1 = ATTR1_MEDIUM | upDown.col;
    shadowOAM[UPDOWNOAM].attr2 = 5 + (upDown.curFrame * 128);
}

void updateLeftRight() {
    leftRight.aniCounter++;
    if (leftRight.aniCounter / 30 == 1) {
        leftRight.aniCounter = 0;
        leftRight.curFrame = (leftRight.curFrame + 1) % leftRight.numFrames;
    }

    shadowOAM[LEFTRIGHTOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | leftRight.row;
    shadowOAM[LEFTRIGHTOAM].attr1 = ATTR1_MEDIUM | leftRight.col;
    shadowOAM[LEFTRIGHTOAM].attr2 = 1 + (leftRight.curFrame * 128);
}