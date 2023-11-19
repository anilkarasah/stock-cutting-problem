#ifndef CORNERSLIST_H
#define CORNERSLIST_H

#include <stdint.h>

typedef struct
{
  uint8_t x;
  uint8_t y;
} Corner;

typedef struct
{
  Corner *corners;
  uint8_t size;
  uint8_t numberOfCorners;
} CornersList;

typedef enum {
  SUCCESS = 0,
  FAILURE_LIST_FULL = 1,
  FAILURE
} CornersListResult;

CornersList *createCornersList();
CornersListResult addCorner(CornersList *cornersList, Corner corner);
void freeCornersList(CornersList *cornersList);

#endif