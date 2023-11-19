#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "corner.h"

CornersList *createCornersList(uint16_t size)
{
  CornersList *cornersList = (CornersList *)malloc(sizeof(CornersList));
  cornersList->size = size;
  cornersList->corners = (Corner *)malloc(sizeof(Corner) * cornersList->size);

  // initialize the first corner at (0, 0)
  cornersList->corners[0].x = 0;
  cornersList->corners[0].y = 0;
  cornersList->numberOfCorners = 1;

  return cornersList;
}

void freeCornersList(CornersList *cornersList)
{
  free(cornersList->corners);
  free(cornersList);
}

Corner findNextAvailableCorner(CornersList *cornersList)
{
  uint8_t minIndex = 0;
  Corner minCorner = {UINT8_MAX, UINT8_MAX};

  for (int i = 0; i < cornersList->numberOfCorners; i++)
  {
    if (cornersList->corners[i].x < minCorner.x)
    {
      // find the corner with the smallest x-axis
      minCorner = cornersList->corners[i];
      minIndex = i;
    }
    else if (cornersList->corners[i].x == minCorner.x && cornersList->corners[i].y < minCorner.y)
    {
      // if there are multiple corners with the same x-axis, find the corner with the smallest y-axis
      minCorner = cornersList->corners[i];
      minIndex = i;
    }
  }
}
