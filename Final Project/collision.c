#include "mode0.h"
#include "game.h"
#include "gameLevel0Collision.h"
#include <stdio.h>
#include <math.h>
#include "collision.h"

int collisionBulletCheck(int start, int speedProportion) {
    int x = bullet.worldCol;
    int y = bullet.worldRow;
    int xDelta = ((bullet.cdel + bullet.worldCol) >> 8) - (bullet.worldCol >> 8);
    int yDelta = ((bullet.rdel + bullet.worldRow) >> 8) - (bullet.worldRow >> 8);
    //Starting facing right and checking counter clockwise
    //mgba_printf("xDelta: %d, yDelta: %d, proportion: %d", xDelta, yDelta, speedProportion);
    if (xDelta > 0 && yDelta == 0) { //Right
        for (int i = start; i < xDelta; i++) {
            if (collision((x >> 8) + bullet.width + i, (y >> 8), 1, bullet.height, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8) + bullet.width + i, (y >> 8), 1, bullet.height, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                return 1;
            }
            for (int j = 0; j < bullet.height; j++) {
                //mgba_printf("collisionmap: %d", ((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width + i, (y >> 8) + j, 256)]);
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width + i, (y >> 8) + j, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width + i, (y >> 8) + j, 256)] == 2) {
                    int tempSpeed = (((i - start) * speedProportion) / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.cdel = -bullet.cdel;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start + 1) * speedProportion) / BULLETSPEED));
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width + i, (y >> 8) + j, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + bullet.width + i, (y >> 8) + j);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8) + bullet.width + i, (y >> 8), bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    int tempSpeed = (((i - start) * speedProportion) / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.cdel = -bullet.cdel;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start + 1) * speedProportion) / BULLETSPEED));
                }
            }
        }
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        return 0;
    } else if (xDelta > 0 && yDelta > 0) { //Right and down
        //int whole = ((yDelta / xDelta) << 8) + ((yDelta % xDelta) << 7);
        int step;
        if (abs(xDelta) >= abs(yDelta)) {
            step = abs(xDelta);
        } else {
            step = abs(yDelta);
        }
        int deltaFrac = 1;
        while (((xDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        xDelta = ((xDelta / step) << 8) + ((xDelta % step) << (8 - deltaFrac));
        deltaFrac = 1;
        while (((yDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        yDelta = ((yDelta / step) << 8) + ((yDelta % step) << (8 - deltaFrac));
        //mgba_printf("xDelta: %d, yDelta: %d", xDelta, yDelta);
        for (int i = start; i < step; i++) {
            int switched = 0;
            if (collision((x >> 8)  + bullet.width, (y >> 8), 1, bullet.height, door.worldCol, door.worldRow, door.width, door.height)
                || collision((x >> 8), (y >> 8) + bullet.height, bullet.width, 1, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8)  + bullet.width, (y >> 8), 1, bullet.height, target.worldCol, target.worldRow, target.width, target.height)
                || collision((x >> 8), (y >> 8) + bullet.height, bullet.width, 1, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                return 1;
            }
            for (int j = 0; j < bullet.height; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width, (y >> 8) + j, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width, (y >> 8) + j, 256)] == 2) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width, (y >> 8) + j, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + bullet.width, (y >> 8) + j);
                    return 1;
                }
            }
            for (int j = 0; j < bullet.width; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height, 256)] == 2) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + j, (y >> 8) + bullet.height);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8) + bullet.width, (y >> 8), bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (collision((x >> 8), (y >> 8) + bullet.height, bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
            }
            if (switched) {
                int tempSpeed = ((i - start) * speedProportion / BULLETSPEED);
                switch (switched) {
                    case 1:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                        break;
                    case 2:
                        bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                    case 3:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                }
                return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
            }
            x += xDelta;
            y += yDelta;
        }
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        return 0;
    } else if (xDelta == 0 && yDelta > 0) { //Down
        for (int i = start; i < yDelta; i++) {
            if (collision((x >> 8), (y >> 8) + bullet.height + i, bullet.width, 1, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8), (y >> 8) + bullet.height + i, bullet.width, 1, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                return 1;
            }
            for (int j = 0; j < bullet.width; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height + i, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height + i, 256)] == 2) {
                    int tempSpeed = ((i - start) * speedProportion / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.rdel = -bullet.rdel;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height + i, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + j, (y >> 8) + bullet.height + i);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8), (y >> 8) + bullet.height + i, bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    int tempSpeed = (((i - start) * speedProportion) / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.rdel = -bullet.rdel;;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
                }
            }
        }
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        return 0;
    } else if (xDelta < 0 && yDelta > 0) { //Left and down
        int step;
        if (abs(xDelta) >= abs(yDelta)) {
            step = abs(xDelta);
        } else {
            step = abs(yDelta);
        }
        int deltaFrac = 1;
        while (((-xDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        xDelta = ((xDelta / step) << 8) + -((-xDelta % step) << (8 - deltaFrac));
        deltaFrac = 1;
        while (((yDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        yDelta = ((yDelta / step) << 8) + ((yDelta % step) << (8 - deltaFrac));
        //mgba_printf("xDelta: %d, yDelta: %d", xDelta, yDelta);
        for (int i = start; i < step; i++) {
            int switched = 0;
            if (collision((x >> 8) - 1, (y >> 8), 1, bullet.height, door.worldCol, door.worldRow, door.width, door.height)
                || collision((x >> 8), (y >> 8) + bullet.height, bullet.width, 1, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8) - 1, (y >> 8), 1, bullet.height, target.worldCol, target.worldRow, target.width, target.height)
                || collision((x >> 8), (y >> 8) + bullet.height, bullet.width, 1, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                return 1;
            }
            for (int j = 0; j < bullet.height; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - 1, (y >> 8) + j, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - 1, (y >> 8) + j, 256)] == 2) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - 1, (y >> 8) + j, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) - 1, (y >> 8) + j);
                    return 1;
                }
            }
            for (int j = 0; j < bullet.width; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height, 256)] == 2) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) + bullet.height, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + j, (y >> 8) + bullet.height);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8) - 1, (y >> 8), bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (collision((x >> 8), (y >> 8) + bullet.height, bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
            }
            if (switched) {
                int tempSpeed = ((i - start) * speedProportion / BULLETSPEED);
                switch (switched) {
                    case 1:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                        break;
                    case 2:
                        bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                    case 3:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                }
                return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
            }
            x += xDelta;
            y += yDelta;
        }
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        return 0;
    } else if (xDelta < 0 && yDelta == 0) { //Left
        for (int i = start; i < abs(xDelta); i++) {
            if (collision((x >> 8) - i - 1, (y >> 8), 1, bullet.height, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8) - i - 1, (y >> 8), 1, bullet.height, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                return 1;
            }
            for (int j = 0; j < bullet.height; j++) {
                //mgba_printf("collisionmap: %d", ((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width + i, (y >> 8) + j, 256)]);
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - i - 1, (y >> 8) + j, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - i - 1, (y >> 8) + j, 256)] == 2) {
                    int tempSpeed = ((i - start) * speedProportion / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.cdel = -bullet.cdel;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - i - 1, (y >> 8) + j, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) - i - 1, (y >> 8) + j);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8) - i - 1, (y >> 8), bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    int tempSpeed = (((i - start) * speedProportion) / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.cdel = -bullet.cdel;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
                }
            }
        }
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        return 0;
    } else if (xDelta < 0 && yDelta < 0) { //Left and up
        int step;
        if (abs(xDelta) >= abs(yDelta)) {
            step = abs(xDelta);
        } else {
            step = abs(yDelta);
        }
        int deltaFrac = 1;
        while (((-xDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        xDelta = ((xDelta / step) << 8) + -((-xDelta % step) << (8 - deltaFrac));
        deltaFrac = 1;
        while (((-yDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        yDelta = ((yDelta / step) << 8) + -((-yDelta % step) << (8 - deltaFrac));
        //mgba_printf("xDelta: %d, yDelta: %d", xDelta, yDelta);
        for (int i = start; i < step; i++) {
            int switched = 0;
            if (collision((x >> 8) - 1, (y >> 8), 1, bullet.height, door.worldCol, door.worldRow, door.width, door.height)
                || collision((x >> 8), (y >> 8) - 1, bullet.width, 1, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8) - 1, (y >> 8), 1, bullet.height, target.worldCol, target.worldRow, target.width, target.height)
                || collision((x >> 8), (y >> 8) - 1, bullet.width, 1, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                return 1;
            }
            for (int j = 0; j < bullet.height; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - 1, (y >> 8) + j, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - 1, (y >> 8) + j, 256)] == 2) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) - 1, (y >> 8) + j, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) - 1, (y >> 8) + j);
                    return 1;
                }
            }
            for (int j = 0; j < bullet.width; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - 1, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - 1, 256)] == 2) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - 1, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + j, (y >> 8) - 1);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8) - 1, (y >> 8), bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (collision((x >> 8), (y >> 8) - 1, bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
            }
            if (switched) {
                int tempSpeed = ((i - start) * speedProportion / BULLETSPEED);
                switch (switched) {
                    case 1:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                        break;
                    case 2:
                        bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                    case 3:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                }
                return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
            }
            x += xDelta;
            y += yDelta;
        }
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        return 0;
    } else if (xDelta == 0 && yDelta < 0) { //Up
        for (int i = start; i < abs(yDelta); i++) {
            if (collision((x >> 8), (y >> 8) - i - 1, bullet.width, 1, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8), (y >> 8) - i - 1, bullet.width, 1, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                mgba_printf("CollisionX: %d, CollisionY: %d", (x >> 8), (y >> 8) - i - 1);
                return 1;
            }
            for (int j = 0; j < bullet.width; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - i - 1, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - i - 1, 256)] == 2) {
                    int tempSpeed = ((i - start) * speedProportion / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.rdel = -bullet.rdel;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - i - 1, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + j, (y >> 8) - i - 1);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8), (y >> 8) - i - 1, bullet.width, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    int tempSpeed = (((i - start) * speedProportion) / BULLETSPEED);
                    bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                    bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                    bullet.rdel = -bullet.rdel;;
                    return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
                }
            }
        }
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        return 0;
    } else { // bullet.cdel > 0 && bullet.rdel < 0 Right and up
        int step;
        if (abs(xDelta) >= abs(yDelta)) {
            step = abs(xDelta);
        } else {
            step = abs(yDelta);
        }
        int deltaFrac = 1;
        while (((xDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        xDelta = ((xDelta / step) << 8) + ((xDelta % step) << (8 - deltaFrac));
        deltaFrac = 1;
        while (((-yDelta % step) >> deltaFrac) != 0) {
            deltaFrac++;
        }
        yDelta = ((yDelta / step) << 8) + -((-yDelta % step) << (8 - deltaFrac));
        //mgba_printf("xDelta: %d, yDelta: %d", xDelta, yDelta);
        for (int i = start; i < step; i++) {
            int switched = 0;
            if (collision((x >> 8) + bullet.width, (y >> 8), 1, bullet.height, door.worldCol, door.worldRow, door.width, door.height)
                || collision((x >> 8), (y >> 8) - 1, bullet.width, 1, door.worldCol, door.worldRow, door.width, door.height)) {
                bullet.hide = 1;
                return 1;
            }
            if (collision((x >> 8) + bullet.width, (y >> 8), 1, bullet.height, target.worldCol, target.worldRow, target.width, target.height)
                || collision((x >> 8), (y >> 8) - 1, bullet.width, 1, target.worldCol, target.worldRow, target.width, target.height)) {
                if (!target.flip) {
                    target.flip = 1;
                    door.flip++;
                }
                bullet.hide = 1;
                return 1;
            }
            for (int j = 0; j < bullet.height; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width, (y >> 8) + j, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width, (y >> 8) + j, 256)] == 2) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + bullet.width, (y >> 8) + j, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + bullet.width, (y >> 8) + j);
                    return 1;
                }
            }
            for (int j = 0; j < bullet.width; j++) {
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - 1, 256)] == 0) {
                    bullet.hide = 1;
                    return 1;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - 1, 256)] == 2) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET((x >> 8) + j, (y >> 8) - 1, 256)] == 4) {
                    bullet.hide = 1;
                    removeTileMap((x >> 8) + j, (y >> 8) - 1);
                    return 1;
                }
            }
            for (int j = 0; j < activeBoxes; j++) {
                if (collision((x >> 8) + bullet.width, (y >> 8), 1, bullet.height, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.cdel = -bullet.cdel;
                    switched += 1;
                    break;
                }
                if (collision((x >> 8), (y >> 8) - 1, bullet.width, 1, moveableBlocks[j].worldCol, moveableBlocks[j].worldRow, moveableBlocks[j].width, moveableBlocks[j].height)) {
                    bullet.rdel = -bullet.rdel;
                    switched += 2;
                    break;
                }
            }
            if (switched) {
                int tempSpeed = ((i - start) * speedProportion / BULLETSPEED);
                switch (switched) {
                    case 1:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (bullet.rdel * tempSpeed) / 256;
                        break;
                    case 2:
                        bullet.worldCol += (bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                    case 3:
                        bullet.worldCol += (-bullet.cdel * tempSpeed) / 256;
                        bullet.worldRow += (-bullet.rdel * tempSpeed) / 256;
                        break;
                }
                return collisionBulletCheck(i + 1, speedProportion - (((i - start) * speedProportion) / BULLETSPEED));
            }
            x += xDelta;
            y += yDelta;
        }
        bullet.worldCol += (bullet.cdel * speedProportion) / 256;
        bullet.worldRow += (bullet.rdel * speedProportion) / 256;
        return 0;
    }
    return 0;
}

