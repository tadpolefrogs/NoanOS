#include "../include/syscall.h"
#include "../include/commands.h"

int sh_shutdown(int argc, char** argv) {
    (void)argc; (void)argv;
    _syscall0(SYS_SHUTDOWN);
    return 0;
}
