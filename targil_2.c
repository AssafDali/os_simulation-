#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define MAX_PROCESSES 10
#define MAX_FILES 5
#define MEMORY_SIZE 1024
#define TIME_QUANTUM 3
#define IO_WAIT_TIME 3 
 
enum State 
{
    READY, RUNNING, WAITING, FINISHED
};
struct Process 
{
    int pid;
    enum State state;
    int total_time;
    int time_left;
    int memory_required;
    int mem_address;
    char name[30];
    int io_wait_remaining; 
    int has_requested_io;
};
 
struct File 
{
    char name[50];
    char data[200];
    int size;
    int is_used;
};
 
struct Process processes[MAX_PROCESSES];
struct File files[MAX_FILES];
int process_count = 0;
int memory_used = 0;
int next_pid = 0;
 
void show_menu();
void handle_process_menu(); 
void handle_file_menu();
void handle_memory_status();
void handle_io_menu();
int allocate_memory(int size);
void free_memory(int size);
int create_process(char *name, int time, int mem);
void schedule_processes();
void print_processes();
void init_files();
int create_file(char *name);
int write_file(char *name, char *content);
int read_file(char *name);
 
int main() 
{
    init_files();
    printf("OS Simulation - \n");
    printf("Total Memory: %dKB\n", MEMORY_SIZE);
    printf("Time Quantum: %d units\n", TIME_QUANTUM);
    printf("Max Processes: %d\n", MAX_PROCESSES);
    printf("Max Files: %d\n\n", MAX_FILES);
    
    int choice;
    
    while (1) 
    {
        show_menu();
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) 
        {
            case 1:
                handle_process_menu();
                break;
            case 2:
                handle_file_menu();
                break;
            case 3:
                handle_memory_status();
                break;
            case 4:
                handle_io_menu();
                break;
            case 5:
                printf("Shutting down OS...\n");
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
        printf("\n");
    }
    return 0;
}
 
void show_menu() 
{
    printf("MAIN MENU - \n");
    printf("1.Process Management\n");
    printf("2.File System\n");
    printf("3.Memory Status\n");
    printf("4.I/O Operations\n");
    printf("5.Exit\n");
}
 
void handle_process_menu() 
{
    int choice;
    printf("Process Management -\n");
    printf("1. Create Process\n");
    printf("2. View All Processes\n");
    printf("3. Run Scheduler\n");
    printf("4. Back\n");
    printf("Choice: ");
    scanf("%d", &choice);
    getchar();
    if (choice == 1) 
    {
        char name[30];
        int time_needed, memory_needed;
        
        printf("\nEnter process name: ");
        fgets(name, 30, stdin);
        name[strcspn(name, "\n")] = 0;
        printf("Enter execution time: ");
        scanf("%d", &time_needed);
        printf("Enter memory needed (KB): ");
        scanf("%d", &memory_needed);
        getchar();
        create_process(name, time_needed, memory_needed);
    }
    else if (choice == 2) 
    {
        print_processes();
    }
    else if (choice == 3) 
    {
        if (process_count == 0) 
        {
            printf("No processes to schedule!\n");
        } 
        else 
        {
            schedule_processes();
        }
    }
}
 
void handle_file_menu() 
{
    int choice;
    printf("File System -\n");
    printf("1. Create File\n");
    printf("2. Write to File\n");
    printf("3. Read File\n");
    printf("4. Back\n");
    printf("Choice: ");
    scanf("%d", &choice);
    getchar();
    
    char filename[50];
    char content[200];
    
    if (choice == 1) 
    {
        printf("\nEnter filename: ");
        fgets(filename, 50, stdin);
        filename[strcspn(filename, "\n")] = 0;
        create_file(filename);
    }
    else if (choice == 2) 
    {
        printf("\nEnter filename: ");
        fgets(filename, 50, stdin);
        filename[strcspn(filename, "\n")] = 0;
        
        printf("Enter content: ");
        fgets(content, 200, stdin);
        content[strcspn(content, "\n")] = 0;
        
        write_file(filename, content);
    }
    else if (choice == 3) 
    {
        printf("\nEnter filename: ");
        fgets(filename, 50, stdin);
        filename[strcspn(filename, "\n")] = 0;
        read_file(filename);
    }
}
 
