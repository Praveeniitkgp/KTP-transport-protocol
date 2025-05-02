#include "ksocket.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>

void *receiver_thread(void *arg);
void *sender_thread(void *arg);

pthread_t r_thread, s_thread;

int last_ack[MAX_SOCKETS] = {0}; // Track last acknowledged sequence number
int acked[MAX_SOCKETS][WINDOW_SIZE] = {0}; // Track individual ACKs

// Initialize threads for reliable communication
int main() {
    srand(time(NULL));
    
    init_shared_memory();
    
    pthread_create(&r_thread, NULL, receiver_thread, NULL);
    pthread_create(&s_thread, NULL, sender_thread, NULL);
    
    pthread_join(r_thread, NULL);
    pthread_join(s_thread, NULL);
    
    return 0;
}

// Receiver thread handling incoming messages
void *receiver_thread(void *arg) {
    char buffer[BUFFER_SIZE + 1];  // Extra byte for sequence number
    while (1) {
        for (int i = 0; i < MAX_SOCKETS; i++) {
            if (socket_table[i].is_allocated) {
                struct sockaddr_in sender_addr;
                socklen_t addr_len = sizeof(sender_addr);
                int bytes = recvfrom(socket_table[i].udp_socket, buffer, BUFFER_SIZE + 1, 0,
                                     (struct sockaddr *)&sender_addr, &addr_len);
                if (bytes > 0) {
                    int seq_num = (int)buffer[0];  // Extract sequence number

                    if (!dropMessage(0.1)) {  // Simulating 10% packet loss
                        strcpy(socket_table[i].recv_buffer[seq_num % WINDOW_SIZE], buffer + 1);
                        acked[i][seq_num % WINDOW_SIZE] = 1;

                        while (acked[i][last_ack[i] % WINDOW_SIZE]) {
                            last_ack[i]++;
                        }

                        int ack_msg[2] = {seq_num, WINDOW_SIZE - 1};  
                        sendto(socket_table[i].udp_socket, ack_msg, sizeof(ack_msg), 0, 
                               (struct sockaddr *)&sender_addr, addr_len);
                    }
                }
            }
        }
        usleep(50000);
    }
}

// Sender thread handling retransmissions
void *sender_thread(void *arg) {
    while (1) {
        for (int i = 0; i < MAX_SOCKETS; i++) {
            if (socket_table[i].is_allocated) {
                struct sockaddr_in remote_addr = socket_table[i].remote_addr;

                for (int j = last_ack[i]; j < last_ack[i] + WINDOW_SIZE; j++) {
                    if (!acked[i][j % WINDOW_SIZE]) { // Packet not acknowledged
                        sendto(socket_table[i].udp_socket, socket_table[i].send_buffer[j % WINDOW_SIZE], BUFFER_SIZE, 0, 
                               (struct sockaddr *)&remote_addr, sizeof(remote_addr));
                        usleep(25000); // Short delay to prevent flooding
                    }
                }
            }
        }
        usleep(50000);
    }
}
