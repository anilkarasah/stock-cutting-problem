#ifndef ITEM_H
#define ITEM_H

#include <stdint.h>
#include "corner.h"
#include "types.h"

// dataset related functions
Data *initData(const char *filename);
void freeData(Data *data);

Result addCorner(Data *data, Corner corner);
Result checkCornerPositionAvailable(Data *data, Corner corner);
Result checkForCrashingItemInBetween(Data *data, uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY);

#endif