#include "Heap.h"

Heap::Heap() {
    size = 0;
}

void Heap::insert(string name, int priority) {
    tasks[size].name = name;
    tasks[size].priority = priority;
    int i = size;
    while (i > 0 && tasks[i].priority < tasks[(i - 1) / 2].priority) {
        swap(tasks[i], tasks[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
    size++;
}

string Heap::getTop() {
    if (size > 0) return tasks[0].name;
    return "No Tasks Available";
}

string Heap::extractTop() {
    if (size == 0) return "No Tasks Available";
    string topTask = tasks[0].name;
    tasks[0] = tasks[--size];
    int i = 0;
    while (2 * i + 1 < size) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = left;
        if (right < size && tasks[right].priority < tasks[left].priority)
            smallest = right;
        if (tasks[i].priority <= tasks[smallest].priority)
            break;
        swap(tasks[i], tasks[smallest]);
        i = smallest;
    }
    return topTask;
}
