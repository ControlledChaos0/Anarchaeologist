#ifndef LEVELSELECT_H
#define LEVELSELECT_H

extern int levelsUnlocked;

extern SPRITE tensMinute;
extern SPRITE onesMinute;
extern SPRITE middle;
extern SPRITE tensSecond;
extern SPRITE onesSecond;

extern SPRITE smallPlayer;

#define LEVELS 5
extern int levelTimes[LEVELS];

typedef struct {
    int x;
    int y;
} LEVEL_POSITION;

void initLevelSelect();
void updateLevelSelect();

void initSmallPlayer();
void updateSmallPlayer();

void initTime();
void updateTime();

void initPositions();
void clearPathsLeft();
void clearPathsRight();
void clearRight();
void clearLeft();

void removeFirstPath();
void removeSecondPath();
void removeThirdPath();
void removeFourthPath();

void drawLevelSign();
void eraseLevelSign();

#endif