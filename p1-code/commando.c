#include "commando.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cmdctl_t commands;

const char *HELP_CONTENTS =
    "COMMANDO COMMANDS\n"
    "help               : show this message\n"
    "exit               : exit the program\n"
    "list               : list all jobs that have been started giving "
    "information on each\n"
    "pause nanos secs   : pause for the given number of nanseconds and "
    "seconds\n"
    "output-for int     : print the output for given job number\n"
    "output-all         : print output for all jobs\n"
    "wait-for int       : wait until the given job number finishes\n"
    "wait-all           : wait for all jobs to finish\n"
    "command arg1 ...   : non-built-in is run as a job";

void cleanup() { cmdctl_freeall(&commands); }

// define command handlers

void exit_handler(char **tokens, int ntok) {
    cleanup();
    exit(0);
}
void help_handler(char **tokens, int ntok) { printf("%s\n", HELP_CONTENTS); }
void list_handler(char **tokens, int ntok) { cmdctl_print(&commands); }
void pause_handler(char **tokens, int ntok) {
    if (ntok < 3) return;
    long nsec = atol(tokens[1]);
    int sec = atoi(tokens[2]);
    pause_for(nsec, sec);
}

void output_cmd(cmd_t *command) {
    printf("@<<< Output for %s[#%d] (%d bytes):\n", command->name, command->pid,
           command->output_size);
    printf("----------------------------------------\n");
    cmd_print_output(command);
    printf("----------------------------------------\n");
}

void output_for_handler(char **tokens, int ntok) {
    if (ntok < 2) return;
    int id = atoi(tokens[1]);
    if (id > commands.size) return;
    output_cmd(commands.cmd[id]);
}
void output_all_handler(char **tokens, int ntok) {
    for (int i = 0, l = commands.size; i < l; ++i) {
        output_cmd(commands.cmd[i]);
    }
}

void wait_for_handler(char **tokens, int ntok) {
    if (ntok < 2) return;
    int id = atoi(tokens[1]);
    if (id > commands.size) return;
    cmd_update_state(commands.cmd[id], DOBLOCK);
}

void wait_all_handler(char **tokens, int ntok) {
    for (int i = 0, l = commands.size; i < l; ++i) {
	cmd_update_state(commands.cmd[i], DOBLOCK);
    }
}

const char *COMMANDS[8] = {"help",       "exit",       "list",     "pause",
                           "output-for", "output-all", "wait-for", "wait-all"};
void (*HANDLERS[8])(char **, int);

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    char input[MAX_LINE + 1];
    char *tokens[ARG_MAX];
    char cmd_flag;
    int ntok = 0;

    commands.size = 0;

    HANDLERS[0] = (void *)help_handler;
    HANDLERS[1] = (void *)exit_handler;
    HANDLERS[2] = (void *)list_handler;
    HANDLERS[3] = (void *)pause_handler;
    HANDLERS[4] = (void *)output_for_handler;
    HANDLERS[5] = (void *)output_all_handler;
    HANDLERS[6] = (void *)wait_for_handler;
    HANDLERS[7] = (void *)wait_all_handler;

    // echoing option
    char echoing = 0;
    if ((argc > 1 && !strncmp(argv[1], "--echo", 6)) || getenv("COMMANDO_ECHO"))
        echoing = 1;

    while (1) {
        // Print the prompt @>
        printf("@> ");

        // Use a call to fgets() to read a whole line of text from the user. The
        // #define MAX_LINE limits the length of what will be read. If no input
        // remains, print End of input and break out of the loop.
        char *result = fgets(input, MAX_LINE, stdin);
        if (!result) {
            printf("\nEnd of input\n");
            break;
        }

        // Echo (print) given input if echoing is enabled.
        if (echoing) {
            printf("%s", input);
        }

        // Use a call to parse_into_tokens() from util.c to break the line up by
        // spaces. If there are no tokens, jump to the end of the loop (the use
        // just
        // hit enter).
        parse_into_tokens(input, tokens, &ntok);
        if (!ntok) {
            continue;
        }

        // Examine the 0th token for built-ins like help, list, and so forth.
        // Use strncmp() to determine if any match and make appropriate calls.
        // This will be a long if/else chain of statements. (why?)
        cmd_flag = 0;
        for (int i = 0; i < 8; ++i) {
            if (!strncmp(tokens[0], COMMANDS[i], strlen(COMMANDS[i]))) {
                cmd_flag = 1;
                HANDLERS[i](tokens, ntok);
                break;
            }
        }

        // If no built-ins match, create a new cmd_t instance where the tokens
        // are the argv[] for it and start it running.
        if (!cmd_flag) {
            cmd_t *cmd = cmd_new(tokens);
            cmdctl_add(&commands, cmd);
            cmd_start(cmd);
        }

        // At the end of each loop, update the state of all child processes via
        // a call to cmdctl_update_state().
        cmdctl_update_state(&commands, NOBLOCK);
    }
    cleanup();
}
