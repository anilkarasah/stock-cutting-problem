#include <stdlib.h>
#include "graph.h"
#include "corner.h"

uint16_t calculateDistance(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY)
{
  uint8_t distanceX = fromX - toX;
  uint8_t distanceY = fromY - toY;

  return distanceX * distanceX + distanceY * distanceY;
}

int compareCorner(const void *corner1, const void *corner2)
{
  return ((Corner *)corner1)->hash - ((Corner *)corner2)->hash;
}

CornersList *calculateUniqueCorners(EdgeList *edgeList, int rollWidth)
{
  Corner *uniqueCorners = (Corner *)calloc(edgeList->numEdges * 2, sizeof(Corner));
  int uniqueCornersIndex = 0;

  for (int i = 0; i < edgeList->numEdges; i++)
  {
    Edge edge = edgeList->edgeList[i];

    bool fromCornerExists = false;
    bool toCornerExists = false;

    for (int k = 0; k < uniqueCornersIndex; k++)
    {
      if (compareCorners(edge.fromCorner, uniqueCorners[k]))
        fromCornerExists = true;

      if (compareCorners(edge.toCorner, uniqueCorners[k]))
        toCornerExists = true;
    }

    if (!fromCornerExists)
    {
      setCornerValues(&(uniqueCorners[uniqueCornersIndex]), edge.fromCorner.x, edge.fromCorner.y, false, rollWidth);
      uniqueCornersIndex++;
    }

    if (!toCornerExists)
    {
      setCornerValues(&(uniqueCorners[uniqueCornersIndex]), edge.toCorner.x, edge.toCorner.y, false, rollWidth);
      uniqueCornersIndex++;
    }
  }

  qsort(uniqueCorners, uniqueCornersIndex, sizeof(Corner), compareCorner);

  CornersList *cornersList = (CornersList *)malloc(sizeof(CornersList));
  cornersList->numCorners = uniqueCornersIndex;
  cornersList->corners = (Corner **)malloc(uniqueCornersIndex * sizeof(Corner *));
  for (int i = 0; i < cornersList->numCorners; i++)
  {
    cornersList->corners[i] = (Corner *)calloc(1, sizeof(Corner));
    cornersList->corners[i]->x = uniqueCorners[i].x;
    cornersList->corners[i]->y = uniqueCorners[i].y;
    cornersList->corners[i]->isUsed = false;
    cornersList->corners[i]->hash = getCornerHash(*cornersList->corners[i], rollWidth);
  }

  return cornersList;
}

int searchCorner(CornersList *cornersList, Corner corner)
{
  // binary search
  int left = 0;
  int right = cornersList->numCorners - 1;

  while (left <= right)
  {
    int mid = (left + right) / 2;

    if (corner.hash == cornersList->corners[mid]->hash)
      return mid;

    if (corner.hash < cornersList->corners[mid]->hash)
      right = mid - 1;
    else
      left = mid + 1;
  }

  return -1;
}

uint8_t **generateAdjacencyMatrix(CornersList *cornersList, EdgeList *edgeList, int rollWidth)
{
  uint8_t **adjacencyMatrix = (uint8_t **)calloc(cornersList->numCorners, sizeof(uint8_t *));
  for (int i = 0; i < cornersList->numCorners; i++)
  {
    adjacencyMatrix[i] = (uint8_t *)calloc(cornersList->numCorners, sizeof(uint8_t));
  }

  for (int i = 0; i < edgeList->numEdges; i++)
  {
    Edge edge = edgeList->edgeList[i];

    edge.fromCorner.hash = getCornerHash(edge.fromCorner, rollWidth);
    edge.toCorner.hash = getCornerHash(edge.toCorner, rollWidth);

    int fromCornerIndex = searchCorner(cornersList, edge.fromCorner);
    int toCornerIndex = searchCorner(cornersList, edge.toCorner);

    adjacencyMatrix[fromCornerIndex][toCornerIndex] = 1;
    adjacencyMatrix[toCornerIndex][fromCornerIndex] = 1;
  }

  return adjacencyMatrix;
}
