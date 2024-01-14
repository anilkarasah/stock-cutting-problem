#include "edge.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "corner.h"

uint8_t **prepareEdgeMatrix(Data *data)
{
  int width = data->rollWidth * 2 + 1;
  int height = data->rollHeight * 2 + 1;

  uint8_t **edgeMatrix = (uint8_t **)malloc(height * sizeof(uint8_t *));
  for (int i = 0; i < height; i++)
  {
    edgeMatrix[i] = (uint8_t *)calloc(width, sizeof(uint8_t));
  }

  for (int i = 0; i < data->rollHeight; i++)
  {
    for (int j = 0; j < data->rollWidth; j++)
    {
      int itemId = data->roll[i][j];
      if (itemId == 0)
        continue;

      edgeMatrix[EDGE_CELL(i)][EDGE_CELL(j)] = itemId;
    }
  }

  return edgeMatrix;
}

void processEdgeMatrix(Data *data, uint8_t **edgeMatrix)
{
  int edgeMatrixWidth = EDGE_CELL(data->rollWidth);
  int edgeMatrixHeight = EDGE_CELL(data->rollHeight);

  // fill top and bottom edges
  memset(edgeMatrix[0], EDGE_VALUE, edgeMatrixWidth);
  memset(edgeMatrix[edgeMatrixHeight - 1], EDGE_VALUE, edgeMatrixWidth);

  // fill left and right edges
  for (int i = 0; i < edgeMatrixHeight; i++)
  {
    edgeMatrix[i][0] = EDGE_VALUE;
    edgeMatrix[i][edgeMatrixWidth - 1] = EDGE_VALUE;
  }

  // fill inner horizontal edges
  for (int i = 1; i < edgeMatrixHeight; i += 2)
  {
    int j = 1;
    int prevItemId = edgeMatrix[i][j];

    while (j < edgeMatrixWidth - 2)
    {
      if (edgeMatrix[i][j + 2] != prevItemId)
      {
        edgeMatrix[i][j + 1] = edgeMatrix[i + 1][j + 1] = EDGE_VALUE;
      }

      j += 2;
      prevItemId = edgeMatrix[i][j];
    }
  }

  // fill inner vertical edges
  for (int j = 1; j < edgeMatrixWidth; j += 2)
  {
    int i = 1;
    int prevItemId = edgeMatrix[i][j];

    while (i < edgeMatrixHeight - 2)
    {
      if (edgeMatrix[i + 2][j] != prevItemId)
      {
        edgeMatrix[i + 1][j] = edgeMatrix[i + 1][j + 1] = EDGE_VALUE;
      }

      i += 2;
      prevItemId = edgeMatrix[i][j];
    }
  }
}

void printEdgeMatrix(uint8_t **edgeMatrix, int width, int height)
{
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (edgeMatrix[i][j] == EDGE_VALUE)
        printf("##");
      else
        printf("  ");
    }
    printf("\n");
  }
}

void printEdgeMatrix2(uint8_t **edgeMatrix, int width, int height)
{
  printf("%4s", "");
  for (int i = 0; i < width; i++)
  {
    printf("%3d", i);
  }

  for (int i = 0; i < height; i++)
  {
    printf("\n%2d: ", i);
    for (int j = 0; j < width; j++)
    {
      if (edgeMatrix[i][j] == EDGE_VALUE)
        printf("<#>");
      else
        printf("   ");
    }
  }
  printf("\n");
}

EdgeList *initEdgeList(int edgeListSize)
{
  EdgeList *edgeList = (EdgeList *)malloc(sizeof(EdgeList));
  edgeList->edgeList = (Edge *)calloc(edgeListSize, sizeof(Edge));
  edgeList->numEdges = 0;

  return edgeList;
}

bool checkVertex(uint8_t **edgeMatrix, int x, int y, int edgeMatrixWidth, int edgeMatrixHeight)
{
  if (edgeMatrix[x][y] < EDGE_VALUE - 1)
    return false;

  // check corner of the edge matrix
  if ((x == 0 || x == edgeMatrixWidth - 1) && (y == 0 || y == edgeMatrixHeight - 1))
    return true;

  // top or bottom edge
  if ((y == 0 && edgeMatrix[x][y + 1] >= EDGE_VALUE - 1) || (y == edgeMatrixHeight - 1 && edgeMatrix[x][y - 1] >= EDGE_VALUE - 1))
    return true;

  // out of boundaries
  if (y <= 0 || y >= edgeMatrixHeight - 1)
    return false;

  bool hasHorizontalEdge = edgeMatrix[x - 1][y] >= EDGE_VALUE - 1 || edgeMatrix[x + 1][y] >= EDGE_VALUE - 1;
  bool hasVerticalEdge = edgeMatrix[x][y - 1] >= EDGE_VALUE - 1 || edgeMatrix[x][y + 1] >= EDGE_VALUE - 1;

  // check if there is any edge in the 4 directions
  if (hasHorizontalEdge && hasVerticalEdge)
    return true;

  return false;
}

bool checkVertexAlreadyExist(CornersList *vertexList, int x, int y)
{
  for (int i = 0; i < vertexList->numCorners; i++)
  {
    if (vertexList->corners[i]->x == x && vertexList->corners[i]->y == y)
      return true;
  }

  return false;
}

int compareCorner(const void *corner1, const void *corner2)
{
  return ((Corner *)corner1)->hash - ((Corner *)corner2)->hash;
}

CornersList *calculateVertexList(Data *data, EdgeList *edgeList)
{
  int vertexListSize = edgeList->numEdges * 2;
  CornersList *vertexList = (CornersList *)malloc(sizeof(CornersList));
  vertexList->numCorners = 0;
  vertexList->corners = (Corner **)malloc(vertexListSize * sizeof(Corner *));
  for (int i = 0; i < vertexListSize; i++)
  {
    vertexList->corners[i] = (Corner *)malloc(sizeof(Corner));
  }

  vertexList->numCorners = 0;

  // traverse all edges
  for (int i = 0; i < edgeList->numEdges; i++)
  {
    Edge currentEdge = edgeList->edgeList[i];

    if (!checkVertexAlreadyExist(vertexList, currentEdge.fromCorner.x, currentEdge.fromCorner.y))
    {
      Corner *corner = (Corner *)malloc(sizeof(Corner));
      corner->x = currentEdge.fromCorner.x;
      corner->y = currentEdge.fromCorner.y;
      corner->hash = getCornerHash(*corner, data->rollWidth);

      // add corner to vertex list
      vertexList->corners[vertexList->numCorners] = corner;
      vertexList->numCorners++;
    }

    if (!checkVertexAlreadyExist(vertexList, currentEdge.toCorner.x, currentEdge.toCorner.y))
    {
      Corner *corner = (Corner *)malloc(sizeof(Corner));
      corner->x = currentEdge.toCorner.x;
      corner->y = currentEdge.toCorner.y;
      corner->hash = getCornerHash(*corner, data->rollWidth);

      // add corner to vertex list
      vertexList->corners[vertexList->numCorners] = corner;
      vertexList->numCorners++;
    }
  }

  return vertexList;
}
