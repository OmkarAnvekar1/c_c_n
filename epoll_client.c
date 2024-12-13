#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
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
// Connect to servermemset(&server_addr, 0, sizeof(server_addr));
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
// Read input from user
printf("> ");
fgets(buffer, BUFFER_SIZE, stdin);
// Send data to server
send(sock_fd, buffer, strlen(buffer), 0);
}
// Clean up
close(sock_fd);
return 0;
}
