// Command pattern for undo/redo functionality
class CommandManager {
  constructor() {
    this.undoStack = [];
    this.redoStack = [];
  }

  execute(command) {
    command.execute();
    this.undoStack.push(command);
    // Clear redo stack when a new command is executed
    this.redoStack = [];
  }

  undo() {
    if (this.undoStack.length === 0) return false;

    const command = this.undoStack.pop();
    command.undo();
    this.redoStack.push(command);
    return true;
  }

  redo() {
    if (this.redoStack.length === 0) return false;

    const command = this.redoStack.pop();
    command.execute();
    this.undoStack.push(command);
    return true;
  }

  canUndo() {
    return this.undoStack.length > 0;
  }

  canRedo() {
    return this.redoStack.length > 0;
  }

  clear() {
    this.undoStack = [];
    this.redoStack = [];
  }
}

// Command classes for different task operations
class AddTaskCommand {
  constructor(taskSystem, task) {
    this.taskSystem = taskSystem;
    this.task = task;
  }

  execute() {
    this.taskSystem.addTaskWithoutHistory(this.task);
  }

  undo() {
    this.taskSystem.removeTaskWithoutHistory(this.task.taskId);
  }
}

class ModifyTaskCommand {
  constructor(taskSystem, taskId, newValues, oldValues) {
    this.taskSystem = taskSystem;
    this.taskId = taskId;
    this.newValues = newValues;
    this.oldValues = oldValues;
  }

  execute() {
    this.taskSystem.modifyTaskWithoutHistory(this.taskId, this.newValues);
  }

  undo() {
    this.taskSystem.modifyTaskWithoutHistory(this.taskId, this.oldValues);
  }
}

class RemoveTaskCommand {
  constructor(taskSystem, task) {
    this.taskSystem = taskSystem;
    this.task = task;
  }

  execute() {
    this.taskSystem.removeTaskWithoutHistory(this.task.taskId);
  }

  undo() {
    this.taskSystem.addTaskWithoutHistory(this.task);
  }
}

class ChangeTaskStatusCommand {
  constructor(taskSystem, taskId, newStatus, oldStatus, oldCompletionDate) {
    this.taskSystem = taskSystem;
    this.taskId = taskId;
    this.newStatus = newStatus;
    this.oldStatus = oldStatus;
    this.oldCompletionDate = oldCompletionDate;
  }

  execute() {
    this.taskSystem.changeTaskStatusWithoutHistory(this.taskId, this.newStatus);
  }

  undo() {
    this.taskSystem.changeTaskStatusWithoutHistory(
      this.taskId,
      this.oldStatus,
      this.oldCompletionDate
    );
  }
}

