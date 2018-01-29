#include "blather.h"

simpio_t simpio;
char username[MAXNAME];
int to_server_fd, to_client_fd;
pthread_t client_thread, server_thread;

void *client_thread_fn(void *arg) {
    // user thread{
    //   repeat:
    //     read input using simpio
    //     when a line is ready
    //     create a mesg_t with the line and write it to the to-server FIFO
    //   until end of input
    //   write a DEPARTED mesg_t into to-server
    //   cancel the server thread
    while (!simpio.end_of_input) {
        simpio_reset(&simpio);
        iprintf(&simpio, "");
        while (!simpio.line_ready &&
               !simpio.end_of_input) { // read until line is complete
            simpio_get_char(&simpio);
        }
        if (simpio.line_ready) {
            // iprintf(&simpio, "You entered: %s\n", simpio.buf);
            mesg_t payload = {.kind = BL_MESG};
            strncpy(payload.name, username, MAXNAME);
            strncpy(payload.body, simpio.buf, MAXLINE);
            write(to_server_fd, &payload, sizeof(mesg_t));
        }
    }
    pthread_cancel(server_thread);
    return NULL;
}

void *server_thread_fn(void *arg) {
    // server thread{
    //   repeat:
    //     read a mesg_t from to-client FIFO
    //     print appropriate response to terminal with simpio
    //   until a SHUTDOWN mesg_t is read
    //   cancel the user thread
    mesg_t payload, response;
    while (read(to_client_fd, &payload, sizeof(mesg_t))) {
        switch (payload.kind) {
        case BL_MESG:
            iprintf(&simpio, "[%s] : %s\n", payload.name, payload.body);
            break;
        case BL_JOINED:
            iprintf(&simpio, "-- %s JOINED --\n", payload.name);
            break;
        case BL_DEPARTED:
            iprintf(&simpio, "-- %s DEPARTED --\n", payload.name);
            break;
        case BL_SHUTDOWN:
            goto server_shutdown;
        case BL_DISCONNECTED:
            iprintf(&simpio, " -- '%s' has disconnected.\n", payload.name);
            break;
        case BL_PING:
            response.kind = BL_PING;
            write(to_server_fd, &response, sizeof(mesg_t));
            break;
        }
    }
server_shutdown:
    iprintf(&simpio, "!!! server is shutting down !!!\n");
    pthread_cancel(client_thread);
    return NULL;
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);

    // variables used
    char *servername;
    int server_join_fd;

    // read name of server and name of user from command line args
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [server] [username]\n", argv[0]);
        exit(1);
    }
    servername = argv[1];
    strncpy(username, argv[2], MAXNAME);
    if (strlen(username) > MAXNAME) {
        fprintf(stderr, "Username '%s' is too long.\n", username);
        exit(1);
    }

    // TODO
    (void)(username);
    (void)(server_join_fd);

    // create to-server and to-client FIFOs
    int len = strlen(servername) + strlen(username);
    char to_server_fifo_name[len + 10];
    snprintf(to_server_fifo_name, len + 10, "%s.to.%s.fifo", username,
             servername);
    char to_client_fifo_name[len + 10];
    snprintf(to_client_fifo_name, len + 10, "%s.to.%s.fifo", servername,
             username);

    remove(to_server_fifo_name);
    remove(to_client_fifo_name);

    mkfifo(to_server_fifo_name, S_IRUSR | S_IWUSR);
    mkfifo(to_client_fifo_name, S_IRUSR | S_IWUSR);

    to_server_fd = open(to_server_fifo_name, O_RDWR);
    to_client_fd = open(to_client_fifo_name, O_RDWR);

    // write a join_t request to the server FIFO
    len = strlen(servername);
    char server_fifo_name[len + 6];
    snprintf(server_fifo_name, len + 6, "%s.fifo", servername);
    server_join_fd = open(server_fifo_name, O_RDWR);

    join_t request;
    strncpy(request.name, username, MAXPATH);
    strncpy(request.to_client_fname, to_client_fifo_name, MAXPATH);
    strncpy(request.to_server_fname, to_server_fifo_name, MAXPATH);
    write(server_join_fd, &request, sizeof(join_t));

    // write join message
    mesg_t payload = {.kind = BL_JOINED};
    strncpy(payload.name, username, MAXNAME);
    write(to_server_fd, &payload, sizeof(mesg_t));

    // configure simpio
    len = strlen(username);
    char prompt[len + 4];
    snprintf(prompt, len + 4, "%s>> ", username);
    simpio_set_prompt(&simpio, prompt);
    simpio_reset(&simpio);
    simpio_noncanonical_terminal_mode();

    // start a user thread to read input
    if (pthread_create(&client_thread, NULL, &client_thread_fn, NULL)) {
        fprintf(stderr, "Error creating client thread.\n");
        exit(1);
    }

    // start a server thread to listen to the server
    if (pthread_create(&server_thread, NULL, &server_thread_fn, NULL)) {
        fprintf(stderr, "Error creating server thread.\n");
        exit(1);
    }

    // wait for threads to return
    pthread_join(client_thread, NULL);
    pthread_join(server_thread, NULL);

    // write depart message
    payload.kind = BL_DEPARTED;
    strncpy(payload.name, username, MAXNAME);
    write(to_server_fd, &payload, sizeof(mesg_t));

    // close file descriptors
    close(to_server_fd);
    close(to_client_fd);

    // restore standard terminal output
    simpio_reset_terminal_mode();
    printf("\n");
    return 0;
}
