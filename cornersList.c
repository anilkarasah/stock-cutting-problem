#include <stdio.h>
#include <stdlib.h>
#include "cornersList.h"

#define INITIAL_BUFFER_SIZE 16

CornersList *createCornersList()
{
  CornersList *cornersList = (CornersList *)malloc(sizeof(CornersList));
  cornersList->size = INITIAL_BUFFER_SIZE;
  cornersList->corners = (Corner *)malloc(sizeof(Corner) * cornersList->size);
  cornersList->numberOfCorners = 0;
  return cornersList;
}

CornersListResult addCorner(CornersList *cornersList, Corner corner)
{
  if (cornersList->numberOfCorners == cornersList->size)
  {
    return FAILURE_LIST_FULL;
  }

  cornersList->corners[cornersList->numberOfCorners] = corner;
  cornersList->numberOfCorners++;
  
  return SUCCESS;
}

void freeCornersList(CornersList *cornersList)
{
  free(cornersList->corners);
  free(cornersList);
}


