#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "raylib.h"
#include "raygui.h"

// Struct for task representation
typedef struct task {
    int id;                
    char d_id[300];        
    char desc[300];        
    int p_lvl;             
    int status;            // Status: 0 = pending, 1 = in progress, 2 = completed
    struct task *pnext;    
} task;

// Function declarations
task *insertion_gui(task *first);
task *deletion_gui(task *first);
task *upstat_gui(task *first);
void display_tasks_gui(task *first);
void search_by_priority_gui(task *first);


void to_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Main function
int main() {
    InitWindow(800, 600, "Task Manager");
    SetTargetFPS(60);

    task *taskList = NULL; // Head of the task linked list
    int currentMenuOption = -1;
    bool exitProgram = false;

    while (!exitProgram && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentMenuOption == -1) {
            // Main Menu
            GuiLabel((Rectangle){20, 20, 300, 30}, "====== Task Manager ======");

            if (GuiButton((Rectangle){20, 60, 200, 30}, "Insert a New Task")) currentMenuOption = 1;
            if (GuiButton((Rectangle){20, 100, 200, 30}, "Delete a Task by ID")) currentMenuOption = 2;
            if (GuiButton((Rectangle){20, 140, 200, 30}, "Update Task Status")) currentMenuOption = 3;
            if (GuiButton((Rectangle){20, 180, 200, 30}, "Display All Tasks")) currentMenuOption = 4;
            if (GuiButton((Rectangle){20, 220, 200, 30}, "Search Tasks by Priority")) currentMenuOption = 5;
            if (GuiButton((Rectangle){20, 260, 200, 30}, "Exit")) exitProgram = true;
        } else {
            // Sub-GUI based on selected menu option
            switch (currentMenuOption) {
                case 1: taskList = insertion_gui(taskList); break;
                case 2: taskList = deletion_gui(taskList); break;
                case 3: taskList = upstat_gui(taskList); break;
                case 4: display_tasks_gui(taskList); break;
                case 5: search_by_priority_gui(taskList); break;
                default: break;
            }

            // Add a "Back" button to return to the main menu
            if (GuiButton((Rectangle){20, 520, 100, 30}, "Back")) currentMenuOption = -1;
        }

        EndDrawing();
    }


    CloseWindow();
    return 0;
}

// GUI for inserting a new task
task *insertion_gui(task *first) {
    static char d_id[300] = "";
    static char desc[300] = "";
    static int priority = 1;             // Priority as an integer
    static int activePriority = 0;       
    static bool dropdownEditMode = false; // Tracks whether the dropdown is in edit mode
    static bool edit_id = false;
    static bool edit_desc = false;

    static bool initialized = false;
    if (!initialized) {
        d_id[0] = '\0';
        desc[0] = '\0';
        priority = 1;        // Default priority is 1
        activePriority = 0;  // Default dropdown selection (corresponds to "1")
        initialized = true;
    }

    GuiLabel((Rectangle){20, 20, 300, 30}, "Insert New Task");

    GuiLabel((Rectangle){20, 60, 100, 20}, "Task Name:");
    if (GuiTextBox((Rectangle){130, 60, 300, 30}, d_id, sizeof(d_id), edit_id)) {
        edit_id = !edit_id;
    }

    GuiLabel((Rectangle){20, 110, 100, 20}, "Description:");
    if (GuiTextBox((Rectangle){130, 110, 300, 30}, desc, sizeof(desc), edit_desc)) {
        edit_desc = !edit_desc;
    }

    GuiLabel((Rectangle){20, 160, 100, 20}, "Priority:");
    const char *priorityOptions = "1;2;3;4;5";

    // Dropdown with toggleable edit mode
    if (GuiDropdownBox((Rectangle){130, 160, 300, 30}, priorityOptions, &activePriority, dropdownEditMode)) {
        dropdownEditMode = !dropdownEditMode; // Toggle edit mode when clicked
    }
    priority = activePriority + 1; // Map the dropdown selection to a priority value (1-5)

    if (GuiButton((Rectangle){20, 210, 100, 30}, "Add Task")) {
        if (strlen(d_id) > 0 && strlen(desc) > 0) {
            task *new_task = (task *)malloc(sizeof(task));
            if (new_task) {
                to_uppercase(d_id); // Convert task name to uppercase
                strcpy(new_task->d_id, d_id);
                strcpy(new_task->desc, desc);
                new_task->p_lvl = priority; // Assign selected priority
                new_task->status = 0;
                new_task->pnext = NULL;

                if (!first) {
                    first = new_task; // Add as the first task if list is empty
                } else {
                    task *p = first;
                    task *q = p->pnext;

                    // Insert based on priority (descending order)
                    if (new_task->p_lvl >= p->p_lvl) {
                        new_task->pnext = p;
                        first = new_task;
                    } else {
                        while (q && new_task->p_lvl < q->p_lvl) {
                            p = q;
                            q = q->pnext;
                        }
                        new_task->pnext = q;
                        p->pnext = new_task;
                    }
                }
            }

            // Reset input fields and dropdown for the next task
            d_id[0] = '\0';
            desc[0] = '\0';
            priority = 1;
            activePriority = 0;   
            dropdownEditMode = false; 
            initialized = false;   // Reset GUI for next use
        }
    }

    return first;
}


