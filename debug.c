#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <form.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/mman.h>
#include "stack.h"
#include "value.h"
#include "mem.h"
#include "vm.h"
#include "exec.h"
#include "debug.h"

#define BREAKPOINTS_FILENAME "script/breakpoints"

// from https://stackoverflow.com/questions/19172541/procs-fork-and-mutexes
// to be used to synchonize writing to the GUI by multiple processes
typedef struct
{
  bool done;
  bool debug_input_mode; // true when accepting input for the debugger (not the VM)
  pthread_mutex_t mutex;
} shared_data;

static shared_data* data = NULL;

WINDOW * reg_win = NULL;
WINDOW * stk_win = NULL;
WINDOW * pc_win = NULL;
WINDOW * out_win = NULL;

vm_t * vm = NULL;

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

// from https://stackoverflow.com/questions/19172541/procs-fork-and-mutexes 
void initialise_shared()
{
    // place our shared data in shared memory
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    data = mmap(NULL, sizeof(shared_data), prot, flags, -1, 0);
    assert(data);

    data->done = false;
    data->debug_input_mode = false;

    // initialise mutex so it works properly in shared memory
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
}

void init_windows(){
    assert(vm);
    int startx, starty, width, height;

    //registers
    startx = 0;
    starty = 0;
    width = COLS / 3;
    height = LINES/2;
    reg_win = newwin(height, width, starty, startx);
    box(reg_win, 0 , 0);
    mvwprintw(reg_win, 1, (getmaxx(reg_win)/2)-5, "Registers");
    mvwprintw(reg_win, 2, (getmaxx(reg_win)/2)-6, "-----------");
    for(int i = 0; i < NUM_REGS; i++){
        mvwprintw(reg_win, 4+(2*i), 4, "r%d -> %" PRIu16 "",i,vm->regs[i]);
    }
    wrefresh(reg_win);

    //stack
    startx = COLS / 3;
    starty = 0;
    width = COLS / 3;
    height = LINES/2;
    stk_win = newwin(height, width, starty, startx);
    box(stk_win, 0 , 0);
    mvwprintw(stk_win, 1, (getmaxx(reg_win)/2)-2, "Stack");
    mvwprintw(stk_win, 2, (getmaxx(reg_win)/2)-5, "-----------");
    wrefresh(stk_win);

    //program counter
    startx = 2 * (COLS / 3);
    starty = 0;
    width = COLS / 3;
    height = LINES/2;
    pc_win = newwin(height, width, starty, startx);
    box(pc_win, 0 , 0);
    mvwprintw(pc_win, 1, (getmaxx(reg_win)/2)-7, "Program Counter");
    mvwprintw(pc_win, 2, (getmaxx(reg_win)/2)-8, "-----------------");
    wrefresh(pc_win);

    //output buffer
    startx = 0;
    starty = LINES/2;
    width = COLS;
    height = LINES/2;
    out_win = newwin(height, width, starty, startx);
    scrollok(out_win,TRUE);
    //idlok(out_win,TRUE);
}

void refresh_windows(){
    pthread_mutex_lock(&data->mutex);

    werase(reg_win);
    box(reg_win, 0 , 0);
    mvwprintw(reg_win, 1, (getmaxx(reg_win)/2)-5, "Registers");
    mvwprintw(reg_win, 2, (getmaxx(reg_win)/2)-6, "-----------");
    for(int i = 0; i < NUM_REGS; i++){
        mvwprintw(reg_win, 4+(2*i), 4, "r%d -> %" PRIu16 "",i,vm->regs[i]);
    }

    werase(stk_win);
    box(stk_win, 0 , 0);
    mvwprintw(stk_win, 1, (getmaxx(reg_win)/2)-2, "Stack");
    mvwprintw(stk_win, 2, (getmaxx(reg_win)/2)-5, "-----------");
    for(int i = 0; i < Stack_size(vm->stk); i++){
        mvwprintw(stk_win, 4+i, 4, "stk%d -> %" PRIu16 "",i,Stack_peek(vm->stk,i));
    }

    werase(pc_win);
    box(pc_win, 0 , 0);
    mvwprintw(pc_win, 1, (getmaxx(reg_win)/2)-7, "Program Counter");
    mvwprintw(pc_win, 2, (getmaxx(reg_win)/2)-8, "-----------------");

    char * buf = malloc(256);
    int i = -6;
    if(vm->pc < 6) i = (-1*vm->pc);
    int pc_mod = i+vm->pc;

    while(i < 6){
        const cell * inst_ptr = Memory_get(vm->mem,pc_mod);
        if(pc_mod == vm->pc){
            wattrset(pc_win, A_STANDOUT);
            if(data->debug_input_mode == true){
                wattron(pc_win,COLOR_PAIR(1));
                mvwprintw(pc_win, 1, (getmaxx(reg_win)/2)-7, "Program Counter [PAUSED]");
            } 
            mvwprintw(pc_win, 10+i, 4, "%d: %s",pc_mod,string_of_cell(vm,inst_ptr,buf,&pc_mod));
            if(data->debug_input_mode == true) wattroff(pc_win,COLOR_PAIR(1));
            wattrset(pc_win, A_NORMAL);
        } 

        else mvwprintw(pc_win, 10+i, 4, "%d: %s",pc_mod,string_of_cell(vm,inst_ptr,buf,&pc_mod));
        
        i++;
        pc_mod++;
    }
    
    free(buf);

    wrefresh(reg_win);
    wrefresh(stk_win);
    wrefresh(pc_win);
    wrefresh(out_win);

    pthread_mutex_unlock(&data->mutex);
}

