#include <errno.h>

#include "blather.h"

// Gets a pointer to the client_t struct at the given index. If the
// index is beyond n_clients, the behavior of the function is
// unspecified and may cause a program crash.
client_t *server_get_client(server_t *server, int idx) {
    if (idx > server->n_clients) {
        // cause a kernel panic
        raise(SIGSEGV);
    }
    return &server->client[idx];
}

// Initializes and starts the server with the given name. A join fifo
// called "server_name.fifo" should be created. Removes any existing
// file of that name prior to creation. Opens the FIFO and stores its
// file descriptor in join_fd._
//
// ADVANCED: create the log file "server_name.log" and write the
// initial empty who_t contents to its beginning. Ensure that the
// log_fd is position for appending to the end of the file. Create the
// POSIX semaphore "/server_name.sem" and initialize it to 1 to
// control access to the who_t portion of the log.
void server_start(server_t *server, char *server_name, int perms) {
    // add .fifo to the server name
    int len = strlen(server_name);
    strncpy(server->server_name, server_name, len + 1);
    char server_fifo_name[len + 6];
    snprintf(server_fifo_name, len + 6, "%s.fifo", server_name);

    // remove existing fifo
    remove(server_fifo_name);
    if (mkfifo(server_fifo_name, perms) == -1) {
        fprintf(stderr, "Error creating '%s': %s\n", server_fifo_name,
                strerror(errno));
        exit(1);
    }

    // open the fifo
    if ((server->join_fd = open(server_fifo_name, O_RDWR)) == -1) {
        fprintf(stderr, "Error opening '%s': %s\n", server_fifo_name,
                strerror(errno));
        exit(1);
    }
}

// Shut down the server. Close the join FIFO and unlink (remove) it so
// that no further clients can join. Send a BL_SHUTDOWN message to all
// clients and proceed to remove all clients in any order.
//
// ADVANCED: Close the log file. Close the log semaphore and unlink
// it.
void server_shutdown(server_t *server) {
    // close the fifo
    close(server->join_fd);

    // get fifo name
    int len = strlen(server->server_name);
    char server_fifo_name[len + 6];
    snprintf(server_fifo_name, len + 6, "%s.fifo", server->server_name);

    // remove the fifo
    remove(server_fifo_name);

    // send BL_SHUTDOWN
    mesg_t msg = {.kind = BL_SHUTDOWN};
    server_broadcast(server, &msg);

    // remove clients
    for (int i = 0; i < server->n_clients; ++i) {
        server_remove_client(server, i);
    }
}

// Adds a client to the server according to the parameter join which
// should have fileds such as name filed in.  The client data is
// copied into the client[] array and file descriptors are opened for
// its to-server and to-client FIFOs. Initializes the data_ready field
// for the client to 0. Returns 0 on success and non-zero if the
// server as no space for clients (n_clients == MAXCLIENTS).
int server_add_client(server_t *server, join_t *join) {
    if (server->n_clients == MAXCLIENTS)
        return 1;

    int to_server_fd, to_client_fd;
    if ((to_server_fd = open(join->to_server_fname, O_RDWR)) == -1) {
        fprintf(stderr, "Could not open connection '%s': %s\n",
                join->to_server_fname, strerror(errno));
        exit(1);
    }
    if ((to_client_fd = open(join->to_client_fname, O_RDWR)) == -1) {
        fprintf(stderr, "Could not open connection '%s': %s\n",
                join->to_server_fname, strerror(errno));
        exit(1);
    }

    client_t client = {.to_server_fd = to_server_fd,
                       .to_client_fd = to_client_fd,
                       .data_ready = 0};

    strncpy(client.name, join->name, MAXPATH);
    strncpy(client.to_server_fname, join->to_server_fname, MAXPATH);
    strncpy(client.to_client_fname, join->to_client_fname, MAXPATH);

    server->client[server->n_clients++] = client;
    return 0;
}

// Remove the given client likely due to its having departed or
// disconnected. Close fifos associated with the client and remove
// them.  Shift the remaining clients to lower indices of the client[]
// array and decrease n_clients.
int server_remove_client(server_t *server, int idx) {
    // close fds
    client_t *client = server_get_client(server, idx);
    close(client->to_server_fd);
    close(client->to_client_fd);

    for (int i = idx; i < server->n_clients - 1; ++i)
        server->client[i] = server->client[i + 1];

    server->n_clients--;
    return 0;
}

