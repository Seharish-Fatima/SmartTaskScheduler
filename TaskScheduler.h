#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "Heap.h"
#include "LinkedList.h"
#include "Trie.h"
#include "Graph.h"

class TaskScheduler {
private:
    Heap taskQueue;
    LinkedList taskHistory;
    Trie taskSearch;
    Graph taskDependencies;

public:
    void addTask(string task, int priority);
    string getNextTask();
    void completeTask();
};

#endif
