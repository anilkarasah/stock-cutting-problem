#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "corner.h"

CornersList *createCornersList(uint16_t size)
{
  CornersList *cornersList = (CornersList *)malloc(sizeof(CornersList));
  if (cornersList == NULL)
  {
    fprintf(stderr, "Error allocating memory for corners list\n");
    exit(-1);
  }

  cornersList->size = size;
  cornersList->corners = (Corner **)malloc(cornersList->size * sizeof(Corner *));
  if (cornersList->corners == NULL)
  {
    fprintf(stderr, "Error allocating memory for cornersList->corners\n");
    exit(-1);
  }

  // initialize the first corner at (0, 0)
  cornersList->corners[0] = initCorner(0, 0, false);
  cornersList->numCorners = 1;

  return cornersList;
}

void freeCornersList(CornersList *cornersList)
{
  free(cornersList->corners);
  free(cornersList);
}

Corner setCornerValues(Corner corner, uint8_t x, uint8_t y, bool isUsed)
{
  corner.x = x;
  corner.y = y;
  corner.isUsed = isUsed;

  return corner;
}

Corner *initCorner(uint8_t x, uint8_t y, bool isUsed)
{
  Corner *corner = (Corner *)malloc(sizeof(Corner));
  if (corner == NULL)
  {
    fprintf(stderr, "Error allocating memory for corner\n");
    exit(-1);
  }

  corner->x = x;
  corner->y = y;
  corner->isUsed = isUsed;

  return corner;
}

Result appendCornerToList(CornersList *cornersList, Corner *corner)
{
  if (cornersList->numCorners >= cornersList->size)
  {
    // corners list is full
    return FAILURE;
  }

  cornersList->corners[cornersList->numCorners++] = corner;

  return SUCCESS;
}

uint8_t findAvailableCorner(CornersList *cornersList)
{
  uint8_t minIndex = UINT8_MAX;
  Corner minCorner;

  minCorner = setCornerValues(minCorner, UINT8_MAX, UINT8_MAX, false);

  for (int i = 0; i < cornersList->numCorners; i++)
  {
    if (cornersList->corners[i]->isUsed)
      continue;

    if (cornersList->corners[i]->y < minCorner.y)
    {
      // find the corner with the smallest x-axis
      minCorner = *(cornersList->corners[i]);
      minIndex = i;
    }
    else if (cornersList->corners[i]->y == minCorner.y && cornersList->corners[i]->x < minCorner.x)
    {
      // if there are multiple corners with the same x-axis, find the corner with the smallest y-axis
      minCorner = *(cornersList->corners[i]);
      minIndex = i;
    }
  }

  return minIndex;
}