// update GUI and wait until 'r' key is pressed
void break_wait(){
    pthread_mutex_lock(&data->mutex);
    data->debug_input_mode = true;
    pthread_mutex_unlock(&data->mutex);
    
    refresh_windows();

    // read from input pipe until 'r' is found
    char c = 'a';
    while(read(vm->in_fd, &c, 1) > 0){
        if(c == 'r'){
            pthread_mutex_lock(&data->mutex);
            data->debug_input_mode = false;
            pthread_mutex_unlock(&data->mutex);
            break;
        }
    }

    refresh_windows();
}

// moved this function here from exec.c for better control over ncurses. May need refactoring.
void execute_debug(vm_t * vm, FILE * breakpoint_fp, int * breakflag){
    assert(vm);
    assert(breakpoint_fp);
    assert(breakflag);

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int breakpoint_count = 0;

    // count number of breakpoints
    while(!feof(breakpoint_fp)){
        char ch = fgetc(breakpoint_fp);
        if(ch == '\n'){
            breakpoint_count++;
        }
    }
    int * breakpoints = malloc(breakpoint_count*sizeof(int)); // FIX THIS! (currently being malloc'd lots of times)
    int curr_idx = 0;
    fseek(breakpoint_fp, 0, SEEK_SET);

    while ((read = getline(&line, &len, breakpoint_fp)) != -1) {
        breakpoints[curr_idx] = atoi(line);
        if(line){
            free(line);
            line = NULL;
        } 
        curr_idx++;
    }
    if (line) free(line);

    const cell * inst_ptr = Memory_get(vm->mem,vm->pc);
    if(!inst_ptr){
        exit(1);
    }

    while(inst_ptr){
        // update GUI when next instruction is INPUT
        if(inst_ptr->value == 20){
            refresh_windows();
        }
        for(int i = 0; i < breakpoint_count; i++){
            if(inst_ptr->addr == ((value_t) breakpoints[i])){
                *breakflag = (*breakflag) ? 0 : 1;
            }
        }
        if(*breakflag){
            break_wait(); //break;
            *breakflag = 0;
        }

        inst_ptr = exec_step(vm,NULL,inst_ptr,breakflag);
        if(!inst_ptr) *breakflag = -1;
    }

    free(breakpoints);

    return;
}

int debugger(vm_t * vm_in){
    vm = vm_in;

    pid_t pid;

    // pipe code from http://unixwiz.net/techtips/remap-pipe-fds.html
    int writepipe[2] = {-1,-1}; /* parent -> child */
    int readpipe [2] = {-1,-1}; /* child -> parent */

    if ( pipe(readpipe) < 0  ||  pipe(writepipe) < 0 ){
        /* FATAL: cannot create pipe */
        exit(EXIT_FAILURE);
    }

    #define PARENT_READ readpipe[0]
    #define CHILD_WRITE readpipe[1]
    #define CHILD_READ writepipe[0]  
    #define PARENT_WRITE writepipe[1]

    // set up ncurses windows
    initscr();
    start_color();
    cbreak(); 
    noecho();
    init_color(COLOR_BLACK, 0, 0, 0);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    keypad(stdscr, TRUE);
    move(LINES-1,0);     
    refresh();
    init_windows();
    initialise_shared();

    if ((pid = fork()) < 0){
        /* FATAL: cannot fork child */
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // we are the child
        close(PARENT_WRITE);
        close(PARENT_READ);

        // configure VM to read and write to pipes
        vm->out_fd = CHILD_WRITE;
        vm->in_fd = CHILD_READ;

        FILE * breakpoints_fp = fopen(BREAKPOINTS_FILENAME,"r");
        int breakflag = 0;
        execute_debug(vm,breakpoints_fp,&breakflag);

        pthread_mutex_lock(&data->mutex);
        data->done = true;
        pthread_mutex_unlock(&data->mutex);

        close(CHILD_WRITE);
        close(CHILD_READ);
        fclose(breakpoints_fp);

        //exit(0);
    }
    else{
        close(CHILD_READ);
        close(CHILD_WRITE);


        pid_t pid2;

         if ((pid2 = fork()) < 0){
            /* FATAL: cannot fork child */
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            // we are responsible for writing to the VM
            close(PARENT_READ);
            int ch;
            while((ch = getch())){
                pthread_mutex_lock(&data->mutex);
                write(PARENT_WRITE,&ch,1);
                if(!data->debug_input_mode) addch((char) ch); // echo typed characters only when not in debug mode
                pthread_mutex_unlock(&data->mutex);
            }
            close(PARENT_WRITE);
        }

        else{
            // we are responsible of reading the VM output
            close(PARENT_WRITE);
            char buf;
            int x_idx = 0;
            int y_idx = 0;

            while (read(PARENT_READ, &buf, 1) > 0){
                pthread_mutex_lock(&data->mutex);
                
                //mvwprintw(out_win,y_idx,2+x_idx,"%c", buf);
                wprintw(out_win,"%c", buf);
                wrefresh(out_win);
                x_idx++;
                if(buf == '\n'){
                    x_idx = 0;
                    y_idx++;
                }
                if(y_idx == (getmaxy(out_win))){
                    scroll(out_win);
                    wrefresh(out_win);
                    x_idx = 0;
                    y_idx--;
                }

                if (data->done) {
                    pthread_mutex_unlock(&data->mutex);
                    break;
                }    

                pthread_mutex_unlock(&data->mutex);
            }
            close(PARENT_READ);
            endwin();
        }
        

        //endwin(); // not sure if calling this twice (once for each child) is problematic
    }

    munmap(data, sizeof(data));
    return 0;
}