#ifndef GRAPH_H
#define GRAPH_H

#include "types.h"

void calculateVertexAndEdgeList(Data *data, EdgeList *edgeList, uint8_t **edgeMatrix, Corner from, Corner current);
uint8_t **generateAdjacencyMatrix(CornersList *cornersList, EdgeList *edgeList);
void dfsTraverse(uint8_t **adjacencyMatrix, CornersList *cornersList, int rollHeight, int x, int y, bool *visited);

#endif // GRAPH_H