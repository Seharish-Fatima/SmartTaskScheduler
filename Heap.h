#include "TaskScheduler.h"

void TaskScheduler::addTask(string task, int priority) {
    taskQueue.insert(task, priority);
    taskSearch.insert(task);
}

string TaskScheduler::getNextTask() {
    return taskQueue.getTop();
}

void TaskScheduler::completeTask() {
    string task = taskQueue.extractTop();
    taskHistory.insert(task);
}
