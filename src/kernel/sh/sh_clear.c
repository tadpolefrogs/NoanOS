#include "../include/syscall.h"
#include "../include/commands.h"

int sh_clear(int argc, char** argv) {
    (void)argc; (void)argv;
    _syscall0(SYS_CLEAR);
    return 0;
}
