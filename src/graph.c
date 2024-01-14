#include <stdlib.h>
#include "graph.h"
#include "corner.h"

uint16_t calculateDistance(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY)
{
  uint8_t distanceX = fromX - toX;
  uint8_t distanceY = fromY - toY;

  return distanceX * distanceX + distanceY * distanceY;
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

uint8_t **generateAdjacencyMatrix(CornersList *vertexList, EdgeList *edgeList, int rollWidth)
{
  uint8_t **adjacencyMatrix = (uint8_t **)calloc(vertexList->numCorners, sizeof(uint8_t *));
  for (int i = 0; i < vertexList->numCorners; i++)
  {
    adjacencyMatrix[i] = (uint8_t *)calloc(vertexList->numCorners, sizeof(uint8_t));
  }

  for (int i = 0; i < vertexList->numCorners; i++)
  {
    Corner *corner = vertexList->corners[i];

    int fromCornerIndex = searchCorner(vertexList, edge.fromCorner);
    int toCornerIndex = searchCorner(vertexList, edge.toCorner);

    adjacencyMatrix[fromCornerIndex][toCornerIndex] = 1;
    adjacencyMatrix[toCornerIndex][fromCornerIndex] = 1;
  }

  return adjacencyMatrix;
}

void dfsTraverse(uint8_t **adjacencyMatrix, CornersList *cornersList, int rollWidth, int x, int y, bool *visited)
{
  Corner corner = {x, y, false, 0};
  corner.hash = getCornerHash(corner, rollWidth);
  int cornerIndex = searchCorner(cornersList, corner);
  if (cornerIndex == -1)
    return;

  visited[cornerIndex] = true;

  for (int i = 0; i < cornersList->numCorners; i++)
  {
    if (adjacencyMatrix[cornerIndex][i] == 1 && !visited[i])
    {
      printf("(%d, %d) -> (%d, %d)\n", cornersList->corners[cornerIndex]->x, cornersList->corners[cornerIndex]->y, cornersList->corners[i]->x, cornersList->corners[i]->y);
      dfsTraverse(adjacencyMatrix, cornersList, rollWidth, cornersList->corners[i]->x, cornersList->corners[i]->y, visited);
    }
  }
}
