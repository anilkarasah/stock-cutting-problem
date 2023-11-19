#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "corner.h"

CornersList *createCornersList(uint16_t size)
{
  CornersList *cornersList = (CornersList *)malloc(sizeof(CornersList));
  cornersList->size = size;
  cornersList->corners = (Corner **)calloc(cornersList->size, sizeof(Corner *));

  // initialize the first corner at (0, 0)
  cornersList->corners[0] = initCorner(0, 0, false);

  return cornersList;
}

void freeCornersList(CornersList *cornersList)
{
  free(cornersList->corners);
  free(cornersList);
}

Corner *initCorner(uint8_t x, uint8_t y, bool isUsed)
{
  Corner *corner = (Corner *)malloc(sizeof(Corner));
  corner->x = x;
  corner->y = y;
  corner->isUsed = isUsed;

  return corner;
}

Result appendCornerToList(CornersList *cornersList, Corner *corner)
{
  int i = 0;
  while (i < cornersList->size && cornersList->corners[i] != NULL)
    i++;

  if (i == cornersList->size)
    return FAILURE;

  cornersList->corners[i] = corner;
  return SUCCESS;
}

uint8_t findAvailableCorner(CornersList *cornersList)
{
  uint8_t minIndex = 0;
  Corner *minCorner = initCorner(UINT8_MAX, UINT8_MAX, false);

  for (int i = 0; i < cornersList->size; i++)
  {
    if (cornersList->corners[i]->isUsed)
      continue;

    if (cornersList->corners[i]->x < minCorner->x)
    {
      // find the corner with the smallest x-axis
      minCorner = cornersList->corners[i];
      minIndex = i;
    }
    else if (cornersList->corners[i]->x == minCorner->x && cornersList->corners[i]->y < minCorner->y)
    {
      // if there are multiple corners with the same x-axis, find the corner with the smallest y-axis
      minCorner = cornersList->corners[i];
      minIndex = i;
    }
  }

  return minIndex;
}
