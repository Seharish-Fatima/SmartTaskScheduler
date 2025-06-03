#include <iostream>
#include <string>
#include <ctime>
#include <fstream>  // Added for file handling

using namespace std;

const int TABLE_SIZE = 10; 
const int MAX_TASKS = 100; 
const string FILENAME = "tasks.txt";  // File to store tasks

enum TaskStatus 
{
    PENDING,
    IN_PROGRESS,
    COMPLETED
};

class Task 
{
    public:
        int taskId;
        string taskName;
        string taskDescription;
        TaskStatus taskStatus;
        int taskPriority;
        time_t taskDueDate;
        time_t taskCreationDate;
        time_t taskCompletionDate;
        Task* next;  
        Task() : taskId(-1), taskName(""), taskDescription(""), taskStatus(PENDING),
            taskPriority(0), taskDueDate(0), taskCreationDate(0), taskCompletionDate(0), next(nullptr) {}
        Task(int id, string name, string description, TaskStatus status, int priority, time_t dueDate)
            : taskId(id), taskName(name), taskDescription(description), taskStatus(status),
            taskPriority(priority), taskDueDate(dueDate), taskCreationDate(time(0)), taskCompletionDate(0), next(nullptr) {}
        Task(const Task& other)
            : taskId(other.taskId), taskName(other.taskName), taskDescription(other.taskDescription),
            taskStatus(other.taskStatus), taskPriority(other.taskPriority), taskDueDate(other.taskDueDate),
            taskCreationDate(other.taskCreationDate), taskCompletionDate(other.taskCompletionDate), next(nullptr) {}
        Task& operator=(const Task& other) 
        {
            if (this != &other) 
            {
                taskId = other.taskId;
                taskName = other.taskName;
                taskDescription = other.taskDescription;
                taskStatus = other.taskStatus;
                taskPriority = other.taskPriority;
                taskDueDate = other.taskDueDate;
                taskCreationDate = other.taskCreationDate;
                taskCompletionDate = other.taskCompletionDate;
            }
            return *this;
        }
        void completeTask() 
        {
            taskStatus = COMPLETED;
            taskCompletionDate = time(0);
        }
        string formatTime(time_t t) const 
        {
            if (t == 0) return "N/A";
            char buffer[20];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));
            return string(buffer);
        }
        void displayTask() const 
        {
            cout << "Task ID: " << taskId << endl;
            cout << "Task Name: " << taskName << endl;
            cout << "Task Description: " << taskDescription << endl;
            cout << "Task Status: " << (taskStatus == PENDING ? "Pending" : (taskStatus == IN_PROGRESS ? "In Progress" : "Completed")) << endl;
            cout << "Task Priority: " << taskPriority << endl;
            cout << "Task Due Date: " << formatTime(taskDueDate) << endl;
            cout << "Task Creation Date: " << formatTime(taskCreationDate) << endl;
            if (taskCompletionDate != 0)
                cout << "Task Completion Date: " << formatTime(taskCompletionDate) << endl;
            cout << "------------------------" << endl;
        }
        bool isValid() const 
        {
            return taskId != -1;
        }
        
        // Added for file handling - write task to file stream
        void writeToFile(ofstream& outFile) const 
        {
            outFile << taskId << endl;
            outFile << taskName << endl;
            outFile << taskDescription << endl;
            outFile << taskStatus << endl;
            outFile << taskPriority << endl;
            outFile << taskDueDate << endl;
            outFile << taskCreationDate << endl;
            outFile << taskCompletionDate << endl;
        }
        
        // Added for file handling - read task from file stream
        bool readFromFile(ifstream& inFile) 
        {
            if (!inFile.good()) return false;
            
            inFile >> taskId;
            inFile.ignore(); // Skip newline
            
            getline(inFile, taskName);
            getline(inFile, taskDescription);
            
            int status;
            inFile >> status;
            taskStatus = static_cast<TaskStatus>(status);
            
            inFile >> taskPriority;
            inFile >> taskDueDate;
            inFile >> taskCreationDate;
            inFile >> taskCompletionDate;
            
            inFile.ignore(); // Skip newline
            return true;
        }
};

