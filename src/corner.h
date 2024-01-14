#ifndef CORNER_H
#define CORNER_H

#include <stdint.h>
#include <stdbool.h>
#include "types.h"

CornersList *createCornersList(uint16_t size);
void freeCornersList(CornersList *cornersList);

Corner *initCorner(uint8_t x, uint8_t y, bool isUsed, int rollWidth);
void setCornerValues(Corner *corner, uint8_t x, uint8_t y, bool isUsed, int rollWidth);
Result appendCornerToList(CornersList *cornersList, Corner *corner);

// helper functions
uint8_t findAvailableCorner(CornersList *cornersList);
bool compareCorners(Corner corner1, Corner corner2);
uint16_t getCornerHash(Corner corner, int rollWidth);

#endif