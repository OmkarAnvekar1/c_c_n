#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
void send_file(FILE *fp, int socket) {
char buffer[BUFFER_SIZE];while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
if (send(socket, buffer, strlen(buffer), 0) == -1) {
perror("send");
exit(EXIT_FAILURE);
}
memset(buffer, 0, BUFFER_SIZE);
}
}
int main() {
int server_fd, client_fd;
struct sockaddr_in server_addr, client_addr;
socklen_t addr_len = sizeof(client_addr);
char *filename = "shared_file.txt";
// Create socket
server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd == -1) {
perror("socket");
exit(EXIT_FAILURE);
}
// Set up the server address structure
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;
server_addr.sin_port = htons(PORT);
// Bind the socket
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
}
printf("Peer-to-peer server is running on port %d...\n", PORT);
while (1) {
// Accept an incoming connection
client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
if (client_fd == -1) {
perror("accept");
continue;}
printf("Client connected.\n");
// Open the file to send
FILE *fp = fopen(filename, "r");
if (fp == NULL) {
perror("File not found");
close(client_fd);
continue;
}
// Send the file to the client
send_file(fp, client_fd);
printf("File sent successfully.\n");
// Clean up
fclose(fp);
close(client_fd);
}
close(server_fd);
return 0;
}
