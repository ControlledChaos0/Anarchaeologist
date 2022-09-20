#include "mode0.h"
#include "pause.h"
#include "pausedBackground2.h"
#include "spritesheet.h"
#include "sound.h"

#include "cocking.h"

ANISPRITE pauseArrow;
int pauseTileArray[] = {17, 18, 19, 20, 21, 17 + (1 << 10), 22, 23, 24, 25, 26, 27, 28,
                17, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                41, 42, 43, 44, 43 + (1 << 10), 45, 46, 47, 46, 48, 49, 45 + (1 << 10), 0,
                0, 50, 51, 52, 53, 54, 55, 56, 0, 0, 0, 0, 0,
                0, 57, 58, 59, 60, 61, 62, 63, 0, 0, 0, 0, 0,
                0, 64, 65, 66, 67, 68, 69, 0, 0, 0, 0, 0, 0,
                0, 70, 71, 72, 73, 74, 75, 0, 0, 0, 0, 0, 0,
                0, 64, 76, 66, 77, 78, 79, 80, 81, 82, 83, 84, 0,
                0, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 0};

void initPause() {
    for (int j = 0; j < 9; j++) {
        for (int i = 0; i < 13; i++) {
            SCREENBLOCK[7].tilemap[OFFSET(7 + i, 5 + j, 32)] = pauseTileArray[i + (j * 13)];
        }
    }

    initPauseArrow();
}

void updatePause() {
    updatePauseArrow();
}

void disablePause() {
    for (int j = 0; j < 9; j++) {
        for (int i = 0; i < 13; i++) {
            SCREENBLOCK[7].tilemap[OFFSET(7 + i, 5 + j, 32)] = 0;
        }
    }
    shadowOAM[PAUSEARROWOAM].attr0 = ATTR0_HIDE;
}

void initPauseArrow() {
    pauseArrow.col = 55;
    pauseArrow.row = 70;
    pauseArrow.height = 8;
    pauseArrow.width = 8;
    pauseArrow.flip = 0;

    shadowOAM[PAUSEARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | pauseArrow.row;
    shadowOAM[PAUSEARROWOAM].attr1 = ATTR1_TINY | pauseArrow.col;
    shadowOAM[PAUSEARROWOAM].attr2 = 31;
}

void updatePauseArrow() {
    if (BUTTON_PRESSED(BUTTON_DOWN) && pauseArrow.flip != 2) {
        pauseArrow.row += 16;
        pauseArrow.flip++;
        playSoundB(cocking_data, cocking_length, 0);
    }
    if (BUTTON_PRESSED(BUTTON_UP) && pauseArrow.flip != 0) {
        pauseArrow.row -= 16;
        pauseArrow.flip--;
        playSoundB(cocking_data, cocking_length, 0);
    }

    shadowOAM[PAUSEARROWOAM].attr0 = ATTR0_REGULAR | ATTR0_4BPP | ATTR0_SQUARE | pauseArrow.row;
    shadowOAM[PAUSEARROWOAM].attr1 = ATTR1_TINY | pauseArrow.col;
    shadowOAM[PAUSEARROWOAM].attr2 = 31;
}