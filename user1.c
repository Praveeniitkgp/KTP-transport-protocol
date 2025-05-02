#include "ksocket.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <local IP> <local port> <remote IP> <remote port>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in local, remote;

    // Initialize shared memory
    init_shared_memory();

    int sock = k_socket(AF_INET, SOCK_KTP, 0);
    if (sock < 0) {
        perror("k_socket failed");
        return 1;
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));
    local.sin_addr.s_addr = inet_addr(argv[1]);

    remote.sin_family = AF_INET;
    remote.sin_port = htons(atoi(argv[4]));
    remote.sin_addr.s_addr = inet_addr(argv[3]);

    if (k_bind(sock, &local, &remote) < 0) {
        perror("k_bind failed");
        return 1;
    }

    FILE *file = fopen("largefile.txt", "r");
    if (!file) {
        perror("File open failed");
        return 1;
    }

    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, file)) {
        if (k_sendto(sock, buffer, BUFFER_SIZE) < 0) {
            perror("k_sendto failed");
            break;
        }
    }

    fclose(file);
    k_close(sock);
    return 0;
}