class PriorityQueue 
{
    private:
        Task** heap;
        int size;
        int capacity;
        void heapify(int i) 
        {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            if (left < size && heap[left]->taskPriority > heap[largest]->taskPriority)
                largest = left;
            if (right < size && heap[right]->taskPriority > heap[largest]->taskPriority)
                largest = right;
            if (largest != i) 
            {
                Task* temp = heap[i];
                heap[i] = heap[largest];
                heap[largest] = temp;
                heapify(largest);
            }
        }
        void buildHeap() 
        {
            for (int i = size / 2 - 1; i >= 0; i--)
                heapify(i);
        }
    public:
        PriorityQueue(int cap = MAX_TASKS) : size(0), capacity(cap) 
        {
            heap = new Task*[capacity];
        }
        ~PriorityQueue() 
        {
            delete[] heap;
        }
        void insert(Task* task) 
        {
            if (size >= capacity) 
            {
                cout << "Priority queue is full!" << endl;
                return;
            }
            heap[size] = task;
            int current = size++;
            while (current > 0 && heap[(current - 1) / 2]->taskPriority < heap[current]->taskPriority) 
            {
                Task* temp = heap[current];
                heap[current] = heap[(current - 1) / 2];
                heap[(current - 1) / 2] = temp;
                current = (current - 1) / 2;
            }
        }
        Task* pop() 
        {
            if (size == 0) return nullptr;
            Task* topTask = heap[0];
            heap[0] = heap[--size];
            heapify(0);
            return topTask;
        }
        bool removeTask(int taskId) 
        {
            int index = -1;
            for (int i = 0; i < size; i++) 
            {
                if (heap[i]->taskId == taskId) 
                {
                    index = i;
                    break;
                }
            }
            if (index == -1) return false;
            heap[index] = heap[size - 1];
            size--;
            buildHeap();
            return true;
        }
        void updateTask(Task* task) 
        {
            bool found = false;
            for (int i = 0; i < size; i++) 
            {
                if (heap[i]->taskId == task->taskId) 
                {
                    found = true;
                    break;
                }
            }
            if (!found) 
            {
                insert(task);
            } 
            else 
            {
                buildHeap();
            }
        }
        void display() const 
        {
            cout << "\n--- Priority Queue (Tasks by Priority) ---\n";
            if (size == 0) {
                cout << "No tasks in the priority queue.\n";
                return;
            }    
            for (int i = 0; i < size; i++) 
            {
                heap[i]->displayTask();
            }
        }
        bool isEmpty() const 
        {
            return size == 0;
        }
        
        // Method to clear the queue - added for file handling
        void clear()
        {
            size = 0;
        }
};

class TaskHashMap 
{
    private:
        Task* table[TABLE_SIZE];
        int hashFunction(int taskID) const 
        {
            return (taskID * 2654435761) % TABLE_SIZE;
        }
    public:
        TaskHashMap() 
        {
            for (int i = 0; i < TABLE_SIZE; i++)
                table[i] = nullptr; 
        }
        void insertTask(Task* task) 
        {
            int index = hashFunction(task->taskId);
            task->next = table[index];
            table[index] = task;
        }
        Task* getTaskByID(int taskID) const 
        {
            int index = hashFunction(taskID);
            Task* current = table[index];
            while (current) 
            {
                if (current->taskId == taskID)
                    return current;
                current = current->next;
            }
            return nullptr;
        }
        bool deleteTask(int taskID) 
        {
            int index = hashFunction(taskID);
            Task* current = table[index];
            Task* prev = nullptr;
            while (current) 
            {
                if (current->taskId == taskID) 
                {
                    if (prev) 
                    {
                        prev->next = current->next;
                    } 
                    else 
                    {
                        table[index] = current->next;  
                    }
                    return true;
                }
                prev = current;
                current = current->next;
            }
            return false;  
        }
        void displayTasks() const 
        {
            cout << "\n--- Task HashMap ---\n";
            bool isEmpty = true;    
            for (int i = 0; i < TABLE_SIZE; i++) 
            {
                if (table[i]) 
                {
                    isEmpty = false;
                    Task* current = table[i];
                    cout << "Bucket " << i << ": ";
                    while (current) 
                    {
                        cout << "[ID: " << current->taskId << ", Name: " << current->taskName
                            << ", Priority: " << current->taskPriority << "] -> ";
                        current = current->next;
                    }
                    cout << "NULL\n";
                }
            }
            if (isEmpty) 
            {
                cout << "No tasks in the hash map.\n";
            }
        }
        
