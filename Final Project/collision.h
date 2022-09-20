#ifndef COLLISION_H
#define COLLISION_H

int collisionBulletCheck(int, int);
int checkBulletSpawn(int, int);

void removeTileMap(int, int);

int collisionCheckHor(SPRITE sprite);
int collisionCheckVer(SPRITE sprite);

int collisionBoxCheckHor(BOXSPRITE* sprite, int speed);
int collisionBoxCheckVer(BOXSPRITE* sprite, int speed);

#endif