void removeTileMap(int x, int y) {
    int tileX = x / 8;
    int tileY = y / 8;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            //mgba_printf("PixelX: %d, PixelY: %d", (tileX * 8) + i, (tileY * 8) + j);
            ((unsigned char *)currentCollisionMap)[OFFSET((tileX * 8) + i, (tileY * 8) + j, 256)] = 1;
            //mgba_printf("PixelX: %d, PixelY: %d", (tileX * 8) + i, (tileY * 8) + j);
        }
    }
    SCREENBLOCK[26].tilemap[OFFSET(tileX, tileY, 32)] = 0;
}

int checkBulletSpawn(int x, int y) {
    int mapCollide = ((unsigned char *)currentCollisionMap)[OFFSET(x, y, 256)];
    if (mapCollide == 0 || mapCollide == 2 || mapCollide == 4 || mapCollide == 5) {
        return 1;
    }
    for (int i = 0; i < activeBoxes; i++) {
        if (collision(x, y, 1, 1, moveableBlocks[i].worldCol, moveableBlocks[i].worldRow, moveableBlocks[i].width, moveableBlocks[i].height)) {
            return 1;
        }
    }
    if (collision(x, y, 1, 1, target.worldCol, target.worldRow, target.width, target.height)) {
        return 1;
    }
    if (collision(x, y, 1, 1, door.worldCol, door.worldRow, door.width, door.height)) {
        return 1;
    }
    return 0;
}

