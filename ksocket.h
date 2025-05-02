#ifndef KSOCKET_H
#define KSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SOCK_KTP 9999  // Custom socket type
#define MAX_SOCKETS 10 // Max active KTP sockets
#define BUFFER_SIZE 512 // Fixed message size
#define WINDOW_SIZE 10  // Send/receive window size
#define TIMEOUT 5       // Retransmission timeout (seconds)

// Error codes
#define ENOSPACE 1     // No space in buffer
#define ENOTBOUND 2    // Socket not bound
#define ENOMESSAGE 3   // No available message

// Structure for KTP socket state
typedef struct {
    int is_allocated;  // 1 if allocated, 0 otherwise
    int udp_socket;    // Corresponding UDP socket
    struct sockaddr_in local_addr;  // Local IP and port
    struct sockaddr_in remote_addr; // Remote IP and port
    char send_buffer[WINDOW_SIZE][BUFFER_SIZE];
    char recv_buffer[WINDOW_SIZE][BUFFER_SIZE];
    int swnd[WINDOW_SIZE];  // Send window tracking
    int rwnd[WINDOW_SIZE];  // Receive window tracking
} ktp_socket_t;

// Declare shared memory variables as extern
extern int shm_id;
extern ktp_socket_t *socket_table;

// Function declarations
void init_shared_memory();
int k_socket(int domain, int type, int protocol);
int k_bind(int socket, struct sockaddr_in *local, struct sockaddr_in *remote);
int k_sendto(int socket, const void *message, size_t length);
int k_recvfrom(int socket, void *buffer, size_t length);
int k_close(int socket);
int dropMessage(float p);

#endif // KSOCKET_H
