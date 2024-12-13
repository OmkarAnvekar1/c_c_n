#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 12345
#define BUFFER_SIZE 1024
void *handle_client(void *client_socket) {
int sock = *(int *)client_socket;
free(client_socket);
char buffer[BUFFER_SIZE];
ssize_t n;while ((n = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
buffer[n] = '\0'; // Null-terminate the received string
printf("Received: %s", buffer);
// Optionally, send a response back to the client
send(sock, "Message received\n", 17, 0);
}
if (n == 0) {
printf("Client disconnected\n");
} else {
perror("recv");
}
close(sock);
return NULL;
}
int main() {
int server_fd, *client_socket;
struct sockaddr_in server_addr, client_addr;
socklen_t addr_len = sizeof(client_addr);
// Create server socket
server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd == -1) {
perror("socket");
exit(EXIT_FAILURE);
}
// Bind the socket
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;
server_addr.sin_port = htons(PORT);
if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
perror("bind");
close(server_fd);
exit(EXIT_FAILURE);
}
// Listen for incoming connections
if (listen(server_fd, 5) == -1) {
perror("listen");
close(server_fd);
exit(EXIT_FAILURE);
}printf("Server is running on port %d...\n", PORT);
while (1) {
client_socket = malloc(sizeof(int));
*client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
if (*client_socket == -1) {
perror("accept");
free(client_socket);
continue;
}
printf("Accepted connection from client\n");
// Create a new thread to handle the client
pthread_t tid;
if (pthread_create(&tid, NULL, handle_client, (void *)client_socket) != 0) {
perror("pthread_create");
close(*client_socket);
free(client_socket);
}
pthread_detach(tid); // Detach the thread to reclaim resources when it finishes
}
close(server_fd);
return 0;
}
