#include "mode0.h"
#include "sound.h"
#include "start.h"
#include "startMenuBackground0.h"
#include "startMenuBackground1.h"
#include "startMenuBackground2.h"
#include "startMenuBackground3.h"
#include "spritesheet.h"
#include "game.h"

#include "sailingStartMenu.h"
#include "cocking.h"

ANISPRITE menuArrow;
int hOff1;
int hOff2;
int hOff3;

void initStart() {
    REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | SPRITE_ENABLE;
    
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(6) | BG_4BPP | BG_SIZE_SMALL | 1;
    REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(7) | BG_4BPP | BG_SIZE_SMALL | 3;
    REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(12) | BG_4BPP | BG_SIZE_WIDE | 3;
    REG_BG3CNT = BG_CHARBLOCK(3) | BG_SCREENBLOCK(14) | BG_4BPP | BG_SIZE_WIDE | 3;

    hOff1 = 0;
    hOff2 = 0;
    hOff3 = 0;

    REG_BG1HOFF = hOff1;
    REG_BG2HOFF = hOff2;
    REG_BG3HOFF = hOff3;

    DMANow(3, startMenuBackground0Pal, PALETTE, 256);
    DMANow(3, startMenuBackground0Tiles, &CHARBLOCK[0], startMenuBackground0TilesLen / 2);
    DMANow(3, startMenuBackground0Map, &SCREENBLOCK[6], startMenuBackground0MapLen / 2);
    DMANow(3, startMenuBackground1Tiles, &CHARBLOCK[1], startMenuBackground1TilesLen / 2);
    DMANow(3, startMenuBackground1Map, &SCREENBLOCK[7], startMenuBackground1MapLen / 2);
    DMANow(3, startMenuBackground2Tiles, &CHARBLOCK[2], startMenuBackground2TilesLen / 2);
    DMANow(3, startMenuBackground2Map, &SCREENBLOCK[12], startMenuBackground2MapLen / 2);
    DMANow(3, startMenuBackground3Tiles, &CHARBLOCK[3], startMenuBackground3TilesLen / 2);
    DMANow(3, startMenuBackground3Map, &SCREENBLOCK[14], startMenuBackground3MapLen / 2);

    DMANow(3, spritesheetPal, SPRITEPALETTE, 16);
    DMANow(3, spritesheetTiles, &CHARBLOCK[4], spritesheetTilesLen / 2);

    hideSprites();

    playSoundA(sailingStartMenu_data, sailingStartMenu_length, 1);

    initMenuArrow();
    initPlayerSprite();
    initGun();
}

void updateStart() {
    hOff1++;
    if (hOff1 == 256) {
        hOff1 = 0;
    }
    if (hOff1 % 8 == 0) {
        hOff2++;
    }
    if (hOff2 == 512) {
        hOff2 = 0;
    }
    if (hOff2 % 8 == 0 && hOff1 % 8 == 0) {
        hOff3++;
    }
    if (hOff3 == 512) {
        hOff3 = 0;
    }

    REG_BG1HOFF = hOff1;
    REG_BG2HOFF = hOff2;
    REG_BG3HOFF = hOff3;

    updateMenuArrow();
    updatePlayerSprite();
    updateGunSprite();
}

void initMenuArrow() {
    menuArrow.col = 73;
    menuArrow.row = 62;
    menuArrow.height = 8;
    menuArrow.width = 8;
    menuArrow.flip = 0;

    shadowOAM[STARTMENUARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | menuArrow.row;
    shadowOAM[STARTMENUARROWOAM].attr1 = ATTR1_TINY | menuArrow.col;
    shadowOAM[STARTMENUARROWOAM].attr2 = 31;
}

void updateMenuArrow() {
    if (BUTTON_PRESSED(BUTTON_DOWN) && menuArrow.flip == 0) {
        menuArrow.col = 73;
        menuArrow.row = 83;
        menuArrow.flip = 1;
        playSoundB(cocking_data, cocking_length, 0);
    }
    if (BUTTON_PRESSED(BUTTON_UP) && menuArrow.flip == 1) {
        menuArrow.col = 73;
        menuArrow.row = 62;
        menuArrow.flip = 0;
        playSoundB(cocking_data, cocking_length, 0);
    }

    shadowOAM[STARTMENUARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | menuArrow.row;
    shadowOAM[STARTMENUARROWOAM].attr1 = ATTR1_TINY | menuArrow.col;
    shadowOAM[STARTMENUARROWOAM].attr2 = 31;
}

void initPlayerSprite() {
    player.worldCol = 120;
    player.worldRow = 136;
    player.height = 16;
    player.width = 8;
    player.hide = 0;
    player.flip = 0;
    player.angle = 0;
    player.numFrames = 5;
    player.curFrame = 0;
    player.aniCounter = 0;
    player.aniState = PLAYERWALK;
    player.prevAniState = player.aniState;
    player.rdel = 0;
    player.cdel = 0;
    player.jump = 0;

    shadowOAM[PLAYEROAM].attr0 = ATTR0_TALL | ATTR0_4BPP | ATTR0_SQUARE | player.worldRow;
    shadowOAM[PLAYEROAM].attr1 = ATTR1_TINY | player.worldCol;
    shadowOAM[PLAYEROAM].attr2 = (0 + (64 * player.curFrame) + (64 * player.aniState)) | (2 << 10);
}

void updatePlayerSprite() {
    if ((player.aniCounter / 10) == 1) {
        player.curFrame = (player.curFrame + 1) % player.numFrames;
        player.aniCounter = 0;
    }

    player.aniCounter++;

    shadowOAM[PLAYEROAM].attr0 = ATTR0_TALL | ATTR0_4BPP | ATTR0_SQUARE | player.worldRow;
    shadowOAM[PLAYEROAM].attr1 = ATTR1_TINY | player.worldCol;
    shadowOAM[PLAYEROAM].attr2 = (0 + (64 * player.curFrame) + (64 * player.aniState)) | (2 << 10);
}

void updateGunSprite() {
    shadowOAM[GUNOAM].attr0 = ATTR0_AFFINE | ATTR0_4BPP | ATTR0_SQUARE | gun.worldRow;
    shadowOAM[GUNOAM].attr1 = ATTR1_TINY | gun.worldCol;
    shadowOAM[GUNOAM].attr2 = (1 + (32 * gun.curFrame)) | (2 << 10);
    SHADOW_OAM_AFF[GUNAFFINE].a = sin_lut_fixed8[(gun.angle + 90) % 360];
    SHADOW_OAM_AFF[GUNAFFINE].b = -sin_lut_fixed8[gun.angle % 360];
    SHADOW_OAM_AFF[GUNAFFINE].c = sin_lut_fixed8[gun.angle % 360];
    SHADOW_OAM_AFF[GUNAFFINE].d = sin_lut_fixed8[(gun.angle + 90) % 360];
}
