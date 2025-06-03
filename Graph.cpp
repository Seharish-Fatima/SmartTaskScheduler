#include "Graph.h"

Graph::Graph() {
    for (int i = 0; i < MAX_TASKS; i++)
        for (int j = 0; j < MAX_TASKS; j++)
            adjMatrix[i][j] = 0;
}

void Graph::addDependency(int from, int to) {
    adjMatrix[from][to] = 1;
}
