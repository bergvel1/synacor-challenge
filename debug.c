#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <form.h>
#include <assert.h>
#include <inttypes.h>
#include "stack.h"
#include "value.h"
#include "mem.h"
#include "vm.h"
#include "exec.h"
#include "debug.h"

#define BREAKPOINTS_FILENAME "script/breakpoints"

WINDOW * reg_win = NULL;
WINDOW * stk_win = NULL;
WINDOW * pc_win = NULL;
WINDOW * out_win = NULL;

vm_t * vm = NULL;

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

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
    cbreak(); 
    //noecho();
    keypad(stdscr, TRUE);
    move(LINES-1,0);     
    refresh();
    init_windows();

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

        while(execute_debug(vm,breakpoints_fp) == 1){
            //werase(reg_win);
            for(int i = 0; i < NUM_REGS; i++){
                mvwprintw(reg_win, 4+(2*i), 4, "r%d -> %" PRIu16 "",i,vm->regs[i]);
            }
            for(int i = 0; i < Stack_size(vm->stk); i++){
                mvwprintw(stk_win, 4+i, 4, "stk%d -> %" PRIu16 "",i,Stack_peek(vm->stk,i));
            }

            mvwprintw(pc_win, 4, 4, "-> %s","test");

            wrefresh(reg_win);
            wrefresh(stk_win);
            wrefresh(pc_win);
            wrefresh(out_win);
            sleep(1);
        }

        close(CHILD_WRITE);
        close(CHILD_READ);
        fclose(breakpoints_fp);

        exit(0);
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
            while((ch = getch()) != KEY_F(1)){
                write(PARENT_WRITE,&ch,1);
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
            }
            close(PARENT_READ);
            endwin();
        }
        

        //endwin(); // not sure if calling this twice (once for each child) is problematic
    }

    return 0;
}