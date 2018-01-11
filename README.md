# synacor-challenge
This project is a virtual machine written in C, to be used in solving the [Synacor Challenge](https://challenge.synacor.com/).

## How to use
The virtual machine may be run either by itself, or within a debugging environment. 

### Standalone
To run the VM without debugging, type:
```
make
./synacor
```
### Custom Commands
In addition to the default commands used to play the text-based adventure game within the VM, it is possible to call several custom instructions which I have implemented to give greater control over the VM during execution. These are:

* **fastforward** or **ff**: skips the beginning of the game by running a scripted sequence of commands given in `script/fastforward`
* **quit** or **exit** or **q**: shuts down the VM
* **tele_hack**: attempts to bypass the teleporter check algorithm by directly modifying the VM memory (see `exec.c` for details)
* **\_setreg _r_ _v_**: sets register _r_ to value _v_
* **dump**: prints internal VM state (not including memory)
* **fdump**: prints internal VM state, including memory

### Debugging
To start the VM in the debugger, type:
```
make
./synacor -d
```
![Debugger GUI](https://github.com/bergvel1/synacor-challenge/raw/master/img/debug_gui.png "Debugger GUI")

The debugger GUI displays information about the internal state of the VM during execution, such as register and stack values, as well as the region of memory containing the code currently being executed.

#### Breakpoints and Debugger Commands

The debugger also allows for the setting of breakpoints and stepwise execution. In order to set breakpoints, edit `script/breakpoints` so that each line contains a memory location of a desired breakpoint. These memory locations can most easily be determined from the decompiled source `log/decomp.txt`.

![Debugger Pause](https://github.com/bergvel1/synacor-challenge/raw/master/img/debug_pause.png "Debugger Pause")

Upon reaching a breakpoint, the debugger will pause execution and wait for user input. We can call this entering _debug mode_, which is distinguishable from normal execution by the red highlighting in the Program Counter pane of the GUI. Within debug mode, it is possible to slowly step through execution one instruction at a time in order to study in detail how the VM state changes.

Since the VM is paused when debug mode is active, it is not possible to interact with it using normal commands. Instead, the following debugger commands may be used:

* **s**: performs a single step of execution
* **r**: exits debug mode and resumes execution until the next breakpoint
* **q**: shuts down the VM and debugger