void handle_memory_status() 
{
    printf("--- Memory Status ---\n");
    printf("Total Memory: %dKB\n", MEMORY_SIZE);
    printf("Used Memory: %dKB\n", memory_used);
    printf("Free Memory: %dKB\n", MEMORY_SIZE - memory_used);
}
 
void handle_io_menu() 
{
    int choice;
    printf("I/O Operations -\n");
    printf("1. Write Output\n");
    printf("2. Read Input\n");
    printf("3. Back\n");
    printf("Choice: ");
    scanf("%d", &choice);
    getchar();
    if (choice == 1) 
    {
        char msg[100];
        printf("\nEnter message to output: ");
        fgets(msg, 100, stdin);
        msg[strcspn(msg, "\n")] = 0;
        printf("[OUTPUT] %s\n", msg);
    }
    else if (choice == 2) 
    {
        char input[100];
        printf("\nEnter input: ");
        fgets(input, 100, stdin);
        input[strcspn(input, "\n")] = 0;
        printf("[INPUT RECEIVED] %s\n", input);
    }
}
 
int allocate_memory(int size) 
{
    if (memory_used + size > MEMORY_SIZE) 
    {
        printf("ERROR: Not enough memory! (Need %dKB, Available %dKB)\n", size, MEMORY_SIZE - memory_used);
        return -1;
    }
    int addr = memory_used;
    memory_used += size;
    printf("Memory allocated: %dKB at address %d\n", size, addr);
    return addr;
}
 
void free_memory(int size) 
{
    memory_used = memory_used - size;
    printf("Memory freed: %dKB\n", size);
}
 
int create_process(char *name, int time_needed, int memory_needed) 
{
    if (process_count >= MAX_PROCESSES) 
    {
        printf("ERROR: Maximum number of processes reached!\n");
        return -1;
    }
    
    int addr = allocate_memory(memory_needed);
    if (addr == -1) 
    {
        return -1;
    }
    
    processes[process_count].pid = next_pid++;
    processes[process_count].state = READY;
    processes[process_count].total_time = time_needed;
    processes[process_count].time_left = time_needed;
    processes[process_count].memory_required = memory_needed;
    processes[process_count].mem_address = addr;
    strcpy(processes[process_count].name, name);
    processes[process_count].io_wait_remaining = 0; 
    processes[process_count].has_requested_io = 0;
    
    printf("Process created successfully!\n");
    printf("PID: %d, Name: %s, Time: %d, Memory: %dKB\n",processes[process_count].pid, name, time_needed, memory_needed);
    process_count++;
    return process_count - 1;
}
 
void print_processes() 
{
    if (process_count == 0) 
    {
        printf("No processes in the system.\n");
        return;
    }
    printf("\nProcess List -\n");
    for (int i = 0; i < process_count; i++) 
    {
        char *status;
        switch(processes[i].state) 
        {
           case READY: status = "READY"; break;
           case RUNNING: status = "RUNNING"; break;
           case WAITING: status = "WAITING"; break;
           case FINISHED: status = "FINISHED"; break;
           default: status = "UNKNOWN";
        }
        
        printf("PID: %d | Name: %s | State: %s | Burst: %d | Memory: %dKB\n",processes[i].pid, processes[i].name, status, processes[i].total_time, 
                processes[i].memory_required);     
    }
}
 