        // Added for file handling - clear all entries
        void clear()
        {
            for (int i = 0; i < TABLE_SIZE; i++)
            {
                table[i] = nullptr;
            }
        }
};

class TaskState 
{
    public:
        int taskId;
        string taskName;
        string taskDescription;
        TaskStatus taskStatus;
        int taskPriority;
        time_t taskDueDate;
        time_t taskCreationDate;
        time_t taskCompletionDate;
        bool exists;
        TaskState() : taskId(-1), exists(false) {}
        TaskState(const Task& task, bool exists = true) 
            : taskId(task.taskId), taskName(task.taskName), 
            taskDescription(task.taskDescription), taskStatus(task.taskStatus),
            taskPriority(task.taskPriority), taskDueDate(task.taskDueDate),
            taskCreationDate(task.taskCreationDate), 
            taskCompletionDate(task.taskCompletionDate),
            exists(exists) {}
};

class StackNode 
{
    public:
        TaskState taskState;
        StackNode* next;
        StackNode(const TaskState& ts) : taskState(ts), next(nullptr) {}
};

class TaskStack 
{
    private:
        StackNode* top;
    public:
        TaskStack() : top(nullptr) {}
        ~TaskStack() 
        {
            while (!isEmpty()) 
            {
                pop();
            }
        }
        void push(const TaskState& taskState) 
        {
            StackNode* newNode = new StackNode(taskState);
            newNode->next = top;
            top = newNode;
        }
        TaskState pop() 
        {
            if (isEmpty()) 
            {
                return TaskState(); 
            }
            StackNode* temp = top;
            TaskState taskState = temp->taskState;
            top = top->next;
            delete temp;
            return taskState;
        }
        bool isEmpty() const 
        {
            return top == nullptr;
        }
        void clear() 
        {
            while (!isEmpty()) 
            {
                pop();
            }
        }
};

