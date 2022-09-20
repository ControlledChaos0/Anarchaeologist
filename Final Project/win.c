#include "mode0.h"
#include "win.h"
#include "winBackground0.h"
#include "winBackground1.h"
#include "spritesheet.h"

void initWin() {
    hideSprites();
    REG_DISPCTL = MODE0 | BG0_ENABLE | SPRITE_ENABLE;
    
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(31) | BG_4BPP | BG_SIZE_SMALL | 1;

    DMANow(3, winBackground0Pal, PALETTE, 256);
    DMANow(3, winBackground0Tiles, &CHARBLOCK[0], winBackground0TilesLen / 2);
    DMANow(3, winBackground0Map, &SCREENBLOCK[31], winBackground0MapLen / 2);

    DMANow(3, spritesheetPal, SPRITEPALETTE, 256);
    DMANow(3, spritesheetTiles, &CHARBLOCK[4], spritesheetTilesLen / 2);
}

void updateWin() {

}