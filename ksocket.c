#include "ksocket.h"

// Define shared memory variables here
int shm_id;
ktp_socket_t *socket_table;

// Initialize shared memory
void init_shared_memory() {
    shm_id = shmget(IPC_PRIVATE, MAX_SOCKETS * sizeof(ktp_socket_t), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("Shared memory allocation failed");
        exit(1);
    }
    socket_table = (ktp_socket_t *)shmat(shm_id, NULL, 0);
    memset(socket_table, 0, MAX_SOCKETS * sizeof(ktp_socket_t));
}

// Other k_socket, k_bind, k_sendto, k_recvfrom, k_close functions remain unchanged.


// Create a KTP socket
int k_socket(int domain, int type, int protocol) {
    if (type != SOCK_KTP) {
        errno = EINVAL;
        return -1;
    }
    
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (!socket_table[i].is_allocated) {
            socket_table[i].is_allocated = 1;
            socket_table[i].udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
            return i;
        }
    }
    errno = ENOSPACE;
    return -1;
}

// Bind a KTP socket
int k_bind(int socket, struct sockaddr_in *local, struct sockaddr_in *remote) {
    if (socket < 0 || socket >= MAX_SOCKETS || !socket_table[socket].is_allocated) {
        errno = ENOTBOUND;
        return -1;
    }
    socket_table[socket].local_addr = *local;
    socket_table[socket].remote_addr = *remote;
    return bind(socket_table[socket].udp_socket, (struct sockaddr *)local, sizeof(struct sockaddr_in));
}

// Send a message
int k_sendto(int socket, const void *message, size_t length) {
    if (socket < 0 || socket >= MAX_SOCKETS || !socket_table[socket].is_allocated) {
        errno = ENOTBOUND;
        return -1;
    }
    return sendto(socket_table[socket].udp_socket, message, length, 0,
                  (struct sockaddr *)&socket_table[socket].remote_addr, sizeof(struct sockaddr_in));
}

// Receive a message
int k_recvfrom(int socket, void *buffer, size_t length) {
    if (socket < 0 || socket >= MAX_SOCKETS || !socket_table[socket].is_allocated) {
        errno = ENOMESSAGE;
        return -1;
    }
    return recvfrom(socket_table[socket].udp_socket, buffer, length, 0, NULL, NULL);
}

// Close a KTP socket
int k_close(int socket) {
    if (socket < 0 || socket >= MAX_SOCKETS || !socket_table[socket].is_allocated) {
        return -1;
    }
    close(socket_table[socket].udp_socket);
    memset(&socket_table[socket], 0, sizeof(ktp_socket_t));
    return 0;
}

// Simulate message loss
int dropMessage(float p) {
    return ((float)rand() / RAND_MAX) < p ? 1 : 0;
}