class TaskScheduler 
{
    private:
        Task** tasks;
        int taskCount;
        int maxTasks;
        int nextTaskId;
        PriorityQueue priorityQueue;
        TaskHashMap taskLookup;   
        TaskStack undoStack; 
        TaskStack redoStack;
        void recordForUndo(const Task* task, bool exists = true) 
        {
            if (task) 
            {
                undoStack.push(TaskState(*task, exists));
                redoStack.clear();
            }
        }
        void updateNextTaskId(int taskId) 
        {
            if (taskId >= nextTaskId) 
            {
                nextTaskId = taskId + 1;
            }
        }
    public:
        TaskScheduler(int maxTaskCount = TABLE_SIZE) : taskCount(0), maxTasks(maxTaskCount), nextTaskId(1) 
        {
            tasks = new Task*[maxTasks];
            for (int i = 0; i < maxTasks; i++) 
            {
                tasks[i] = nullptr;
            }
        }
        ~TaskScheduler() 
        {
            for (int i = 0; i < taskCount; i++) 
            {
                if (tasks[i]) 
                {
                    delete tasks[i];
                    tasks[i] = nullptr;
                }
            }
            delete[] tasks;
        }
        void addTask(const string& name, const string& description, TaskStatus status, int priority, time_t dueDate) 
        {
            if (taskCount >= maxTasks) 
            {
                cerr << "Task limit reached. Cannot add more tasks.\n";
                return;
            }
            int id = nextTaskId++;
            Task* newTask = new Task(id, name, description, status, priority, dueDate);
            tasks[taskCount++] = newTask;
            taskLookup.insertTask(newTask);
            priorityQueue.insert(newTask);
            recordForUndo(newTask);
            cout << "Task added: " << name << " (ID: " << id << ")" << endl;
            
            // Save tasks after adding
            saveTasks();
        }
        void removeTask(int taskId) 
        {
            Task* taskToRemove = taskLookup.getTaskByID(taskId);    
            if (!taskToRemove) 
            {
                cout << "Task not found.\n";
                return;
            }
            recordForUndo(taskToRemove, false);
            priorityQueue.removeTask(taskId);
            taskLookup.deleteTask(taskId);
            int indexToRemove = -1;
            for (int i = 0; i < taskCount; i++) 
            {
                if (tasks[i] && tasks[i]->taskId == taskId) 
                {
                    indexToRemove = i;
                    break;
                }
            }
            if (indexToRemove != -1) 
            {
                delete tasks[indexToRemove];
                if (indexToRemove < taskCount - 1) 
                {
                    tasks[indexToRemove] = tasks[taskCount - 1];
                }
                tasks[taskCount - 1] = nullptr;
                taskCount--;
            }
            cout << "Task removed successfully.\n";
            
            // Save tasks after removing
            saveTasks();
        }
        void modifyTask(int taskId, const string& newName, const string& newDescription, TaskStatus newStatus, int newPriority, time_t newDueDate) 
        {
            Task* task = taskLookup.getTaskByID(taskId);
            if (!task) 
            {
                cout << "Task not found.\n";
                return;
            }
            recordForUndo(task);
            task->taskName = newName;
            task->taskDescription = newDescription;
            if (newStatus == COMPLETED && task->taskStatus != COMPLETED) 
            {
                task->completeTask();
            } 
            else 
            {
                task->taskStatus = newStatus;
            }
            bool needPriorityUpdate = (task->taskPriority != newPriority);
            task->taskPriority = newPriority;
            task->taskDueDate = newDueDate;
            priorityQueue.updateTask(task);
            cout << "Task modified successfully.\n";
            
            // Save tasks after modifying
            saveTasks();
        }
        void changeTaskStatus(int taskId, TaskStatus newStatus) 
        {
            Task* task = taskLookup.getTaskByID(taskId);
            if (!task) 
            {
                cout << "Task not found.\n";
                return;
            }
            recordForUndo(task);
            if (newStatus == COMPLETED && task->taskStatus != COMPLETED) 
            {
                task->completeTask();
            } 
            else 
            {
                task->taskStatus = newStatus;
            }
            priorityQueue.updateTask(task);
            cout << "Task status updated successfully.\n";
            
            // Save tasks after changing status
            saveTasks();
        }
        void undo() 
        {
            if (undoStack.isEmpty()) 
            {
                cout << "Nothing to undo.\n";
                return;
            }
            TaskState lastAction = undoStack.pop();
            if (lastAction.exists) 
            {
                Task* currentTask = taskLookup.getTaskByID(lastAction.taskId);
                if (currentTask) 
                {
                    redoStack.push(TaskState(*currentTask));    
                    currentTask->taskName = lastAction.taskName;
                    currentTask->taskDescription = lastAction.taskDescription;
                    currentTask->taskStatus = lastAction.taskStatus;
                    currentTask->taskPriority = lastAction.taskPriority;
                    currentTask->taskDueDate = lastAction.taskDueDate;
                    currentTask->taskCreationDate = lastAction.taskCreationDate;
                    currentTask->taskCompletionDate = lastAction.taskCompletionDate;
                    priorityQueue.updateTask(currentTask);
                } 
                else 
                {
                    Task* newTask = new Task(lastAction.taskId, lastAction.taskName, lastAction.taskDescription, lastAction.taskStatus, lastAction.taskPriority, lastAction.taskDueDate);
                    newTask->taskCreationDate = lastAction.taskCreationDate;
                    newTask->taskCompletionDate = lastAction.taskCompletionDate;  
                    updateNextTaskId(lastAction.taskId);
                    if (taskCount < maxTasks) 
                    {
                        tasks[taskCount++] = newTask;
                        taskLookup.insertTask(newTask);
                        priorityQueue.insert(newTask);    
                        redoStack.push(TaskState(*newTask, false)); 
                    } 
                    else 
                    {
                        cout << "Cannot undo - task limit reached.\n";
                        delete newTask;
                        return;
                    }
                }
            } 
            else 
            {
                Task* taskToDelete = taskLookup.getTaskByID(lastAction.taskId);
                if (taskToDelete) 
                {
                    redoStack.push(TaskState(*taskToDelete));    
                    priorityQueue.removeTask(lastAction.taskId);
                    taskLookup.deleteTask(lastAction.taskId);
                    for (int i = 0; i < taskCount; i++) 
                    {
                        if (tasks[i] && tasks[i]->taskId == lastAction.taskId) 
                        {
                            delete tasks[i];
                            if (i < taskCount - 1) 
                            {
                                tasks[i] = tasks[taskCount - 1];
                            }
                            tasks[taskCount - 1] = nullptr;
                            taskCount--;
                            break;
                        }
                    }
                }
            }
            cout << "Undo successful.\n";
            
            // Save tasks after undo
            saveTasks();
        }
        void redo() 
        {
            if (redoStack.isEmpty()) 
            {
                cout << "Nothing to redo.\n";
                return;
            }
            TaskState lastUndone = redoStack.pop();
            if (lastUndone.exists) 
            {
                Task* currentTask = taskLookup.getTaskByID(lastUndone.taskId);    
                if (currentTask) 
                {
                    undoStack.push(TaskState(*currentTask));
                    currentTask->taskName = lastUndone.taskName;
                    currentTask->taskDescription = lastUndone.taskDescription;
                    currentTask->taskStatus = lastUndone.taskStatus;
                    currentTask->taskPriority = lastUndone.taskPriority;
                    currentTask->taskDueDate = lastUndone.taskDueDate;
                    currentTask->taskCreationDate = lastUndone.taskCreationDate;
                    currentTask->taskCompletionDate = lastUndone.taskCompletionDate;    
                    priorityQueue.updateTask(currentTask);
                } 
                else 
                {
                    Task* newTask = new Task(lastUndone.taskId, lastUndone.taskName, lastUndone.taskDescription, lastUndone.taskStatus, lastUndone.taskPriority, lastUndone.taskDueDate);
                    newTask->taskCreationDate = lastUndone.taskCreationDate;
                    newTask->taskCompletionDate = lastUndone.taskCompletionDate;
                    updateNextTaskId(lastUndone.taskId);
                    if (taskCount < maxTasks) 
                    {
                        tasks[taskCount++] = newTask;
                        taskLookup.insertTask(newTask);
                        priorityQueue.insert(newTask);    
                        undoStack.push(TaskState(*newTask, false));
                    } 
                    else 
                    {
                        cout << "Cannot redo - task limit reached.\n";
                        delete newTask;
                        return;
                    }
                }
            } 
            else 
            {
                Task* taskToDelete = taskLookup.getTaskByID(lastUndone.taskId);
                if (taskToDelete) 
                {
                    undoStack.push(TaskState(*taskToDelete));    
                    priorityQueue.removeTask(lastUndone.taskId);
                    taskLookup.deleteTask(lastUndone.taskId);
                    for (int i = 0; i < taskCount; i++) 
                    {
                        if (tasks[i] && tasks[i]->taskId == lastUndone.taskId) 
                        {
                            delete tasks[i];
                            if (i < taskCount - 1) 
                            {
                                tasks[i] = tasks[taskCount - 1];
                            }
                            tasks[taskCount - 1] = nullptr;
                            taskCount--;
                            break;
                        }
                    }
                }
            }
            cout << "Redo successful.\n";
            
            // Save tasks after redo
            saveTasks();
        }
        void displayAllTasks() const 
        {
            if (taskCount == 0) 
            {
                cout << "No tasks to display.\n";
                return;
            }
            cout << "\n--- All Tasks ---\n";
            for (int i = 0; i < taskCount; i++) 
            {
                if (tasks[i]) 
                {
                    tasks[i]->displayTask();
                }
            }
        }
        void displayTasksByStatus(TaskStatus status) const 
        {
            cout << "\n--- Tasks with Status: " << (status == PENDING ? "Pending" : (status == IN_PROGRESS ? "In Progress" : "Completed")) << " ---\n";
            bool found = false;
            for (int i = 0; i < taskCount; i++) 
            {
                if (tasks[i] && tasks[i]->taskStatus == status) 
                {
                    tasks[i]->displayTask();
                    found = true;
                }
            }
            if (!found) 
            {
                cout << "No tasks with the specified status.\n";
            }
        }
        void displayTasksByPriority() const 
        {
            priorityQueue.display();
        }
        void displayTaskStructure() const 
        {
            taskLookup.displayTasks();
        }
        int getTaskCount() const 
        {
            return taskCount;
        }
        Task* getTaskByIndex(int index) const 
        {
            if (index >= 0 && index < taskCount) 
            {
                return tasks[index];
            }
            return nullptr;
        }
        