int collisionCheckHor(SPRITE sprite) {
    int check = -1;
    if (sprite.cdel > 0) {
        for (int j = 0; j < sprite.cdel; j++) {
            for (int i = 0; i < sprite.height; i++) {
                //mgba_printf("%d", ((unsigned char *)currentCollisionMap)[OFFSET(sprite.col + sprite.width + j, sprite.row + i, 256)]);
                if (sprite.worldCol + sprite.width + j < 240
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + sprite.width + j, sprite.worldRow + i, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + sprite.width + j, sprite.worldRow + i, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + sprite.width + j, sprite.worldRow + i, 256)] == 4)
                && (check == -1 || j <= check)) {
                    check = j;
                    if (check == 0) {
                        return 0;
                    }
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + sprite.width + j, sprite.worldRow + i, 256)] == 3) {
                    winInt = 1;
                    return 0;
                }
            }
            for (int i = 0; i < activeBoxes; i++) {
                if (moveableBlocks[i].isPushed) {
                    continue;
                }
                if (collision(sprite.worldCol + sprite.width + j + 1, sprite.worldRow, 1, sprite.height, moveableBlocks[i].worldCol, moveableBlocks[i].worldRow, moveableBlocks[i].width, moveableBlocks[i].height)) {
                    int move = collisionBoxCheckHor(&moveableBlocks[i], sprite.cdel - j);
                    if (j == 0 && move == 0) {
                        return 0;
                    }
                }
            }
            if (collision(sprite.worldCol + sprite.width + j + 1, sprite.worldRow, 1, sprite.height, door.worldCol, door.worldRow, door.width, door.height)
                && (check == -1 || j <= check)) {
                    check = j;
                    if (check == 0) {
                        return 0;
                    }
            }
        }
        return -check;
    } else if (sprite.cdel < 0) {
        for (int j = 1; j < -sprite.cdel + 1; j++) {
            for (int i = 0; i < sprite.height; i++) {
                if (sprite.worldCol - j >= 0 
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol - j, sprite.worldRow + i, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol - j, sprite.worldRow + i, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol - j, sprite.worldRow + i, 256)] == 4)
                && (check == -1 || j - 1 <= check)) {
                    check = j - 1;
                    if (check == 0) {
                        return 0;
                    }
                }
                if (((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + sprite.width + j, sprite.worldRow + i, 256)] == 3) {
                    winInt = 1;
                    return 0;
                }
            }
            for (int i = 0; i < activeBoxes; i++) {
                if (moveableBlocks[i].isPushed) {
                    continue;
                }
                if (collision(sprite.worldCol - j - 1, sprite.worldRow, 1, sprite.height, moveableBlocks[i].worldCol, moveableBlocks[i].worldRow, moveableBlocks[i].width, moveableBlocks[i].height)) {
                    int move = collisionBoxCheckHor(&moveableBlocks[i], sprite.cdel - j + 1);
                    if (j == 1 && move == 0) {
                        return 0;
                    }
                }
            }
            if (collision(sprite.worldCol - j - 1, sprite.worldRow, 1, sprite.height, door.worldCol, door.worldRow, door.width, door.height)
                && (check == -1 || j <= check)) {
                    check = j;
                    if (check == 0) {
                        return 0;
                    }
            }
        }
        return check;
    }
    return sprite.cdel;
}

