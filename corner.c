#include <stdio.h>
#include <stdlib.h>
#include "corner.h"

CornersList *createCornersList(uint16_t size)
{
  CornersList *cornersList = (CornersList *)malloc(sizeof(CornersList));
  cornersList->size = size;
  cornersList->corners = (Corner *)malloc(sizeof(Corner) * cornersList->size);
  cornersList->numberOfCorners = 0;
  return cornersList;
}

void addCorner(CornersList *cornersList, Corner corner)
{
  if (cornersList->numberOfCorners == cornersList->size)
  {
    return;
  }

  cornersList->corners[cornersList->numberOfCorners] = corner;
  cornersList->numberOfCorners++;
}

void freeCornersList(CornersList *cornersList)
{
  free(cornersList->corners);
  free(cornersList);
}


