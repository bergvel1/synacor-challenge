/*

  CURIN1.C
  ========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Demonstrating basic ncurses single key input.

*/


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
    idlok(out_win,TRUE);
}

int exec_debug(vm_t * vm_in){
    vm = vm_in;

    int ch;

    initscr();

    cbreak(); 
    //noecho();
    keypad(stdscr, TRUE);
    move(LINES-1,0);     

    refresh();
    init_windows();
    int i = 0;


    while((ch = getch()) != 'q')
    {       
        if(i == (getmaxy(out_win)-1)){
            scroll(out_win);
            wrefresh(out_win);
            i--;
        }
        else i++;
        mvwprintw(out_win,i,2,"%d - test\n", i);
        wrefresh(out_win);
    }
    
    //execute(vm,NO_LOG);
        
    endwin();
    return 0;
}