#ifndef CORNER_H
#define CORNER_H

#include <stdint.h>
#include <stdbool.h>
#include "types.h"

CornersList *createCornersList(uint16_t size);
void freeCornersList(CornersList *cornersList);

Corner *initCorner(uint8_t x, uint8_t y, bool isUsed);
Corner setCornerValues(Corner corner, uint8_t x, uint8_t y, bool isUsed);
Result appendCornerToList(CornersList *cornersList, Corner *corner);

// helper functions
uint8_t findAvailableCorner(CornersList *cornersList);

#endif