int collisionBoxCheckHor(BOXSPRITE* sprite, int speed) {
    //mgba_printf("You should print twice");
    sprite->isPushed = 1;
    sprite->cdel = speed;
    int check = -1;
    if (speed > 0) {
        for (int j = 0; j < speed; j++) {
            for (int i = 0; i < sprite->height; i++) {
                //mgba_printf("%d", ((unsigned char *)currentCollisionMap)[OFFSET(sprite->col + sprite->width + j, sprite->row + i, 256)]);
                if (sprite->worldCol + sprite->width + j < 240
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + sprite->width + j, sprite->worldRow + i, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + sprite->width + j, sprite->worldRow + i, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + sprite->width + j, sprite->worldRow + i, 256)] == 4)
                && (check == -1 || j <= check)) {
                    check = j;
                    if (check == 0) {
                        sprite->cdel = check;
                        return 0;
                    }
                }
            }
            if (collision(sprite->worldCol + sprite->width + j + 1, sprite->worldRow, 1, sprite->height, door.worldCol, door.worldRow, door.width, door.height)
                && (check == -1 || j <= check)) {
                    check = j;
                    if (check == 0) {
                        sprite->cdel = check;
                        return 0;
                    }
            }
            for (int i = 0; i < activeBoxes; i++) {
                if (i == sprite->number || moveableBlocks[i].isPushed) {
                    continue;
                }
                if (collision(sprite->worldCol + sprite->width + j + 1, sprite->worldRow, 1, sprite->height, moveableBlocks[i].worldCol, moveableBlocks[i].worldRow, moveableBlocks[i].width, moveableBlocks[i].height)) {
                    int move = collisionBoxCheckHor(&moveableBlocks[i], sprite->cdel - j);
                    if (j == 0 && move == 0) {
                        sprite->cdel = move;
                        return 0;
                    }
                }
            }
        }
        sprite->cdel = -check;
        return -check;
    } else if (speed < 0) {
        for (int j = 1; j < -speed + 1; j++) {
            for (int i = 0; i < sprite->height; i++) {
                if (sprite->worldCol - j >= 0 
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol - j, sprite->worldRow + i, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol - j, sprite->worldRow + i, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol - j, sprite->worldRow + i, 256)] == 4)
                && (check == -1 || j - 1 <= check)) {
                    check = j - 1;
                    if (check == 0) {
                        sprite->cdel = check;
                        return 0;
                    }
                }
            }
            if (collision(sprite->worldCol - j - 1, sprite->worldRow, 1, sprite->height, door.worldCol, door.worldRow, door.width, door.height)
                && (check == -1 || j <= check)) {
                    check = j;
                    if (check == 0) {
                        sprite->cdel = check;
                        return 0;
                    }
            }
            for (int i = 0; i < activeBoxes; i++) {
                if (i == sprite->number || moveableBlocks[i].isPushed) {
                    continue;
                }
                if (collision(sprite->worldCol - j - 1, sprite->worldRow, 1, sprite->height, moveableBlocks[i].worldCol, moveableBlocks[i].worldRow, moveableBlocks[i].width, moveableBlocks[i].height)) {
                    int move = collisionBoxCheckHor(&moveableBlocks[i], sprite->cdel - j + 1);
                    if (j == 1 && move == 0) {
                        sprite->cdel = move;
                        return 0;
                    }
                }
            }
        }
        sprite->cdel = check;
        return check;
    }
    return 0;
}

