#ifndef GRAPH_H
#define GRAPH_H

#include "types.h"

uint8_t **generateAdjacencyMatrix(CornersList *cornersList, EdgeList *edgeList, int rollWidth);
void dfsTraverse(uint8_t **adjacencyMatrix, CornersList *cornersList, int rollHeight, int x, int y, bool *visited);

#endif // GRAPH_H