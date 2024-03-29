#include "commando.h"

// Add the given cmd to the ctl structure. update the cmd[] array and
// size field.
void cmdctl_add(cmdctl_t *ctl, cmd_t *cmd) { ctl->cmd[ctl->size++] = cmd; }

// Print all cmd elements in the given ctl structure.  The format of
// the table is
//
// JOB  #PID      STAT   STR_STAT OUTB COMMAND
// 0    #17434       0    EXIT(0) 2239 ls -l -a -F
// 1    #17435       0    EXIT(0) 3936 gcc --help
// 2    #17436      -1        RUN   -1 sleep 2
// 3    #17437       0    EXIT(0)  921 cat Makefile
//
// Widths of the fields and justification are as follows
//
// JOB  #PID      STAT   STR_STAT OUTB COMMAND
// 1234  12345678 1234 1234567890 1234 Remaining
// left  left    right      right rigt left
// int   int       int     string  int string
//
// The final field should be the contents of cmd->argv[] with a space
// between each element of the array.
void cmdctl_print(cmdctl_t *ctl) {
    printf("JOB  #PID      STAT   STR_STAT OUTB COMMAND\n");
    for (int i = 0, l = ctl->size; i < l; ++i) {
        printf("%-4d #%-8d %4d %10s %4d ", i, ctl->cmd[i]->pid,
               ctl->cmd[i]->status, ctl->cmd[i]->str_status,
               ctl->cmd[i]->output_size);
        for (int j = 0; ctl->cmd[i]->argv[j]; ++j) {
            printf("%s ", ctl->cmd[i]->argv[j]);
        }
        printf("\n");
    }
}

// Update each cmd in ctl by calling cmd_update_state() which is also
// passed the block argument (either NOBLOCK or DOBLOCK)
void cmdctl_update_state(cmdctl_t *ctl, int block) {
    for (int i = 0, l = ctl->size; i < l; ++i) {
        cmd_update_state(ctl->cmd[i], block);
    }
}

// Call cmd_free() on all of the constituent cmd_t's.
void cmdctl_freeall(cmdctl_t *ctl) {
    for (int i = 0, l = ctl->size; i < l; ++i) {
        cmd_free(ctl->cmd[i]);
    }
}
