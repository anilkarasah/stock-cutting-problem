#ifndef CORNER_H
#define CORNER_H

#include <stdint.h>

typedef struct
{
  uint8_t x;
  uint8_t y;
} Corner;

typedef struct
{
  Corner *corners;
  uint16_t size;
  uint16_t numberOfCorners;
} CornersList;

CornersList *createCornersList(uint16_t size);
void addCorner(CornersList *cornersList, Corner corner);
void freeCornersList(CornersList *cornersList);

#endif