// Send the given message to all clients connected to the server by
// writing it to the file descriptors associated with them.
//
// ADVANCED: Log the broadcast message unless it is a PING which
// should not be written to the log.
int server_broadcast(server_t *server, mesg_t *mesg) {
    client_t *client;
    for (int i = 0; i < server->n_clients; ++i) {
        client = server_get_client(server, i);
        if ((write(client->to_client_fd, mesg, sizeof(mesg_t))) < 0)
            return 1;
    }
    return 0;
}

// Checks all sources of data for the server to determine if any are
// ready for reading. Sets the servers join_ready flag and the
// data_ready flags of each of client if data is ready for them.
// Makes use of the select() system call to efficiently determine
// which sources are ready.
void server_check_sources(server_t *server) {
    fd_set readfds;
    FD_ZERO(&readfds);

    int maxfd = server->join_fd;
    FD_SET(server->join_fd, &readfds);
    client_t *client;
    for (int i = 0; i < server->n_clients; ++i) {
        client = server_get_client(server, i);
        client->data_ready = 0;
        FD_SET(client->to_server_fd, &readfds);
        if (client->to_server_fd > maxfd)
            maxfd = client->to_server_fd;
    }

    if (select(maxfd + 1, &readfds, NULL, NULL, NULL)) {
        if (FD_ISSET(server->join_fd, &readfds))
            server->join_ready = 1;
        for (int i = 0; i < server->n_clients; ++i) {
            client = server_get_client(server, i);
            if (FD_ISSET(client->to_server_fd, &readfds))
                client->data_ready = 1;
        }
    }
}

// Return the join_ready flag from the server which indicates whether
// a call to server_handle_join() is safe.
int server_join_ready(server_t *server) { return server->join_ready; }

// Call this function only if server_join_ready() returns true. Read a
// join request and add the new client to the server. After finishing,
// set the servers join_ready flag to 0.
int server_handle_join(server_t *server) {
    // read join request
    join_t request;
    if (read(server->join_fd, &request, sizeof(join_t)) < 0)
        return -1;
    printf("name: %s\n", request.name);

    // add client
    server_add_client(server, &request);
    server->join_ready = 0;
    return 0;
}

// Return the data_ready field of the given client which indicates
// whether the client has data ready to be read from it.
int server_client_ready(server_t *server, int idx) {
    client_t *client = server_get_client(server, idx);
    return client->data_ready;
}

// Process a message from the specified client. This function should
// only be called if server_client_ready() returns true. Read a
// message from to_server_fd and analyze the message kind. Departure
// and Message types should be broadcast to all other clients.  Ping
// responses should only change the last_contact_time below. Behavior
// for other message types is not specified. Clear the client's
// data_ready flag so it has value 0.
//
// ADVANCED: Update the last_contact_time of the client to the current
// server time_sec.
int server_handle_client(server_t *server, int idx) {
    client_t *client = server_get_client(server, idx);

    mesg_t payload;
    read(client->to_server_fd, &payload, sizeof(mesg_t));

    printf("received message: (%d) <%s> '%s'\n", payload.kind, payload.name,
           payload.body);
    switch (payload.kind) {
    case BL_MESG:
    case BL_JOINED:
    case BL_DEPARTED:
        server_broadcast(server, &payload);
        break;
    case BL_SHUTDOWN:
    case BL_DISCONNECTED:
        // shouldn't happen
        break;
    case BL_PING:
        // TODO
        break;
    }

    client->data_ready = 0;
    return 0;
}

// ADVANCED: Increment the time for the server
void server_tick(server_t *server);

// ADVANCED: Ping all clients in the server by broadcasting a ping.
void server_ping_clients(server_t *server);

// ADVANCED: Check all clients to see if they have contacted the
// server recently. Any client with a last_contact_time field equal to
// or greater than the parameter disconnect_secs should be
// removed. Broadcast that the client was disconnected to remaining
// clients.  Process clients from lowest to highest and take care of
// loop indexing as clients may be removed during the loop
// necessitating index adjustments.
void server_remove_disconnected(server_t *server, int disconnect_secs);

// ADVANCED: Write the current set of clients logged into the server
// to the BEGINNING the log_fd. Ensure that the write is protected by
// locking the semaphore associated with the log file. Since it may
// take some time to complete this operation (acquire semaphore then
// write) it should likely be done in its own thread to preven the
// main server operations from stalling.  For threaded I/O, consider
// using the pwrite() function to write to a specific location in an
// open file descriptor which will not alter the position of log_fd so
// that appends continue to write to the end of the file.
void server_write_who(server_t *server);

// ADVANCED: Write the given message to the end of log file associated
// with the server.
void server_log_message(server_t *server, mesg_t *mesg);