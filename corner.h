#ifndef CORNER_H
#define CORNER_H

#include <stdint.h>
#include "types.h"

CornersList *createCornersList(uint16_t size);
void freeCornersList(CornersList *cornersList);

// helper functions
Corner findNextAvailableCorner(CornersList *cornersList);

#endif