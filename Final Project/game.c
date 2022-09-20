#include "mode0.h"
#include "print.h"
#include "game.h"
#include "level0.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"
#include "level4.h"
#include "levelSelect.h"
#include "gameLevel0Collision.h"
#include "gameLevel1Collision.h"
#include "gameLevel2Collision.h"
#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "sunshineGame.h"
#include "gunshot.h"

SPRITE player;
SPRITE gun;
SPRITE bullet;
SPRITE door;
SPRITE target;
BOXSPRITE moveableBlocks[BOXCOUNT];
BOXSPRITE* rowOrder[BOXCOUNT];
int boxes[BOXCOUNT];
int activeBoxes;

int level;
int winInt;
int second;
int minute;

const unsigned short* collisionMaps[] = {gameLevel0CollisionBitmap, gameLevel1CollisionBitmap, gameLevel2CollisionBitmap};
unsigned short * currentCollisionMap;

void initGame() {
    hideSprites();
    REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | SPRITE_ENABLE;
    
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(7) | BG_4BPP | BG_SIZE_SMALL | 1;
    REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(26) | BG_4BPP | BG_SIZE_SMALL | 3;
    REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(28) | BG_4BPP | BG_SIZE_SMALL | 3;
    REG_BG3CNT = BG_CHARBLOCK(3) | BG_SCREENBLOCK(30) | BG_4BPP | BG_SIZE_SMALL | 3;

    REG_BG1HOFF = 0;
    REG_BG2HOFF = 0;
    REG_BG3HOFF = 0;

    REG_BLDCNT = 0;

    setInterrupt();

    second = 0;
    minute = 0;

    winInt = 0;

    playSoundA(sunshineGame_data, sunshineGame_length, 1);

    switch(level) {
        case 0:
            initLevel0();
            break;
        case 1:
            initLevel1();
            break;
        case 2:
            initLevel2();
            break;
        case 3:
            initLevel3();
            break;
        case 4:
            initLevel4();
            break;
    }

    initGun();
    initBullet();
    char tenMin = (minute / 10) + '0';
    char oneMin = (minute % 10) + '0';
    char tenSec = (second / 10) + '0';
    char oneSec = (second % 10) + '0';
    char timeString[7] = {tenMin, oneMin, ':', tenSec, oneSec, '\0'};
    drawString0(204, 4, timeString, 10);
}

void updateGame() {
    //mgba_printf("UPDATING");
    clearPush();
    qsort(rowOrder, activeBoxes, sizeof(BOXSPRITE*), compareFunc);
    //for (int i = 0; i < activeBoxes; i++) {
    //    mgba_printf("Row: %d, Velocity: %d", (*rowOrder[i]).worldRow, (*rowOrder[i]).rdel);
    //}
    updatePlayer();
    updateGun();
    for (int i = 0; i < activeBoxes; i++) {
        fallBoxes(rowOrder[i]);
    }
    for (int i = 0; i < activeBoxes; i++) {
        updateBoxes(&moveableBlocks[i]);
    }
    updateBullet();
    updateTarget();
    updateDoor();
    char tenMin = (minute / 10) + '0';
    char oneMin = (minute % 10) + '0';
    char tenSec = (second / 10) + '0';
    char oneSec = (second % 10) + '0';
    char timeString[7] = {tenMin, oneMin, ':', tenSec, oneSec, '\0'};
    drawString0(204, 4, timeString, 10);
}

void initPlayer() {
    player.worldCol = 17;
    player.worldRow = 40;
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
    player.jump = 0;

    shadowOAM[PLAYEROAM].attr0 = ATTR0_TALL | ATTR0_4BPP | ATTR0_SQUARE | player.worldRow;
    shadowOAM[PLAYEROAM].attr1 = ATTR1_TINY | player.worldCol;
    shadowOAM[PLAYEROAM].attr2 = (0 + (64 * player.curFrame) + (64 * player.aniState)) | (2 << 10);
}