// Modifications to the existing script
document.addEventListener("DOMContentLoaded", function () {
  // Task data store
  let tasks = [];
  let nextTaskId = 1;

  // Command manager for undo/redo
  const commandManager = new CommandManager();

  // DOM elements
  const tabs = document.querySelectorAll(".tab");
  const tabContents = document.querySelectorAll(".tab-content");
  const addTaskBtn = document.getElementById("addTaskBtn");
  const undoBtn = document.getElementById("undoBtn");
  const redoBtn = document.getElementById("redoBtn");
  const saveTasksBtn = document.getElementById("saveTasksBtn");
  const loadTasksBtn = document.getElementById("loadTasksBtn");
  const taskModal = document.getElementById("taskModal");
  const taskDetailsModal = document.getElementById("taskDetailsModal");
  const changeStatusModal = document.getElementById("changeStatusModal");
  const confirmModal = document.getElementById("confirmModal");
  const taskForm = document.getElementById("taskForm");
  const changeStatusForm = document.getElementById("changeStatusForm");
  const toast = document.getElementById("toast");

  // TaskSystem object to handle operations with and without history
  const taskSystem = {
    // Methods that go through command history
    addTask(task) {
      const command = new AddTaskCommand(this, task);
      commandManager.execute(command);
      this.updateUndoRedoButtons();
      refreshTasks();
      saveTasks();
    },

    modifyTask(taskId, newValues) {
      const taskIndex = tasks.findIndex((t) => t.taskId === taskId);
      if (taskIndex !== -1) {
        const oldValues = { ...tasks[taskIndex] };
        const command = new ModifyTaskCommand(
          this,
          taskId,
          newValues,
          oldValues
        );
        commandManager.execute(command);
        this.updateUndoRedoButtons();
        refreshTasks();
        saveTasks();
      }
    },

    removeTask(taskId) {
      const taskIndex = tasks.findIndex((t) => t.taskId === taskId);
      if (taskIndex !== -1) {
        const task = { ...tasks[taskIndex] };
        const command = new RemoveTaskCommand(this, task);
        commandManager.execute(command);
        this.updateUndoRedoButtons();
        refreshTasks();
        saveTasks();
      }
    },

    changeTaskStatus(taskId, newStatus) {
      const taskIndex = tasks.findIndex((t) => t.taskId === taskId);
      if (taskIndex !== -1) {
        const oldStatus = tasks[taskIndex].taskStatus;
        const oldCompletionDate = tasks[taskIndex].taskCompletionDate;
        const command = new ChangeTaskStatusCommand(
          this,
          taskId,
          newStatus,
          oldStatus,
          oldCompletionDate
        );
        commandManager.execute(command);
        this.updateUndoRedoButtons();
        refreshTasks();
        saveTasks();
      }
    },

    // Methods that bypass command history
    addTaskWithoutHistory(task) {
      tasks.push(task);
      if (task.taskId >= nextTaskId) {
        nextTaskId = task.taskId + 1;
      }
      showToast(`Task "${task.taskName}" added successfully`);
    },

    modifyTaskWithoutHistory(taskId, values) {
      const taskIndex = tasks.findIndex((t) => t.taskId === taskId);
      if (taskIndex !== -1) {
        // Update all fields from values
        Object.keys(values).forEach((key) => {
          tasks[taskIndex][key] = values[key];
        });
        showToast(`Task "${tasks[taskIndex].taskName}" modified successfully`);
      }
    },

    removeTaskWithoutHistory(taskId) {
      const taskIndex = tasks.findIndex((t) => t.taskId === taskId);
      if (taskIndex !== -1) {
        const taskName = tasks[taskIndex].taskName;
        tasks.splice(taskIndex, 1);
        showToast(`Task "${taskName}" removed successfully`);
      }
    },

    changeTaskStatusWithoutHistory(taskId, newStatus, completionDate = null) {
      const taskIndex = tasks.findIndex((t) => t.taskId === taskId);
      if (taskIndex !== -1) {
        const task = tasks[taskIndex];
        task.taskStatus = newStatus;

        // Handle completion date
        if (completionDate !== null) {
          task.taskCompletionDate = completionDate;
        } else if (newStatus === 2 && task.taskCompletionDate === 0) {
          task.taskCompletionDate = Math.floor(Date.now() / 1000);
        } else if (newStatus !== 2) {
          task.taskCompletionDate = 0;
        }

        showToast(`Task status updated successfully`);
      }
    },

    updateUndoRedoButtons() {
      undoBtn.disabled = !commandManager.canUndo();
      redoBtn.disabled = !commandManager.canRedo();
    },
  };

  // Tab switching
  tabs.forEach((tab) => {
    tab.addEventListener("click", function () {
      const tabId = this.getAttribute("data-tab");

      tabs.forEach((t) => t.classList.remove("active"));
      tabContents.forEach((tc) => tc.classList.remove("active"));

      this.classList.add("active");
      document.getElementById(tabId).classList.add("active");

      // Refresh the active tab's content
      refreshTasks();
    });
  });

  // Modal close buttons
  document.querySelectorAll(".close-modal, .close-modal-btn").forEach((btn) => {
    btn.addEventListener("click", function () {
      document.querySelectorAll(".modal").forEach((modal) => {
        modal.style.display = "none";
      });
    });
  });

  // Add task button
  addTaskBtn.addEventListener("click", function () {
    document.getElementById("modalTitle").textContent = "Add New Task";
    document.getElementById("taskId").value = "";
    taskForm.reset();

    // Set default due date to tomorrow
    const tomorrow = new Date();
    tomorrow.setDate(tomorrow.getDate() + 1);
    document.getElementById("taskDueDate").value = formatDateForInput(tomorrow);

    taskModal.style.display = "flex";
  });

  // Initialize undo/redo buttons
  undoBtn.disabled = true;
  redoBtn.disabled = true;

  // Undo button
  undoBtn.addEventListener("click", function () {
    if (commandManager.undo()) {
      taskSystem.updateUndoRedoButtons();
      refreshTasks();
      showToast("Undo operation performed");
    } else {
      showToast("Nothing to undo", true);
    }
  });

  // Redo button
  redoBtn.addEventListener("click", function () {
    if (commandManager.redo()) {
      taskSystem.updateUndoRedoButtons();
      refreshTasks();
      showToast("Redo operation performed");
    } else {
      showToast("Nothing to redo", true);
    }
  });

  // Save tasks button
  saveTasksBtn.addEventListener("click", function () {
    saveTasks();
  });

  // Load tasks button
  loadTasksBtn.addEventListener("click", function () {
    loadTasks();
  });

  // Task form submission
  taskForm.addEventListener("submit", function (e) {
    e.preventDefault();

    const taskId = document.getElementById("taskId").value;
    const taskName = document.getElementById("taskName").value;
    const taskDescription = document.getElementById("taskDescription").value;
    const taskPriority = parseInt(
      document.getElementById("taskPriority").value
    );
    const taskStatus = parseInt(document.getElementById("taskStatus").value);
    const taskDueDate = new Date(document.getElementById("taskDueDate").value);

    if (taskId) {
      // Edit existing task
      const newValues = {
        taskName,
        taskDescription,
        taskPriority,
        taskStatus,
        taskDueDate: taskDueDate.getTime() / 1000,
        taskCompletionDate:
          taskStatus === 2 ? Math.floor(Date.now() / 1000) : 0,
      };
      taskSystem.modifyTask(parseInt(taskId), newValues);
    } else {
      // Add new task
      const task = {
        taskId: nextTaskId++,
        taskName,
        taskDescription,
        taskStatus,
        taskPriority,
        taskDueDate: taskDueDate.getTime() / 1000,
        taskCreationDate: Math.floor(Date.now() / 1000),
        taskCompletionDate:
          taskStatus === 2 ? Math.floor(Date.now() / 1000) : 0,
      };
      taskSystem.addTask(task);
    }

    taskModal.style.display = "none";
  });

  // Change status form submission
  changeStatusForm.addEventListener("submit", function (e) {
    e.preventDefault();

    const taskId = parseInt(document.getElementById("statusTaskId").value);
    const newStatus = parseInt(document.getElementById("newStatus").value);

    taskSystem.changeTaskStatus(taskId, newStatus);
    changeStatusModal.style.display = "none";
  });

  // Save tasks to localStorage (simulating file storage)
  function saveTasks() {
    try {
      localStorage.setItem("tasks", JSON.stringify(tasks));
      localStorage.setItem("nextTaskId", nextTaskId.toString());
      showToast("Tasks saved successfully");
    } catch (error) {
      showToast("Failed to save tasks", true);
      console.error("Error saving tasks:", error);
    }
  }

  // Load tasks from localStorage (simulating file loading)
  function loadTasks() {
    try {
      const savedTasks = localStorage.getItem("tasks");
      const savedNextTaskId = localStorage.getItem("nextTaskId");

      if (savedTasks && savedNextTaskId) {
        tasks = JSON.parse(savedTasks);
        nextTaskId = parseInt(savedNextTaskId);
        // Clear command history when loading tasks
        commandManager.clear();
        taskSystem.updateUndoRedoButtons();
        showToast(`Loaded ${tasks.length} tasks successfully`);
        refreshTasks();
      } else {
        showToast("No saved tasks found", true);
      }
    } catch (error) {
      showToast("Failed to load tasks", true);
      console.error("Error loading tasks:", error);
    }
  }

  // Update task counts in summary cards
  function updateTaskCounts() {
    document.getElementById("totalTasksCount").textContent = tasks.length;
    document.getElementById("pendingTasksCount").textContent = tasks.filter(
      (t) => t.taskStatus === 0
    ).length;
    document.getElementById("inProgressTasksCount").textContent = tasks.filter(
      (t) => t.taskStatus === 1
    ).length;
    document.getElementById("completedTasksCount").textContent = tasks.filter(
      (t) => t.taskStatus === 2
    ).length;
  }

  // Refresh task lists
  function refreshTasks() {
    updateTaskCounts();

    // All tasks
    const allTasksBody = document.getElementById("all-tasks-body");
    allTasksBody.innerHTML = "";
    tasks.forEach((task) => {
      allTasksBody.appendChild(createTaskRow(task));
    });

    // Pending tasks
    const pendingTasksBody = document.getElementById("pending-tasks-body");
    pendingTasksBody.innerHTML = "";
    tasks
      .filter((t) => t.taskStatus === 0)
      .forEach((task) => {
        pendingTasksBody.appendChild(createTaskRow(task, false));
      });

    // In progress tasks
    const inProgressTasksBody = document.getElementById(
      "in-progress-tasks-body"
    );
    inProgressTasksBody.innerHTML = "";
    tasks
      .filter((t) => t.taskStatus === 1)
      .forEach((task) => {
        inProgressTasksBody.appendChild(createTaskRow(task, false));
      });

    // Completed tasks
    const completedTasksBody = document.getElementById("completed-tasks-body");
    completedTasksBody.innerHTML = "";
    tasks
      .filter((t) => t.taskStatus === 2)
      .forEach((task) => {
        completedTasksBody.appendChild(createCompletedTaskRow(task));
      });

    // Priority view (sorted by priority descending)
    const priorityTasksBody = document.getElementById("priority-tasks-body");
    priorityTasksBody.innerHTML = "";
    [...tasks]
      .sort((a, b) => b.taskPriority - a.taskPriority)
      .forEach((task) => {
        priorityTasksBody.appendChild(createTaskRow(task));
      });
  }

  // Create a task row for the tables
  function createTaskRow(task, includeStatus = true) {
    const tr = document.createElement("tr");

    // ID cell
    const idCell = document.createElement("td");
    idCell.textContent = task.taskId;
    tr.appendChild(idCell);

    // Name cell
    const nameCell = document.createElement("td");
    nameCell.textContent = task.taskName;
    nameCell.style.cursor = "pointer";
    nameCell.style.color = "#4a6fa5";
    nameCell.style.textDecoration = "underline";
    nameCell.addEventListener("click", () => showTaskDetails(task));
    tr.appendChild(nameCell);

    // Priority cell
    const priorityCell = document.createElement("td");
    const priorityBadge = document.createElement("span");
    priorityBadge.className = `priority-badge priority-${task.taskPriority}`;
    priorityBadge.textContent = task.taskPriority;
    priorityCell.appendChild(priorityBadge);
    tr.appendChild(priorityCell);

    // Status cell (optional)
    if (includeStatus) {
      const statusCell = document.createElement("td");
      const statusBadge = document.createElement("span");
      statusBadge.className = `status-badge status-${task.taskStatus}`;
      statusBadge.textContent =
        task.taskStatus === 0
          ? "Pending"
          : task.taskStatus === 1
          ? "In Progress"
          : "Completed";
      statusCell.appendChild(statusBadge);
      tr.appendChild(statusCell);
    }

    // Due date cell
    const dueDateCell = document.createElement("td");
    dueDateCell.textContent = formatDate(task.taskDueDate);
    tr.appendChild(dueDateCell);

    // Actions cell
    const actionsCell = document.createElement("td");
    const actionsDiv = document.createElement("div");
    actionsDiv.className = "task-actions";

    // Edit button
    const editBtn = document.createElement("button");
    editBtn.className = "action-btn btn-info";
    editBtn.textContent = "Edit";
    editBtn.addEventListener("click", () => editTask(task));
    actionsDiv.appendChild(editBtn);

    // Status button
    const statusBtn = document.createElement("button");
    statusBtn.className = "action-btn btn-warning";
    statusBtn.textContent = "Status";
    statusBtn.addEventListener("click", () => showChangeStatusModal(task));
    actionsDiv.appendChild(statusBtn);

    // Delete button
    const deleteBtn = document.createElement("button");
    deleteBtn.className = "action-btn btn-danger";
    deleteBtn.textContent = "Delete";
    deleteBtn.addEventListener("click", () => confirmDeleteTask(task));
    actionsDiv.appendChild(deleteBtn);

    actionsCell.appendChild(actionsDiv);
    tr.appendChild(actionsCell);

    return tr;
  }

  // Create a completed task row for the tables
  function createCompletedTaskRow(task) {
    const tr = document.createElement("tr");

    // ID cell
    const idCell = document.createElement("td");
    idCell.textContent = task.taskId;
    tr.appendChild(idCell);

    // Name cell
    const nameCell = document.createElement("td");
    nameCell.textContent = task.taskName;
    nameCell.style.cursor = "pointer";
    nameCell.style.color = "#4a6fa5";
    nameCell.style.textDecoration = "underline";
    nameCell.addEventListener("click", () => showTaskDetails(task));
    tr.appendChild(nameCell);

    // Priority cell
    const priorityCell = document.createElement("td");
    const priorityBadge = document.createElement("span");
    priorityBadge.className = `priority-badge priority-${task.taskPriority}`;
    priorityBadge.textContent = task.taskPriority;
    priorityCell.appendChild(priorityBadge);
    tr.appendChild(priorityCell);

    // Completion date cell
    const completionDateCell = document.createElement("td");
    completionDateCell.textContent = formatDate(task.taskCompletionDate);
    tr.appendChild(completionDateCell);

    // Actions cell
    const actionsCell = document.createElement("td");
    const actionsDiv = document.createElement("div");
    actionsDiv.className = "task-actions";

    // Edit button
    const editBtn = document.createElement("button");
    editBtn.className = "action-btn btn-info";
    editBtn.textContent = "Edit";
    editBtn.addEventListener("click", () => editTask(task));
    actionsDiv.appendChild(editBtn);

    // Delete button
    const deleteBtn = document.createElement("button");
    deleteBtn.className = "action-btn btn-danger";
    deleteBtn.textContent = "Delete";
    deleteBtn.addEventListener("click", () => confirmDeleteTask(task));
    actionsDiv.appendChild(deleteBtn);

    actionsCell.appendChild(actionsDiv);
    tr.appendChild(actionsCell);

    return tr;
  }

  // Edit task
  function editTask(task) {
    document.getElementById("modalTitle").textContent = "Edit Task";
    document.getElementById("taskId").value = task.taskId;
    document.getElementById("taskName").value = task.taskName;
    document.getElementById("taskDescription").value = task.taskDescription;
    document.getElementById("taskPriority").value = task.taskPriority;
    document.getElementById("taskStatus").value = task.taskStatus;
    document.getElementById("taskDueDate").value = formatDateForInput(
      new Date(task.taskDueDate * 1000)
    );

    taskModal.style.display = "flex";
  }

  // Show change status modal
  function showChangeStatusModal(task) {
    document.getElementById("statusTaskId").value = task.taskId;
    document.getElementById("newStatus").value = task.taskStatus;

    changeStatusModal.style.display = "flex";
  }

  // Confirm delete task
  function confirmDeleteTask(task) {
    document.getElementById(
      "confirmMessage"
    ).textContent = `Are you sure you want to delete task "${task.taskName}"?`;

    const confirmActionBtn = document.getElementById("confirmActionBtn");
    // Remove any existing event listeners
    const newBtn = confirmActionBtn.cloneNode(true);
    confirmActionBtn.parentNode.replaceChild(newBtn, confirmActionBtn);

    // Add new event listener
    newBtn.addEventListener("click", function () {
      taskSystem.removeTask(task.taskId);
      confirmModal.style.display = "none";
    });

    confirmModal.style.display = "flex";
  }

  // Show task details
  function showTaskDetails(task) {
    const content = document.getElementById("taskDetailsContent");

    let statusText = "Pending";
    if (task.taskStatus === 1) statusText = "In Progress";
    else if (task.taskStatus === 2) statusText = "Completed";

    let completionInfo = "";
    if (task.taskStatus === 2) {
      completionInfo = `
          <p><strong>Completion Date:</strong> ${formatDate(
            task.taskCompletionDate
          )}</p>
        `;
    }

    content.innerHTML = `
        <h3>${task.taskName}</h3>
        <p><strong>ID:</strong> ${task.taskId}</p>
        <p><strong>Description:</strong></p>
        <p>${task.taskDescription}</p>
        <p><strong>Priority:</strong> ${task.taskPriority}</p>
        <p><strong>Status:</strong> ${statusText}</p>
        <p><strong>Due Date:</strong> ${formatDate(task.taskDueDate)}</p>
        <p><strong>Creation Date:</strong> ${formatDate(
          task.taskCreationDate
        )}</p>
        ${completionInfo}
        <div class="form-actions">
          <button class="btn btn-primary edit-task-btn">Edit Task</button>
          <button class="btn btn-warning change-status-btn">Change Status</button>
        </div>
      `;

    // Add event listeners to buttons
    content
      .querySelector(".edit-task-btn")
      .addEventListener("click", function () {
        taskDetailsModal.style.display = "none";
        editTask(task);
      });

    content
      .querySelector(".change-status-btn")
      .addEventListener("click", function () {
        taskDetailsModal.style.display = "none";
        showChangeStatusModal(task);
      });

    taskDetailsModal.style.display = "flex";
  }

  // Show toast notification
  function showToast(message, isError = false) {
    toast.textContent = message;
    toast.className = isError ? "toast error show" : "toast show";

    setTimeout(() => {
      toast.className = toast.className.replace("show", "");
    }, 3000);
  }

  // Format date for display
  function formatDate(timestamp) {
    const date = new Date(timestamp * 1000);
    return date.toLocaleDateString();
  }

  // Format date for input field
  function formatDateForInput(date) {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, "0");
    const day = String(date.getDate()).padStart(2, "0");
    return `${year}-${month}-${day}`;
  }

  // Initialize app - load tasks or create sample tasks
  function initApp() {
    // Try to load tasks from storage
    const savedTasks = localStorage.getItem("tasks");
    const savedNextTaskId = localStorage.getItem("nextTaskId");

    if (savedTasks && savedNextTaskId) {
      tasks = JSON.parse(savedTasks);
      nextTaskId = parseInt(savedNextTaskId);
    } else {
      // Create sample tasks
      const today = new Date();
      const tomorrow = new Date();
      tomorrow.setDate(tomorrow.getDate() + 1);
      const nextWeek = new Date();
      nextWeek.setDate(nextWeek.getDate() + 7);

      // Sample tasks
      const task1 = {
        taskId: nextTaskId++,
        taskName: "Complete project proposal",
        taskDescription:
          "Write a detailed proposal for the new client project including timeline and budget estimates.",
        taskStatus: 0, // Pending
        taskPriority: 4, // High priority
        taskDueDate: tomorrow.getTime() / 1000,
        taskCreationDate: Math.floor(Date.now() / 1000),
        taskCompletionDate: 0,
      };

      const task2 = {
        taskId: nextTaskId++,
        taskName: "Team meeting preparation",
        taskDescription:
          "Prepare slides and agenda for the weekly team meeting.",
        taskStatus: 1, // In Progress
        taskPriority: 3, // Medium priority
        taskDueDate: today.getTime() / 1000,
        taskCreationDate: Math.floor(Date.now() / 1000),
        taskCompletionDate: 0,
      };

      const task3 = {
        taskId: nextTaskId++,
        taskName: "Research new technologies",
        taskDescription:
          "Research and document potential new technologies for the upcoming development phase.",
        taskStatus: 0, // Pending
        taskPriority: 2, // Low priority
        taskDueDate: nextWeek.getTime() / 1000,
        taskCreationDate: Math.floor(Date.now() / 1000),
        taskCompletionDate: 0,
      };

      tasks.push(task1, task2, task3);
    }

    // Initially disable undo/redo buttons
    taskSystem.updateUndoRedoButtons();

    // Refresh task lists
    refreshTasks();
  }

  // Initialize the app
  initApp();
});
