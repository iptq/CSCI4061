// Redirect standard output temporarily to a pipe, then restore
// standard output and retrieve the captured output
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define PREAD 0   // index of read end of pipe
#define PWRITE 1  // index of write end of pipe

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);  // Turn off I/O buffering

    printf("Process %d Piping\n", getpid());
    int my_pipe[2];
    pipe(my_pipe);

    int stdout_bak =
        dup(STDOUT_FILENO);  // Duplicate stdout so it can be restored later
    dup2(my_pipe[PWRITE], STDOUT_FILENO);  // Redirect stdout to pipe

    pid_t pid = fork();

    if (pid == 0) {
        printf(
            "%d In the pipe, five by five",  // Should not show up on the screen
            getpid());
        close(my_pipe[PWRITE]);  // Close the write end of the pipe
        exit(0);
    }
    wait(NULL);
    dup2(stdout_bak,
         STDOUT_FILENO);  // Restore stdout: redirect to backed up fd

    printf("%d Read from the my_pipe\n", getpid());
    char buf[2048];
    int bytes_read = read(my_pipe[PREAD], buf, 2048);

    buf[bytes_read] = '\0';  // Not null-terminated by default, append '\0'
    close(my_pipe[PREAD]);   // Close read end
    printf("%d Read: '%s'\n", getpid(), buf);  // Show earlier output

    return 0;
}
