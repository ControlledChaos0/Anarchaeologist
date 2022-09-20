#include "mode0.h"
#include "game.h"
#include "level3.h"
#include "gameLevel3Background0.h"
#include "gameLevel3Background1.h"
#include "gameLevel3Background2.h"
#include "gameLevel0Background3.h"
#include "gameLevel3Collision.h"
#include "spritesheet.h"
#include <stdlib.h>

void initLevel3() {
    DMANow(3, gameLevel3Background0Pal, PALETTE, 256);
    DMANow(3, gameLevel3Background0Tiles, &CHARBLOCK[0], gameLevel3Background0TilesLen / 2);
    DMANow(3, gameLevel3Background0Map, &SCREENBLOCK[7], gameLevel3Background0MapLen / 2);
    DMANow(3, gameLevel3Background1Tiles, &CHARBLOCK[1], gameLevel3Background1TilesLen / 2);
    DMANow(3, gameLevel3Background1Map, &SCREENBLOCK[26], gameLevel3Background1MapLen / 2);
    DMANow(3, gameLevel3Background2Tiles, &CHARBLOCK[2], gameLevel3Background2TilesLen / 2);
    DMANow(3, gameLevel3Background2Map, &SCREENBLOCK[28], gameLevel3Background2MapLen / 2);
    DMANow(3, gameLevel0Background3Tiles, &CHARBLOCK[3], gameLevel0Background3TilesLen / 2);
    DMANow(3, gameLevel0Background3Map, &SCREENBLOCK[30], gameLevel0Background3MapLen / 2);

    DMANow(3, spritesheetPal, SPRITEPALETTE, 256);
    DMANow(3, spritesheetTiles, &CHARBLOCK[4], spritesheetTilesLen / 2);

    currentCollisionMap = (unsigned short *) malloc(gameLevel3CollisionBitmapLen * sizeof(*currentCollisionMap));
    DMANow(3, gameLevel3CollisionBitmap, currentCollisionMap, gameLevel3CollisionBitmapLen / 2);

    initPlayer3();
    initBoxes3();
    initTarget3();
    initDoor3();
}

void initPlayer3() {
    player.worldCol = 17;
    player.worldRow = 80;
    player.height = 16;
    player.width = 8;
    player.hide = 0;
    player.flip = 0;
    player.angle = 0;
    player.numFrames = 5;
    player.curFrame = 0;
    player.aniCounter = 0;
    player.aniState = PLAYERIDLE;
    player.prevAniState = player.aniState;
    player.rdel = 0;
    player.cdel = 0;
    player.jump = 1;

    shadowOAM[PLAYEROAM].attr0 = ATTR0_TALL | ATTR0_4BPP | ATTR0_SQUARE | player.worldRow;
    shadowOAM[PLAYEROAM].attr1 = ATTR1_TINY | player.worldCol;
    shadowOAM[PLAYEROAM].attr2 = (0 + (64 * player.curFrame) + (64 * player.aniState)) | (2 << 10);
}

void initBoxes3() {
    activeBoxes = 2;
    for (int i = 0; i < activeBoxes; i++) {
        moveableBlocks[i].angle = 0;
        moveableBlocks[i].aniCounter = 0;
        moveableBlocks[i].aniState = 0;
        moveableBlocks[i].cdel = 0;
        moveableBlocks[i].curFrame = 0;
        moveableBlocks[i].height = 8;
        moveableBlocks[i].hide = 0;
        moveableBlocks[i].number = i;
        moveableBlocks[i].numFrames = 0;
        moveableBlocks[i].prevAniState = 0;
        moveableBlocks[i].rdel = 0;
        moveableBlocks[i].type = 0;
        moveableBlocks[i].width = 8;
        moveableBlocks[i].worldCol = 100 + (8 * i);
        moveableBlocks[i].worldRow = 120;

        if (!moveableBlocks[i].hide) {
            shadowOAM[BOXOAM + i].attr0 = ATTR0_AFFINE | ATTR0_4BPP | ATTR0_SQUARE | moveableBlocks[i].worldRow;
            shadowOAM[BOXOAM + i].attr1 = ATTR1_TINY | ((BOXAFFINE + i) << 9) | moveableBlocks[i].worldCol;
            shadowOAM[BOXOAM + i].attr2 = (2 + (32 * moveableBlocks[i].type)) | (2 << 10);
            SHADOW_OAM_AFF[BOXAFFINE + i].a = sin_lut_fixed8[(moveableBlocks[i].angle + 90) % 360];
            SHADOW_OAM_AFF[BOXAFFINE + i].b = -sin_lut_fixed8[moveableBlocks[i].angle % 360];
            SHADOW_OAM_AFF[BOXAFFINE + i].c = sin_lut_fixed8[moveableBlocks[i].angle % 360];
            SHADOW_OAM_AFF[BOXAFFINE + i].d = sin_lut_fixed8[(moveableBlocks[i].angle + 90) % 360];
        }

        rowOrder[i] = &moveableBlocks[i];
    }
}

void initDoor3() {
    door.worldCol = 208;
    door.worldRow = 112;
    door.height = 16;
    door.width = 8;
    door.hide = 0;
    door.flip = 0;
    door.angle = 0;
    door.numFrames = 1;
    door.curFrame = 0;
    door.aniCounter = 0;
    door.aniCounter = 0;
    door.prevAniState = door.aniState;
    door.rdel = 0;
    door.cdel = -1;
    door.jump = 1;

    shadowOAM[DOOROAM].attr0 = ATTR0_TALL | ATTR0_4BPP | ATTR0_SQUARE | door.worldRow;
    shadowOAM[DOOROAM].attr1 = ATTR1_TINY | door.worldCol;
    shadowOAM[DOOROAM].attr2 = (4 + (64 * door.curFrame)) | (2 << 10);
}

void initTarget3() {
    target.worldCol = 216;
    target.worldRow = 24;
    target.height = 8;
    target.width = 8;
    target.hide = 0;
    target.flip = 0;
    target.angle = 0;
    target.numFrames = 1;
    target.curFrame = 0;
    target.aniCounter = 0;
    target.aniCounter = 0;
    target.prevAniState = target.aniState;
    target.rdel = 0;
    target.cdel = 0;
    target.jump = 1;

    shadowOAM[TARGETOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | target.worldRow;
    shadowOAM[TARGETOAM].attr1 = ATTR1_TINY | target.worldCol;
    shadowOAM[TARGETOAM].attr2 = (5 + (32 * target.curFrame)) | (2 << 10);
}