int collisionCheckVer(SPRITE sprite) {
    int check = -1;
    
    if (sprite.rdel < 0) {
        for (int i = 0; i < sprite.width; i++) {
            for (int j = 0; j < -(sprite.rdel); j++) {
                if (sprite.worldRow - j >= 0 
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + i, sprite.worldRow + j + 1, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + i, sprite.worldRow + j + 1, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + i, sprite.worldRow + j + 1, 256)] == 4)
                && (check == -1 || j <= check)) {
                    check = j;
                    //mgba_printf("Less than 0 check: %d", check);
                    if (check == 0) {
                        return 8;
                    }
                }
            }
        }
        if (check == -1) {
            return sprite.rdel;
        }
        return -check;
    }
    if (sprite.rdel >= 0) {
        if (sprite.rdel == 0) {
            sprite.rdel = 1;
        }
        //mgba_printf("Does this even work");
        for (int j = 0; j < sprite.rdel; j++) {
            for (int i = 0; i < sprite.width; i++) {
                //mgba_printf("Rdel: %d", sprite.rdel);
                if (sprite.worldRow + sprite.height + j < SCREENHEIGHT
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + i, sprite.worldRow + sprite.height + j, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + i, sprite.worldRow + sprite.height + j, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite.worldCol + i, sprite.worldRow + sprite.height + j, 256)] == 4)) {
                    if ((check == -1 || j <= check)) {
                        check = j;
                        if (check == 0) {
                            return 0;
                        }
                    }
                    break;
                    //mgba_printf("Check: %d", check);
                }
            }
            for (int i = 0; i < activeBoxes; i++) {
                if (collision(sprite.worldCol, sprite.worldRow + sprite.height + 1, sprite.width, 1, moveableBlocks[i].worldCol, moveableBlocks[i].worldRow, moveableBlocks[i].width, moveableBlocks[i].height)) {
                    if ((check == -1 || j <= check)) {
                        check = j;
                        if (check == 0) {
                            return 0;
                        }
                    }
                    break;
                    //mgba_printf("Check: %d", check);
                }
            }
        }
        //mgba_printf("Rdel: %d, Check: %d", sprite.rdel, check);
        if (check == -1) {
            return sprite.rdel;
        }
        if (sprite.rdel >> 3 > check) {
            return (check << 3);
        }
    }
    return sprite.rdel;
}

