#ifndef GAME_H
#define GAME_H

extern int level;
extern int winInt;

extern int second;
extern int minute;

extern const unsigned short* collisionMaps[];
extern unsigned short* currentCollisionMap;

extern SPRITE player;
#define PLAYERVEL 1
#define PLAYERVERTVEL 2
#define PLAYEROAM 63
enum playerFrames {PLAYERIDLE, PLAYERJUMP, PLAYERWALK, PLAYERCLIMB};

extern SPRITE gun;
#define GUNOAM 62
#define GUNAFFINE 0

#define BOXCOUNT 4
extern BOXSPRITE moveableBlocks[BOXCOUNT];
extern BOXSPRITE* rowOrder[BOXCOUNT];
extern int boxes[BOXCOUNT];
extern int activeBoxes;
#define BOXOAM 4
#define BOXAFFINE 1

extern SPRITE door;
#define DOOROAM 0
extern SPRITE target;
#define TARGETOAM 1

extern SPRITE bullet;
#define BULLETSPEED 2
#define BULLETOAM 2

#define GRAVITY .125

void initGame();
void updateGame();

void initPlayer();
void updatePlayer();

void initGun();
void updateGun();

void initBoxes();
void updateBoxes(BOXSPRITE* sprite);
void fallBoxes(BOXSPRITE* sprite);

void initDoor();
void updateDoor();

void initTarget();
void updateTarget();

void clearPush();

void initBullet();
void updateBullet();
void fireBullet();

int compareFunc(const void * a, const void * b);
void setInterrupt();
void interruptHandle();

void pauseTimer();
void startTimer();

#endif