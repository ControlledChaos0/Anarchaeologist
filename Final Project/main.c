/**
 * @file main.c
 * @author Jade :)
 * @brief 
 * @version 0.5
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 * FINISHED: Almost all art and gameplay elements are done.
 * 
 * BUGS: The only gameplay element that is broken right now is the changing of the collision map when you break the breakable block, meaning that
 * even after when you break it, the area still remains blocked. This will either be implemented through changing the collision map or just checking the tile
 * instead of the collision map. The audio is a little messed up right now as well as the moving off the error clicks strangly and when you go to the instruction
 * screen, there's a screech for some reason that I haven't figured out yet.
 * 
 * NEEDS TO BE ADDED: The only things that have yet to be added are the other levels, traversal of the level map to get to these other levels, a limited number of shots before making
 * you restart the level (which means a lose screen may need to be implemented), the cheat that lets you have unlimited bullets, and more audio. 
 * For the most part, this is very close to being completed.
 * 
 * CONTROLS:
 *      A: JUMP
 *      B: SHOOT
 *      LEFT/RIGHT: MOVEMENT
 *      UP/DOWN: GUN ROTATION
 * The goal is to shoot the target that is located somewhere in the level that needs proper aiming and bouncing to do. This
 * will open the door and allow you to leave the room and finish the level.
 * However, the instructions will hopefully be helpful enough :)
 * 
 * Also, the timer may seem arbitrary, but after you beat the level and go back out to the main menu, go back to the level select screen. It
 * keeps track of your fastest time which is pretty cool.
 */


#include <stdlib.h>
#include <stdio.h>
#include "mode0.h"
#include "sound.h"
#include "print.h"
#include "spritesheet.h"
#include "start.h"
#include "instructions.h"
#include "levelSelect.h"
#include "game.h"
#include "pause.h"
#include "win.h"

// Prototypes.
void initialize();

// State Prototypes.
void goToStart();
void start();
void goToInstructions();
void instructions();
void goToLevelSelect();
void levelSelect();
void goToGame();
void game();
void goToPause();
void pause();
void goToWin();
void win();
void goToLose();
void lose();

// States.
enum {
    START,
    INSTRUCTIONS,
    LEVELSELECT,
    GAME,
    PAUSE,
    WIN,
    LOSE
};
int state;

// Button Variables.
unsigned short buttons;
unsigned short oldButtons;

// Shadow OAM.
OBJ_ATTR shadowOAM[128];
OBJ_AFFINE* SHADOW_OAM_AFF = (OBJ_AFFINE*)shadowOAM;

int main() {
    initialize();

    mgba_open();

    while (1) {
        // Update button variables.
        oldButtons = buttons;
        buttons = BUTTONS;

        // State Machine.
        switch (state) {
        case START:
            start();
            break;
        case INSTRUCTIONS:
            instructions();
            break;
        case LEVELSELECT:
            levelSelect();
            break;
        case GAME:
            game();
            break;
        case PAUSE:
            pause();
            break;
        case WIN:
            win();
            break;
        case LOSE:
            lose();
            break;
        }
    }
}

// Sets up GBA.
void initialize() {

    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(6) | BG_4BPP | BG_SIZE_SMALL | 1;
    REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(14) | BG_4BPP | BG_SIZE_SMALL | 3;
    REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(22) | BG_4BPP | BG_SIZE_SMALL | 3;
    REG_BG3CNT = BG_CHARBLOCK(3) | BG_SCREENBLOCK(30) | BG_4BPP | BG_SIZE_SMALL | 3;

    REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | SPRITE_ENABLE; // Bitwise OR the BG(s) you want to use and Bitwise OR SPRITE_ENABLE if you want to use sprites.
    // Don't forget to set up whatever BGs you enabled in the line above!
    
    buttons = BUTTONS;
    oldButtons = 0;
    levelsUnlocked = 1;
    level = 0;

    for (int i = 0; i < LEVELS; i++) {
        levelTimes[i] = -1;
    }

    setupSounds();
    setupInterrupts();

    goToStart();
}

// Sets up the start state.
void goToStart() {
    initStart();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM, 128 * 4);

    state = START;
}

// Runs every frame of the start state.
void start() {
    updateStart();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM, 128 * 4);

    if (menuArrow.flip == 0 && BUTTON_PRESSED(BUTTON_A)) {
        goToLevelSelect();
    }
    if (menuArrow.flip == 1 && BUTTON_PRESSED(BUTTON_A)) {
        goToInstructions();
    }
    if (BUTTON_HELD(BUTTON_L) && BUTTON_HELD(BUTTON_R)) {
        levelsUnlocked = LEVELS;
    }
}

void goToInstructions() {
    instructScreenNum = 1;
    initInstruct();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM, 128 * 4);

    state = INSTRUCTIONS;
}

void instructions() {
    updateInstruct();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM , 128 * 4);

    if (instructScreenNum == 0) {
        goToStart();
    }
    if (instructScreenNum == 3) {
        goToLevelSelect();
    }
}

void goToLevelSelect() {
    initLevelSelect();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM , 128 * 4);

    state = LEVELSELECT;
}

void levelSelect() {
    updateLevelSelect();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM, 128 * 4);

    if (BUTTON_PRESSED(BUTTON_A) && !smallPlayer.jump) {
        goToGame();
    }
}

// Sets up the game state.
void goToGame() {
    if (state == LEVELSELECT || state == INSTRUCTIONS || state == GAME || pauseArrow.flip == 1) {
        initGame();

        waitForVBlank();

        DMANow(3, shadowOAM, OAM, 128 * 4);
    }
    state = GAME;
}

// Runs every frame of the game state.
void game() {
    updateGame();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM , 128 * 4);

    if (BUTTON_PRESSED(BUTTON_SELECT)) {
        goToPause();
    }
    if (BUTTON_PRESSED(BUTTON_START)) {
        free(currentCollisionMap);
        goToGame();
    }
    if (winInt) {
        if (levelTimes[level] == -1 || ((minute * 60) + second) < levelTimes[level]) {
            levelTimes[level] = (minute * 60) + second;
        }
        free(currentCollisionMap);
        if (!((levelsUnlocked - 1) - level)) {
            levelsUnlocked++;
        }
        if (level == 4) {
            goToWin();
        } else {
            goToLevelSelect();
        }
    }
}

// Sets up the pause state.
void goToPause() {
    pauseTimer();

    initPause();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM , 128 * 4);

    state = PAUSE;
}

// Runs every frame of the pause state.
void pause() {
    updatePause();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM , 128 * 4);

    if (BUTTON_PRESSED(BUTTON_A)) {
        disablePause();
        if (pauseArrow.flip == 0) {
            startTimer();
            goToGame();
        }
        if (pauseArrow.flip == 1) {
            free(currentCollisionMap);
            goToGame();
        }
        if (pauseArrow.flip == 2) {
            free(currentCollisionMap);
            goToStart();
        }
    }
}

// Sets up the win state.
void goToWin() {
    initWin();

    state = WIN;
}

// Runs every frame of the win state.
void win() {
    updateWin();

    waitForVBlank();

    DMANow(3, shadowOAM, OAM, 128 * 4);

    if (BUTTON_PRESSED(BUTTON_A)) {
        goToStart();
    }
}

// Sets up the lose state.
void goToLose() {
    state = LOSE;
}

// Runs every frame of the lose state.
void lose() {}