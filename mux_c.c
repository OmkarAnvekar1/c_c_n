#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server. Type messages to send:\n");

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock_fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // Wait for activity
        int max_fd = sock_fd > STDIN_FILENO ? sock_fd : STDIN_FILENO;
        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        // Check if there is data to read from the socket
        if (FD_ISSET(sock_fd, &readfds)) {
            ssize_t n = recv(sock_fd, buffer, sizeof(buffer), 0);
            if (n > 0) {
                buffer[n] = '\0'; // Null-terminate the received string
                printf("Server: %s", buffer);
            } else {
                // Server closed the connection
                printf("Server disconnected\n");
                close(sock_fd);
                exit(EXIT_SUCCESS);
            }
        }

        // Check if there is input from the user
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            printf("> ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(sock_fd, buffer, strlen(buffer), 0);
        }
    }

    // Clean up
    close(sock_fd);
    return 0;
}

