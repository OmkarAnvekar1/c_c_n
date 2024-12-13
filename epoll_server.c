#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#define PORT 12345
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024
// Set a socket to non-blocking mode
int set_nonblocking(int sock) {
int flags = fcntl(sock, F_GETFL, 0);
if (flags == -1) return -1;
return fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}
int main() {
int server_fd, client_fd, epoll_fd;
struct sockaddr_in server_addr, client_addr;
socklen_t addr_len = sizeof(client_addr);
struct epoll_event event, events[MAX_EVENTS];
// Create server socket
server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd == -1) {
perror("socket");
exit(EXIT_FAILURE);
}
// Set the socket to non-blocking mode
if (set_nonblocking(server_fd) == -1) {
perror("set_nonblocking");
close(server_fd);
exit(EXIT_FAILURE);
}
// Bind the socket
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;
server_addr.sin_port = htons(PORT);
if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
perror("bind");close(server_fd);
exit(EXIT_FAILURE);
}
// Listen for incoming connections
if (listen(server_fd, 5) == -1) {
perror("listen");
close(server_fd);
exit(EXIT_FAILURE);
}
// Create epoll instance
epoll_fd = epoll_create1(0);
if (epoll_fd == -1) {
perror("epoll_create1");
close(server_fd);
exit(EXIT_FAILURE);
}
// Register the server socket with epoll
event.events = EPOLLIN;
event.data.fd = server_fd;
if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
perror("epoll_ctl");
close(server_fd);
exit(EXIT_FAILURE);
}
printf("Server is running on port %d...\n", PORT);
while (1) {
int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
for (int i = 0; i < nfds; i++) {
if (events[i].data.fd == server_fd) {
// Accept new connection
client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
if (client_fd == -1) {
perror("accept");
continue;
}
printf("Accepted connection from client\n");
set_nonblocking(client_fd);
// Register client socket with epoll
event.events = EPOLLIN | EPOLLET; // Edge-triggered
event.data.fd = client_fd;
if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
perror("epoll_ctl");
close(client_fd);
}} else {
// Handle data from client
char buffer[BUFFER_SIZE];
ssize_t count = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
if (count == -1) {
perror("recv");
close(events[i].data.fd);
} else if (count == 0) {
// Client has closed connection
printf("Client disconnected\n");
close(events[i].data.fd);
} else {
// Print received data
buffer[count] = '\0'; // Null-terminate the received string
printf("Received from client: %s", buffer);
}
}
}
}
close(server_fd);
close(epoll_fd);
return 0;
}
