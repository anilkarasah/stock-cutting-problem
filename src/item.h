#ifndef ITEM_H
#define ITEM_H

#include <stdint.h>
#include "corner.h"
#include "types.h"

// dataset related functions
Data *initData(const char *filename);
void freeData(Data *data);

Result addCorner(Data *data, Corner *corner);
Result checkCornerPositionAvailable(Data *data, Corner corner);
Result checkForCrashingItemInBetween(Data *data, Corner fromCorner, Corner toCorner);
void placeItemToTheCorner(Data *data, Item item, Corner corner);

#endif