        // New methods for file handling
        bool saveTasks() const
        {
            ofstream outFile(FILENAME);
            if (!outFile.is_open())
            {
                cerr << "Error: Could not open file for writing." << endl;
                return false;
            }
            
            // First, write the next task ID and task count
            outFile << nextTaskId << endl;
            outFile << taskCount << endl;
            
            // Then write each task's data
            for (int i = 0; i < taskCount; i++)
            {
                if (tasks[i])
                {
                    tasks[i]->writeToFile(outFile);
                }
            }
            
            outFile.close();
            return true;
        }
        
        bool loadTasks()
        {
            ifstream inFile(FILENAME);
            if (!inFile.is_open())
            {
                cout << "No saved tasks found or could not open file." << endl;
                return false;
            }
            
            // Clear existing data
            for (int i = 0; i < taskCount; i++)
            {
                if (tasks[i])
                {
                    delete tasks[i];
                    tasks[i] = nullptr;
                }
            }
            taskCount = 0;
            nextTaskId = 1;
            taskLookup.clear();
            priorityQueue.clear();
            
            // Read next task ID and task count
            inFile >> nextTaskId;
            inFile >> taskCount;
            inFile.ignore(); // Skip newline
            
            // Check if task count is valid
            if (taskCount > maxTasks)
            {
                cerr << "Error: Task count in file exceeds maximum." << endl;
                taskCount = 0;
                inFile.close();
                return false;
            }
            
            // Read tasks
            for (int i = 0; i < taskCount; i++)
            {
                Task* newTask = new Task();
                if (newTask->readFromFile(inFile))
                {
                    tasks[i] = newTask;
                    taskLookup.insertTask(newTask);
                    priorityQueue.insert(newTask);
                }
                else
                {
                    delete newTask;
                    taskCount = i;
                    cerr << "Error: Failed to read task " << i + 1 << " from file." << endl;
                    break;
                }
            }
            
            inFile.close();
            cout << "Loaded " << taskCount << " tasks from file." << endl;
            return true;
        }
};

