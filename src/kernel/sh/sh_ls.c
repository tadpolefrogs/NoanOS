#include "../include/syscall.h"
#include "../include/commands.h"

int sh_ls(int argc, char** argv) {
    (void)argc; (void)argv;
    _syscall0(SYS_LS);
    return 0;
}
