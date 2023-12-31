#include "edge.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

void calculateEdges(Data *data, uint8_t **edgeMatrix)
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

void printEdges(uint8_t **edgeMatrix, int width, int height)
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