// GUI for deleting a task
task *deletion_gui(task *first) {
    static char del_id[300] = "";
    static bool edit_id = false;

    static bool initialized = false;
    if (!initialized) {
        del_id[0] = '\0';
        initialized = true;
    }

    GuiLabel((Rectangle){20, 20, 300, 30}, "Delete Task");

    GuiLabel((Rectangle){20, 60, 100, 20}, "Task Name:");
    if (GuiTextBox((Rectangle){130, 60, 300, 30}, del_id, sizeof(del_id), edit_id)) {
        edit_id = !edit_id;
    }

    if (GuiButton((Rectangle){20, 110, 100, 30}, "Delete")) {
        if (strlen(del_id) > 0) {
            to_uppercase(del_id);

            task *p = first, *q = NULL;

            if (p && strcmp(p->d_id, del_id) == 0) {
                first = p->pnext;
                free(p);
            } else {
                while (p && strcmp(p->d_id, del_id) != 0) {
                    q = p;
                    p = p->pnext;
                }

                if (p) {
                    q->pnext = p->pnext;
                    free(p);
                }
            }
            del_id[0] = '\0';
            initialized = false; // Reset GUI for next use
        }
    }

    return first;
}

// GUI for updating task status
task *upstat_gui(task *first) {
    static char up_id[300] = "";
    static bool edit_id = false;

    static bool initialized = false;
    if (!initialized) {
        up_id[0] = '\0';
        initialized = true;
    }

    GuiLabel((Rectangle){20, 20, 300, 30}, "Update Task Status");

    GuiLabel((Rectangle){20, 60, 100, 20}, "Task Name:");
    if (GuiTextBox((Rectangle){130, 60, 300, 30}, up_id, sizeof(up_id), edit_id)) {
        edit_id = !edit_id;
    }

    if (GuiButton((Rectangle){20, 110, 100, 30}, "Update")) {
        if (strlen(up_id) > 0) {
            to_uppercase(up_id);
            task *p = first;

            while (p) {
                if (strcmp(p->d_id, up_id) == 0) {
                    p->status = (p->status + 1) % 3;
                    break;
                }
                p = p->pnext;
            }
            up_id[0] = '\0';
            initialized = false; // Reset GUI for next use
        }
    }

    return first;
}


// GUI for displaying all tasks
void display_tasks_gui(task *first) {


    GuiLabel((Rectangle){20, 20, 300, 30}, "Task List");

    int y_offset = 60;
    task *p = first;

    while (p) {
        char buffer[512];
        snprintf(buffer, sizeof(buffer), "Task: %s\nPriority: %d\nStatus: %s\nDescription: %s",
                 p->d_id, p->p_lvl, (p->status == 0 ? "Pending" : p->status == 1 ? "In Progress" : "Completed"), p->desc);

        GuiLabel((Rectangle){20, y_offset, 760, 60}, buffer);
        y_offset += 70;
        p = p->pnext;
    }

    
}

// GUI for searching tasks by priority
void search_by_priority_gui(task *first) {
    static int activePriority = 0;    
    static bool dropdownEditMode = false; // Tracks whether the dropdown is in edit mode
    static bool taskFound = false;   
    static char resultBuffer[10][512]; // Buffer to store up to 10 task descriptions
    static int resultCount = 0;      // Number of tasks found

    GuiLabel((Rectangle){20, 20, 300, 30}, "Search by Priority");

    GuiLabel((Rectangle){20, 60, 100, 20}, "Priority:");
    const char *priorityOptions = "1;2;3;4;5";

    
    if (GuiDropdownBox((Rectangle){130, 60, 300, 30}, priorityOptions, &activePriority, dropdownEditMode)) {
        dropdownEditMode = !dropdownEditMode; // Toggle edit mode when clicked (this was so hard to do )
    }

    int priority = activePriority + 1;

    if (GuiButton((Rectangle){20, 110, 100, 30}, "Search")) {
        task *p = first;
        int y_offset = 160;

        //this is te kind of refresh results
        resultCount = 0;
        taskFound = false;

        while (p && resultCount < 10) { //I LMITED it to 10 results brk
            if (p->p_lvl == priority) {
                snprintf(resultBuffer[resultCount], sizeof(resultBuffer[resultCount]),
                         "Task: %s\nPriority: %d\nDescription: %s", p->d_id, p->p_lvl, p->desc);
                resultCount++;
                taskFound = true;
            }
            p = p->pnext;
        }

        if (!taskFound) {
            snprintf(resultBuffer[0], sizeof(resultBuffer[0]), "No tasks found with priority %d.", priority);
            resultCount = 1;
        }
    }

    // Display search results
    int y_offset = 160;
    for (int i = 0; i < resultCount; i++) {
        GuiLabel((Rectangle){20, y_offset, 760, 60}, resultBuffer[i]);
        y_offset += 70;
    }
}