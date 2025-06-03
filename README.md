# 🌟 TaskMaster: Advanced C++ Task Scheduler

<div align="center">

**Organize, Prioritize, and Never Miss a Deadline Again**

</div>

## ✨ Features

- **Efficient Task Management**: Add, modify, and remove tasks with ease
- **Priority Queue System**: Tasks automatically sorted by importance
- **Status Tracking**: Monitor tasks through different states (Pending, In Progress, Completed)
- **Detailed Task Information**: Store and display comprehensive task details
- **Undo/Redo Functionality**: Made a mistake? No problem!
- **Hash-Based Lookup**: Fast task retrieval by ID
- **Multiple Display Options**: View tasks by status, priority, or structure

## 🚀 Getting Started

### Prerequisites

- C++ compiler (supporting C++11 or later)
- Basic knowledge of console applications

### Compilation

```bash
g++ -std=c++11 main.cpp -o taskmaster
```

### Running the Application

```bash
./taskmaster
```

## 📖 How to Use

TaskMaster offers a menu-driven interface for interacting with your tasks:

1. **Add New Task**: Create a task with name, description, priority, status, and due date
2. **Remove Task**: Delete a task by its ID
3. **Modify Task**: Update any property of an existing task
4. **Change Task Status**: Quickly update a task's progress state
5. **Display All Tasks**: See your complete task list
6. **Display by Status**: Filter tasks by completion state
7. **Display by Priority**: View tasks ordered by importance
8. **Display Task Structure**: See how tasks are organized in memory
9. **Undo Last Action**: Revert your most recent change
10. **Redo Last Action**: Restore a previously undone action

## 🧩 Data Structures

TaskMaster implements several advanced data structures:

- **Priority Queue (Max Heap)**: Organizes tasks by priority level
- **Hash Map**: Provides O(1) average-case lookup by task ID
- **Stack**: Enables undo/redo functionality

## 🔍 Technical Details

### Task Properties

Each task in TaskMaster contains:

- Unique ID
- Name
- Description
- Status (Pending, In Progress, Completed)
- Priority (1-5)
- Due Date
- Creation Date
- Completion Date (when applicable)

### Memory Management

TaskMaster carefully manages memory to prevent leaks:

- Uses proper destructors
- Implements copy constructors and assignment operators
- Maintains ownership of dynamically allocated objects

## 💡 Example Usage

```cpp
// Create a new high-priority task due next week
scheduler.addTask("Complete project proposal",
                 "Finish the draft and send to team for review",
                 PENDING,
                 5,
                 time(0) + 60*60*24*7);

// Mark a task as in progress
scheduler.changeTaskStatus(1, IN_PROGRESS);

// Complete a task
scheduler.changeTaskStatus(1, COMPLETED);
```

## 🔮 Future Enhancements

- Persistent storage (save/load from file)
- Task categories and tags
- Due date notifications
- Recurring tasks
- Multi-user support
- GUI interface

## 📚 Class Structure

- **Task**: Base unit for storing task information
- **PriorityQueue**: Manages tasks by priority using a heap
- **TaskHashMap**: Provides efficient task lookup by ID
- **TaskState**: Captures task state for undo/redo operations
- **TaskStack**: Implements the undo/redo stack
- **TaskScheduler**: Coordinates all components

## 🎨 Design Philosophy

TaskMaster was built with the following principles in mind:

- **Efficiency**: Fast operations regardless of task count
- **Robustness**: Handle errors gracefully
- **Flexibility**: Adapt to various task management needs
- **Usability**: Simple interface for complex functionality

---

<div align="center">
  
**Made with ❤️ and C++**

</div>
