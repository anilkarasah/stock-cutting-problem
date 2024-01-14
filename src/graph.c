#include <stdlib.h>
#include "graph.h"
#include "corner.h"
#include "edge.h"

uint16_t calculateDistance(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY)
{
  uint8_t distanceX = fromX - toX;
  uint8_t distanceY = fromY - toY;

  return distanceX * distanceX + distanceY * distanceY;
}

int searchCorner(CornersList *cornersList, Corner corner)
{
  // linear search
  int i = 0;
  while (i < cornersList->numCorners && (cornersList->corners[i]->x != corner.x || cornersList->corners[i]->y != corner.y))
    i++;

  if (i < cornersList->numCorners)
    return i;

  return -1;
}

void calculateVertexAndEdgeList(Data *data, EdgeList *edgeList, uint8_t **edgeMatrix, Corner from, Corner current)
{
  int edgeMatrixWidth = EDGE_CELL(data->rollWidth);
  int edgeMatrixHeight = EDGE_CELL(data->rollHeight);

  // out of bound
  if (current.x >= edgeMatrixWidth || current.y >= edgeMatrixHeight || edgeMatrix[current.x][current.y] != EDGE_VALUE)
    return;

  if ((from.x == current.x && from.y != current.y && ((current.x - 1 >= 0 && edgeMatrix[current.x - 1][current.y] >= EDGE_VALUE - 1) || (current.x + 1 < edgeMatrixHeight && edgeMatrix[current.x + 1][current.y] >= EDGE_VALUE - 1))) || (from.y == current.y && from.x != current.x && ((current.y - 1 >= 0 && edgeMatrix[current.x][current.y - 1] >= EDGE_VALUE - 1) || (current.y + 1 < edgeMatrixWidth && edgeMatrix[current.x][current.y + 1] >= EDGE_VALUE - 1))))
  {
    // add new edge
    uint8_t fromX = from.x / 2;
    uint8_t fromY = from.y / 2;
    uint8_t x = current.x / 2;
    uint8_t y = current.y / 2;

    Corner fromNew = {fromX, fromY, false, 0};
    Corner currentNew = {x, y, false, 0};

    setCornerValues(&(edgeList->edgeList[edgeList->numEdges].fromCorner), fromX, fromY, false, getCornerHash(fromNew, data->rollWidth));
    setCornerValues(&(edgeList->edgeList[edgeList->numEdges].toCorner), x, y, false, getCornerHash(currentNew, data->rollWidth));
    edgeList->numEdges++;

    // mark edge as visited (do not mark vertex as visited for future visits)
    if (from.x == current.x)
    {
      if (from.y > current.y)
        for (int i = current.y + 1; i < from.y; i++)
          edgeMatrix[current.x][i] = EDGE_VALUE - 1;
      else
        for (int i = from.y + 1; i < current.y; i++)
          edgeMatrix[current.x][i] = EDGE_VALUE - 1;
    }
    else if (from.y == current.y)
    {
      if (from.x > current.x)
        for (int i = current.x + 1; i < from.x; i++)
          edgeMatrix[i][current.y] = EDGE_VALUE - 1;
      else
        for (int i = from.x + 1; i < current.x; i++)
          edgeMatrix[i][current.y] = EDGE_VALUE - 1;
    }

    setCornerValues(&from, current.x, current.y, false, getCornerHash(current, data->rollWidth));
  }

  Corner rightCorner = {current.x + 1, current.y, false, 0};
  rightCorner.hash = getCornerHash(rightCorner, data->rollWidth);
  Corner bottomCorner = {current.x, current.y + 1, false, 0};
  bottomCorner.hash = getCornerHash(bottomCorner, data->rollWidth);

  calculateVertexAndEdgeList(data, edgeList, edgeMatrix, from, rightCorner);
  calculateVertexAndEdgeList(data, edgeList, edgeMatrix, from, bottomCorner);
}

uint8_t **generateAdjacencyMatrix(CornersList *vertexList, EdgeList *edgeList)
{
  uint8_t **adjacencyMatrix = (uint8_t **)calloc(vertexList->numCorners, sizeof(uint8_t *));
  for (int i = 0; i < vertexList->numCorners; i++)
  {
    adjacencyMatrix[i] = (uint8_t *)calloc(vertexList->numCorners, sizeof(uint8_t));
  }

  for (int i = 0; i < vertexList->numCorners; i++)
  {
    Edge edge = edgeList->edgeList[i];

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
