#include "blather.h"

server_t server;

void signal_handler(int signum) {
    server_shutdown(&server);
    exit(0);
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    client_t *client;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [name]\n", argv[0]);
        exit(1);
    }

    // set up signals
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    server_start(&server, argv[1], S_IRUSR | S_IWUSR);

    // main loop
    while (1) {
        // check all sources
        server_check_sources(&server);

        // handle a join request if one is ready
        if (server_join_ready(&server))
            server_handle_join(&server);

        for (int i = 0; i < server.n_clients; ++i) {
            client = server_get_client(&server, i);
            if (client->data_ready)
                server_handle_client(&server, i);
        }
    }
}