int main() 
{
    TaskScheduler scheduler;
    int choice = 0;
    
    // Load tasks at start
    scheduler.loadTasks();
    
    while (true) 
    {
        cout << "\n===== TASK SCHEDULER MENU =====\n";
        cout << "1. Add New Task\n";
        cout << "2. Remove Task\n";
        cout << "3. Modify Task\n";
        cout << "4. Change Task Status\n";
        cout << "5. Display All Tasks\n";
        cout << "6. Display Tasks by Status\n";
        cout << "7. Display Tasks by Priority\n";
        cout << "8. Display Task Structure\n";
        cout << "9. Undo Last Action\n";
        cout << "10. Redo Last Action\n";
        cout << "11. Save Tasks to File\n";  // Added option
        cout << "12. Load Tasks from File\n"; // Added option
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        if (choice == 0) 
        {
            // Save tasks before exiting
            scheduler.saveTasks();
            cout << "Tasks saved. Exiting Task Scheduler. Goodbye!\n";
            return 0;
        }
        else if (choice == 1) 
        {
            string taskName, taskDescription;
            int taskPriority;
            int statusInput;
            TaskStatus taskStatus;
            char dateStr[11];    
            cout << "Enter task name: ";
            getline(cin, taskName);
            cout << "Enter task description: ";
            getline(cin, taskDescription);
            cout << "Enter task priority (1-5): ";
            cin >> taskPriority;
            cout << "Enter task status (0: Pending, 1: In Progress, 2: Completed): ";
            cin >> statusInput;
            taskStatus = static_cast<TaskStatus>(statusInput);
            cout << "Enter due date (YYYY-MM-DD): ";
            cin >> dateStr;
            struct tm dueDate = {0};
            if (sscanf(dateStr, "%d-%d-%d", &dueDate.tm_year, &dueDate.tm_mon, &dueDate.tm_mday) == 3) 
            {
                dueDate.tm_year -= 1900;
                dueDate.tm_mon -= 1;
                time_t taskDueDate = mktime(&dueDate);
                scheduler.addTask(taskName, taskDescription, taskStatus, taskPriority, taskDueDate);
            } 
            else 
            {
                cout << "Invalid date format. Task not added.\n";
            }
            cin.ignore();
        }
        else if (choice == 2) 
        {
            int taskId;
            cout << "Enter task ID to remove: ";
            cin >> taskId;
            scheduler.removeTask(taskId);
        }
        else if (choice == 3) 
        {
            string taskName, taskDescription;
            int taskId, taskPriority;
            int statusInput;
            TaskStatus taskStatus;
            char dateStr[11];    
            cout << "Enter task ID to modify: ";
            cin >> taskId;
            cin.ignore();            
            cout << "Enter new task name: ";
            getline(cin, taskName);            
            cout << "Enter new task description: ";
            getline(cin, taskDescription);
            cout << "Enter new task priority (1-5): ";
            cin >> taskPriority;
            cout << "Enter new task status (0: Pending, 1: In Progress, 2: Completed): ";
            cin >> statusInput;
            taskStatus = static_cast<TaskStatus>(statusInput);
            cout << "Enter new due date (YYYY-MM-DD): ";
            cin >> dateStr;
            struct tm newDueDate = {0};
            if (sscanf(dateStr, "%d-%d-%d", &newDueDate.tm_year, &newDueDate.tm_mon, &newDueDate.tm_mday) == 3) 
            {
                newDueDate.tm_year -= 1900;
                newDueDate.tm_mon -= 1;
                time_t taskDueDate = mktime(&newDueDate);
                scheduler.modifyTask(taskId, taskName, taskDescription, taskStatus, taskPriority, taskDueDate);
            } 
            else 
            {
                cout << "Invalid date format. Task not modified.\n";
            }
            cin.ignore();
        }
        else if (choice == 4) 
        {
            int taskId;
            int statusInput;
            TaskStatus taskStatus;    
            cout << "Enter task ID: ";
            cin >> taskId;
            cout << "Enter new status (0: Pending, 1: In Progress, 2: Completed): ";
            cin >> statusInput;
            taskStatus = static_cast<TaskStatus>(statusInput);
            scheduler.changeTaskStatus(taskId, taskStatus);
        }
        else if (choice == 5) 
        {
            scheduler.displayAllTasks();
        }
        else if (choice == 6) 
        {
            int statusInput;
            TaskStatus taskStatus;    
            cout << "Enter status to display (0: Pending, 1: In Progress, 2: Completed): ";
            cin >> statusInput;
            taskStatus = static_cast<TaskStatus>(statusInput);
            scheduler.displayTasksByStatus(taskStatus);
        }
        else if (choice == 7) 
        {
            scheduler.displayTasksByPriority();
        }
        else if (choice == 8) 
        {
            scheduler.displayTaskStructure();
        }
        else if (choice == 9) 
        {
            scheduler.undo();
        }
        else if (choice == 10) 
        {
            scheduler.redo();
        }
        else if (choice == 11) 
        {
            if (scheduler.saveTasks())
            {
                cout << "Tasks saved to file successfully.\n";
            }
            else
            {
                cout << "Failed to save tasks to file.\n";
            }
        }
        else if (choice == 12) 
        {
            if (scheduler.loadTasks())
            {
                cout << "Tasks loaded from file successfully.\n";
            }
            else
            {
                cout << "Failed to load tasks from file or no saved tasks found.\n";
            }
        }
        else 
        {
            cout << "Invalid choice. Please try again.\n";
        }
        cout << "\nPress Enter to continue...";
        cin.ignore();
        string temp;
        getline(cin, temp);
    }
    return 0;
}