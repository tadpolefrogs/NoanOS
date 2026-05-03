#include "../include/commands.h"
#include "../include/types.h"
#include "../include/syscall.h"

// Command prototypes
int sh_ls(int argc, char** argv);
int sh_cd(int argc, char** argv);
int sh_pwd(int argc, char** argv);
int sh_mkdir(int argc, char** argv);
int sh_rm(int argc, char** argv);
int sh_cp(int argc, char** argv);
int sh_mv(int argc, char** argv);
int sh_touch(int argc, char** argv);
int sh_cat(int argc, char** argv);
int sh_echo(int argc, char** argv);
int sh_clear(int argc, char** argv);
int sh_help(int argc, char** argv);
int sh_reboot(int argc, char** argv);
int sh_shutdown(int argc, char** argv);
int sh_vim(int argc, char** argv);

shell_command_t commands[] = {
    {"ls", sh_ls, "List directory contents"},
    {"cd", sh_cd, "Change directory"},
    {"pwd", sh_pwd, "Print working directory"},
    {"mkdir", sh_mkdir, "Create directory"},
    {"rm", sh_rm, "Remove file or directory"},
    {"cp", sh_cp, "Copy file"},
    {"mv", sh_mv, "Move/rename file"},
    {"touch", sh_touch, "Create empty file"},
    {"cat", sh_cat, "Print file contents"},
    {"echo", sh_echo, "Print text"},
    {"vim", sh_vim, "Simple text editor"},
    {"clear", sh_clear, "Clear screen"},
    {"help", sh_help, "Show this help message"},
    {"reboot", sh_reboot, "Reboot system"},
    {"shutdown", sh_shutdown, "Shutdown system"}
};

int num_commands = sizeof(commands) / sizeof(shell_command_t);

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int execute_command(const char* cmd_line) {
    char buf[128];
    char* argv[16];
    int argc = 0;
    
    int i = 0;
    while(cmd_line[i] && i < 127) { buf[i] = cmd_line[i]; i++; }
    buf[i] = '\0';

    char* p = buf;
    while (*p) {
        while (*p == ' ') *p++ = '\0';
        if (*p == '\0') break;
        argv[argc++] = p;
        while (*p && *p != ' ') p++;
        if (argc >= 16) break;
    }

    if (argc == 0) return 0;

    for (int j = 0; j < num_commands; j++) {
        if (strcmp(argv[0], commands[j].name) == 0) {
            return commands[j].handler(argc, argv);
        }
    }

    _syscall1(SYS_PRINT, (u32)"Unknown command: ");
    _syscall1(SYS_PRINT, (u32)argv[0]);
    _syscall1(SYS_PUTCHAR, '\n');
    return -1;
}
