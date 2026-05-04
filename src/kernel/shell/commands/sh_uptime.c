#include "system/timer.h"
#include "core/malloc.h"
#include "cpu/syscall.h"


int sh_uptime(int argc, char** argv){
malloc_stats();
    _syscall0(SYS_EXIT);

};