void updatePlayer() {
    player.prevAniState = player.aniState;
    player.aniState = PLAYERIDLE;

    if (BUTTON_HELD(BUTTON_LEFT) || BUTTON_PRESSED(BUTTON_LEFT)) {
        player.cdel = -PLAYERVEL;
        player.cdel = collisionCheckHor(player);
        player.aniState = PLAYERWALK;
        player.worldCol += player.cdel;
        player.flip = 1;
    } else if (BUTTON_HELD(BUTTON_RIGHT) || BUTTON_PRESSED(BUTTON_RIGHT)) {
        player.cdel = PLAYERVEL;
        player.cdel = collisionCheckHor(player);
        player.aniState = PLAYERWALK;
        player.worldCol += player.cdel;
        player.flip = 0;
    }
    
    if (BUTTON_PRESSED(BUTTON_A) && !player.jump) {
        player.aniState = PLAYERJUMP;
        player.rdel = -(PLAYERVERTVEL << 3);
        player.worldRow += (player.rdel >> 3);
        player.jump = 1;
    } else {
        player.rdel = (player.rdel + (int)(8 * GRAVITY));
        int collisionVer = collisionCheckVer(player);
        player.rdel = collisionVer;
        if (player.rdel != 0 || collisionVer != 0) {
            player.aniState = PLAYERJUMP;
        } else {
            player.jump = 0;
        }
        player.worldRow += (player.rdel >> 3);
    }

    if ((player.aniCounter / 10) == 1) {
        player.curFrame = (player.curFrame + 1) % player.numFrames;
        player.aniCounter = 0;
    }

    if (player.aniState == PLAYERIDLE || player.aniState == PLAYERJUMP) {
        player.curFrame = 0;
    } else {
        player.aniCounter++;
    }

    //mgba_printf("Player rdel: %d", player.rdel >> 3);

    shadowOAM[PLAYEROAM].attr0 = ATTR0_TALL | ATTR0_4BPP | ATTR0_SQUARE | player.worldRow;
    shadowOAM[PLAYEROAM].attr1 = ATTR1_TINY | player.worldCol;
    if (player.flip) {
        shadowOAM[PLAYEROAM].attr1 |= ATTR1_HFLIP;
    }
    shadowOAM[PLAYEROAM].attr2 = (0 + (64 * player.curFrame) + (64 * player.aniState)) | (2 << 10);
}

void initGun() {
    gun.worldCol = player.worldCol + 2;
    gun.worldRow = player.worldRow + 4;
    gun.height = 8;
    gun.width = 8;
    gun.hide = 0;
    gun.flip = 0;
    gun.angle = 90;
    gun.numFrames = 1;
    gun.curFrame = 0;
    gun.aniCounter = 0;
    gun.aniCounter = 0;
    gun.prevAniState = gun.aniState;
    gun.rdel = 0;
    gun.cdel = 0;

    shadowOAM[GUNOAM].attr0 = ATTR0_AFFINE | ATTR0_4BPP | ATTR0_SQUARE | gun.worldRow;
    shadowOAM[GUNOAM].attr1 = ATTR1_TINY | gun.worldCol;
    shadowOAM[GUNOAM].attr2 = (1 + (32 * gun.curFrame)) | (2 << 10);
    SHADOW_OAM_AFF[GUNAFFINE].a = sin_lut_fixed8[(gun.angle + 90) % 360];
    SHADOW_OAM_AFF[GUNAFFINE].b = -sin_lut_fixed8[gun.angle % 360];
    SHADOW_OAM_AFF[GUNAFFINE].c = sin_lut_fixed8[gun.angle % 360];
    SHADOW_OAM_AFF[GUNAFFINE].d = sin_lut_fixed8[(gun.angle + 90) % 360];
}

void updateGun() {
    int prevFlip = gun.flip;
    if (player.flip) {
        gun.worldCol = player.worldCol - 3;
        gun.worldRow = player.worldRow + 4;
        gun.flip = 1;       
    } else {
        gun.worldCol = player.worldCol + 2;
        gun.worldRow = player.worldRow + 4;
        gun.flip = 0;
    }
    if (gun.flip != prevFlip) {
        gun.angle = (90 - gun.angle) + 90;
    } 

    if ((BUTTON_PRESSED(BUTTON_UP) || BUTTON_HELD(BUTTON_UP))) {
        if (gun.flip && gun.angle > 0) {
            gun.angle--;
        } else  if (!gun.flip && gun.angle < 180) {
            gun.angle++;
        }
    } else if (BUTTON_PRESSED(BUTTON_DOWN) || BUTTON_HELD(BUTTON_DOWN)) {
        if (gun.flip && gun.angle < 180) {
            gun.angle++;
        } else if (!gun.flip && gun.angle > 0) {
            gun.angle--;
        }
    }

    if (BUTTON_PRESSED(BUTTON_B)) {
        fireBullet();
    }

    shadowOAM[GUNOAM].attr0 = ATTR0_AFFINE | ATTR0_4BPP | ATTR0_SQUARE | gun.worldRow;
    shadowOAM[GUNOAM].attr1 = ATTR1_TINY | gun.worldCol;
    if (gun.flip) {
        SHADOW_OAM_AFF[GUNAFFINE].c = -sin_lut_fixed8[gun.angle % 360];
        SHADOW_OAM_AFF[GUNAFFINE].d = -sin_lut_fixed8[(gun.angle + 90) % 360];
    } else {
        SHADOW_OAM_AFF[GUNAFFINE].c = sin_lut_fixed8[gun.angle % 360];
        SHADOW_OAM_AFF[GUNAFFINE].d = sin_lut_fixed8[(gun.angle + 90) % 360];
    }
    shadowOAM[GUNOAM].attr2 = (1 + (32 * gun.curFrame)) | (2 << 10);
    SHADOW_OAM_AFF[GUNAFFINE].a = sin_lut_fixed8[(gun.angle + 90) % 360];
    SHADOW_OAM_AFF[GUNAFFINE].b = -sin_lut_fixed8[gun.angle % 360];
}

