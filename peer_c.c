#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
void receive_file(int socket) {
char buffer[BUFFER_SIZE];
FILE *fp = fopen("received_file.txt", "w");
if (fp == NULL) {
perror("File could not be opened");
exit(EXIT_FAILURE);
}
int n;
while ((n = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
fwrite(buffer, sizeof(char), n, fp);
memset(buffer, 0, BUFFER_SIZE);
}fclose(fp);
}
int main() {
int sock_fd;
struct sockaddr_in server_addr;
// Create socket
sock_fd = socket(AF_INET, SOCK_STREAM, 0);
if (sock_fd == -1) {
perror("socket");
exit(EXIT_FAILURE);
}
// Set up the server address structure
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // Change IP if needed
// Connect to the server
if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
perror("connect");
close(sock_fd);
exit(EXIT_FAILURE);
}
// Receive the file from the server
receive_file(sock_fd);
printf("File received successfully.\n");
// Clean up
close(sock_fd);
return 0;
}