int collisionBoxCheckVer(BOXSPRITE* sprite, int speed) {
    int check = -1;
    sprite->rdel = speed;
    
    if (speed < 0) {
        for (int i = 0; i < sprite->width; i++) {
            for (int j = 0; j < -(speed); j++) {
                if (sprite->worldRow - j >= 0 
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + i, sprite->worldRow + j + 1, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + i, sprite->worldRow + j + 1, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + i, sprite->worldRow + j + 1, 256)] == 4)
                && (check == -1 || j <= check)) {
                    check = j;
                    //mgba_printf("Less than 0 check: %d", check);
                    if (check == 0) {
                        sprite->rdel = 8;
                        return 8;
                    }
                }
            }
        }
        if (check == -1) {
            return sprite->rdel;
        }
        return -check;
    }
    if (speed >= 0) {
        int xDelta = (sprite->worldRow + (sprite->rdel >> 3)) - (sprite->worldRow);
        if (xDelta == 0) {
            return sprite->rdel;
        }
        //mgba_printf("Does this even work");
        for (int j = 0; j < (speed >> 3); j++) {
            for (int i = 0; i < sprite->width; i++) {
                //mgba_printf("Rdel: %d", sprite->rdel);
                if (sprite->worldRow + sprite->height + j < SCREENHEIGHT
                && (((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + i, sprite->worldRow + sprite->height + j, 256)] == 0 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + i, sprite->worldRow + sprite->height + j, 256)] == 2 ||
                    ((unsigned char *)currentCollisionMap)[OFFSET(sprite->worldCol + i, sprite->worldRow + sprite->height + j, 256)] == 4)) {
                    if ((check == -1 || j <= check)) {
                        check = j;
                        if (check == 0) {
                            sprite->rdel = 0;
                            return 0;
                        }
                    }
                    break;
                    //mgba_printf("Check: %d", check);
                }
            }
            for (int i = 0; i < activeBoxes; i++) {
                if (i == sprite->number || moveableBlocks[i].worldRow + sprite->height <= sprite->worldRow) {
                    continue;
                }
                if (collision(sprite->worldCol, sprite->worldRow + sprite->height + j + 1, sprite->width, 1, moveableBlocks[i].worldCol, moveableBlocks[i].worldRow, moveableBlocks[i].width, moveableBlocks[i].height)) {
                    sprite->rdel = (j << 3);
                    return 1;
                    //mgba_printf("Check: %d", check);
                }
            }
        }
        //mgba_printf("Rdel: %d, Check: %d", sprite->rdel, check);
        if (check == -1) {
            return sprite->rdel;
        }
        if (sprite->rdel >> 3 > check) {
            sprite->rdel = check << 3;
            return (check << 3);
        }
    }
    return sprite->rdel;
}