void schedule_processes() 
{
    printf("\nRound Robin Scheduler:\n");
    printf("Time Quantum: %d\n", TIME_QUANTUM);
    int has_unfinished = 0;
    for (int i = 0; i < process_count; i++)
    {
        if (processes[i].state != FINISHED) 
        {
            has_unfinished = 1;
            break;
        }
    }
    if (!has_unfinished)
    {
        printf("All processes finished!\n");
        return;
    }
    int cycle = 1;
    int all_done = 0;
    while (!all_done)
    {
        all_done = 1;
        printf("-- Cycle %d --\n", cycle);
        
        for (int i = 0; i < process_count; i++) 
        {
            if (processes[i].state == WAITING)
            {
                processes[i].io_wait_remaining--;
                printf("Process [%s] PID:%d waiting for I/O (%d cycles remaining)\n",processes[i].name, processes[i].pid, processes[i].io_wait_remaining);
                if (processes[i].io_wait_remaining <= 0)
                {
                    processes[i].state = READY;
                    printf("Process [%s] PID:%d finished I/O, back to ready\n", processes[i].name, processes[i].pid);
                }
            }
        }
        for (int i = 0; i < process_count; i++) 
        {
            if (processes[i].state == FINISHED) 
            {
                continue;
            }
            all_done = 0;
            if (processes[i].state == WAITING) 
            {
                continue; 
            }
            if (processes[i].state == READY && processes[i].time_left == processes[i].total_time / 2 && processes[i].has_requested_io == 0) 
            {
                processes[i].state = WAITING;
                processes[i].io_wait_remaining = IO_WAIT_TIME;
                processes[i].has_requested_io = 1;
                printf("Process [%s] PID:%d starting I/O, will wait %d cycles\n", processes[i].name, processes[i].pid, IO_WAIT_TIME);
                continue;
            }
            processes[i].state = RUNNING;
            int run_time;
            
            if (processes[i].time_left < TIME_QUANTUM)
            {
                run_time = processes[i].time_left;
            } 
            else 
            {
                run_time = TIME_QUANTUM;
            }
            printf("Running [%s] PID:%d for %d units...\n", processes[i].name, processes[i].pid, run_time);
            processes[i].time_left = processes[i].time_left - run_time;
            if (processes[i].time_left == 0) 
            {
                processes[i].state = FINISHED;
                printf("FINISHED\n");
                free_memory(processes[i].memory_required);
            } 
            else 
            {
                processes[i].state = READY;
            }
        }
        cycle++;
    }
    printf("All Processes Completed!!\n");
    process_count = 0; 
    next_pid = 0;
}
 
void init_files() 
{
    for (int i = 0; i < MAX_FILES; i++) 
    {
        files[i].is_used = 0;
    }
}
 
int create_file(char *name) 
{
    for (int i = 0; i < MAX_FILES; i++) 
    {
        if (files[i].is_used && strcmp(files[i].name, name) == 0)
        {
            printf("ERROR: File '%s' already exists!\n", name);
            return -1;
        }
    }
    for (int i = 0; i < MAX_FILES; i++) 
    {
        if (files[i].is_used == 0) 
        {
            strcpy(files[i].name, name);
            files[i].data[0] = '\0';
            files[i].size = 0;
            files[i].is_used = 1;
            printf("File created: %s\n", name);
            return i;
        }
    }
    printf("ERROR: No space for new files!\n");
    return -1;
}
 
int write_file(char *name, char *content) 
{
    for (int i = 0; i < MAX_FILES; i++) 
    {
        if (files[i].is_used && strcmp(files[i].name, name) == 0) 
        {
            strcpy(files[i].data, content);
            files[i].size = strlen(content);
            printf("Written to '%s'\n", name);
            return 0;
        }
    }
    printf("ERROR: File '%s' not found!\n", name);
    return -1;
}
 
int read_file(char *name) 
{
    for (int i = 0; i < MAX_FILES; i++) 
    {
        if (files[i].is_used && strcmp(files[i].name, name) == 0)
        {
            printf("File: %s\n", name);
            printf("%s\n", files[i].data);
            printf("Size: %d bytes \n", files[i].size);
            return 0;
        }
    }
    printf("ERROR: File '%s' not found!\n", name);
    return -1;
}