void initBoxes() {
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
        moveableBlocks[i].worldCol = 40 + (10 * i);
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

void fallBoxes(BOXSPRITE* sprite) {
    if (!sprite->hide) {
        sprite->rdel = (sprite->rdel + (int)(8 * GRAVITY));
        collisionBoxCheckVer(sprite, sprite->rdel);
        sprite->worldRow += sprite->rdel >> 3;
    }
}

void updateBoxes(BOXSPRITE* sprite) {
    if (!(sprite->hide)) {

        if (sprite->isPushed) {
            sprite->worldCol += sprite->cdel;
        }

        shadowOAM[BOXOAM + sprite->number].attr0 = ATTR0_AFFINE | ATTR0_4BPP | ATTR0_SQUARE | sprite->worldRow;
        shadowOAM[BOXOAM + sprite->number].attr1 = ATTR1_TINY | ((BOXAFFINE + sprite->number) << 9) | sprite->worldCol;
        shadowOAM[BOXOAM + sprite->number].attr2 = (2 + (32 * sprite->type)) | (2 << 10);
        SHADOW_OAM_AFF[BOXAFFINE + sprite->number].a = sin_lut_fixed8[(sprite->angle + 90) % 360];
        SHADOW_OAM_AFF[BOXAFFINE + sprite->number].b = -sin_lut_fixed8[sprite->angle % 360];
        SHADOW_OAM_AFF[BOXAFFINE + sprite->number].c = sin_lut_fixed8[sprite->angle % 360];
        SHADOW_OAM_AFF[BOXAFFINE + sprite->number].d = sin_lut_fixed8[(sprite->angle + 90) % 360];
    }
}

void clearPush() {
    for (int i = 0; i < BOXCOUNT; i++) {
        boxes[i] = 0;
        moveableBlocks[i].isPushed = 0;
    }
}

void initDoor() {
    door.worldCol = 224;
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

void updateDoor() {
    if (target.flip && door.flip) {
        door.flip = (door.flip + 1) % 17;
        door.worldRow += door.cdel;
    }
    shadowOAM[DOOROAM].attr0 = ATTR0_TALL | ATTR0_4BPP | ATTR0_SQUARE | door.worldRow;
    shadowOAM[DOOROAM].attr1 = ATTR1_TINY | door.worldCol;
    shadowOAM[DOOROAM].attr2 = (4 + (64 * door.curFrame)) | (2 << 10);
}

void initTarget() {
    target.worldCol = 108;
    target.worldRow = 56;
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

void updateTarget() {
    shadowOAM[TARGETOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | target.worldRow;
    shadowOAM[TARGETOAM].attr1 = ATTR1_TINY | target.worldCol;
    shadowOAM[TARGETOAM].attr2 = (5 + (32 * target.curFrame)) | (2 << 10);
}

void initBullet() {
    bullet.worldCol = 0;
    bullet.worldRow = 0;
    bullet.rdel = 0;
    bullet.cdel = 0;
    bullet.width = 2;
    bullet.height = 2;

    bullet.hide = 1;

    shadowOAM[BULLETOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | (bullet.worldRow >> 8);
    shadowOAM[BULLETOAM].attr1 = ATTR1_TINY | (bullet.worldCol >> 8);
    shadowOAM[BULLETOAM].attr2 = (3 + (32 * bullet.curFrame)) | (2 << 10);
}

void updateBullet() {
    //mgba_printf("UPDATING");
    if (bullet.hide == 0) {
        if ((bullet.worldCol >> 8) < 0 || (bullet.worldCol >> 8) >= SCREENWIDTH) {
            bullet.hide = 1;
        }
        if ((bullet.worldRow >> 8) < 0 || (bullet.worldRow >> 8) >= SCREENHEIGHT) {
            bullet.hide = 1;
        }
        collisionBulletCheck(0, 256);
    }

    if (bullet.hide == 0) {
        shadowOAM[BULLETOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | (bullet.worldRow >> 8);
        shadowOAM[BULLETOAM].attr1 = ATTR1_TINY | (bullet.worldCol >> 8);
        shadowOAM[BULLETOAM].attr2 = (3 + (32 * bullet.curFrame)) | (2 << 10);
    } else {
        shadowOAM[BULLETOAM].attr0 = ATTR0_HIDE;
    }
}

void fireBullet() {
    if (bullet.hide != 0) {
        playSoundB(gunshot_data, gunshot_length, 0);
        int x;
        int y;
        if (gun.flip != 0) {
            x = (SHADOW_OAM_AFF[GUNAFFINE].a * 0) + (SHADOW_OAM_AFF[GUNAFFINE].b * 4);
            y = (SHADOW_OAM_AFF[GUNAFFINE].c * 0) + (SHADOW_OAM_AFF[GUNAFFINE].d * 4);
        } else {
            x = (SHADOW_OAM_AFF[GUNAFFINE].a * 0) + (-SHADOW_OAM_AFF[GUNAFFINE].b * 4);
            y = (-SHADOW_OAM_AFF[GUNAFFINE].c * 0) + (SHADOW_OAM_AFF[GUNAFFINE].d * 4);
        }
        x = ((gun.worldCol + 3) * 256) + x;
        y = ((gun.worldRow + 3) * 256) + y;
        int notCollided = checkBulletSpawn(x >> 8, y >> 8);
        if (!notCollided) {
            if (gun.flip != 0) {
                bullet.rdel = -BULLETSPEED * sin_lut_fixed8[gun.angle + 90];
                bullet.cdel = -BULLETSPEED * sin_lut_fixed8[gun.angle];
            } else {
                bullet.rdel = BULLETSPEED * sin_lut_fixed8[gun.angle + 90];
                bullet.cdel = BULLETSPEED * sin_lut_fixed8[gun.angle];
            }
            bullet.worldCol = x;
            bullet.worldRow = y;
            bullet.hide = 0;
        }
    }
    //mgba_printf("Player rdel: %d", ((1 / 2) << 8) + ((1 % 2) << 7));
}

int compareFunc(const void * a, const void * b) {
    if ((((BOXSPRITE*)b)->hide && ((BOXSPRITE*)a)->hide)) {
        return 0;
    } else if (((BOXSPRITE*)b)->hide) {
        return INT_MIN;
    } else if (((BOXSPRITE*)a)->hide) {
        return INT_MAX;
    }
    return (((BOXSPRITE*)b)->worldRow - ((BOXSPRITE*)a)->worldRow);
}

void setInterrupt() {
    REG_IME = 0;

    REG_IE |= INT_TM2;
    REG_IE |= INT_TM3;

    REG_TM2CNT = 0;
    REG_TM2D = 65536 - 16384;
    REG_TM2CNT = TM_FREQ_1024 | TM_IRQ | TIMER_ON;

    REG_TM3CNT = 0;
    REG_TM3D = 65536 - 60;
    REG_TM3CNT = TM_CASCADE | TM_IRQ | TIMER_ON;

    REG_INTERRUPT = interruptHandle;
    REG_IME = 1;
}

void pauseTimer() {
    REG_TM0CNT = 0;
    REG_TM2CNT = 0;
}

void startTimer() {
    REG_TM2CNT = TM_FREQ_1024 | TM_IRQ | TIMER_ON;
    REG_TM0CNT |= TIMER_ON;
}

void interruptHandle() {
    REG_IME = 0;

    if (REG_IF & INT_TM2) {
        second++;
        if (second == 60) {
            second = 0;
        }
    }
    if (REG_IF & INT_TM3) {
        minute++;
        if (minute == 100) {
            minute = 99;
        }
    }
    if(REG_IF & INT_VBLANK) {
        if (soundA.isPlaying) {

            soundA.vBlankCount = soundA.vBlankCount + 1;
            if (soundA.vBlankCount > soundA.duration) {
                if (soundA.loops) {
                    playSoundA(soundA.data, soundA.length, soundA.loops);
                } else {
                    soundA.isPlaying = 0;
                    dma[1].cnt = 0;
                    REG_TM0CNT = TIMER_OFF;
                }
            }
        }

        if (soundB.isPlaying) {

            soundB.vBlankCount = soundB.vBlankCount + 1;
            if (soundB.vBlankCount > soundB.duration) {
                if (soundB.loops) {
                    playSoundB(soundB.data, soundB.length, soundB.loops);
                } else {
                    soundB.isPlaying = 0;
                    dma[2].cnt = 0;
                    REG_TM1CNT = TIMER_OFF;
                }
            }
		}
	}

    REG_IF = REG_IF;
    REG_IME = 1;
}