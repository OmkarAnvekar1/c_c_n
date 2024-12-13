#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 12345
#define MULTICAST_ADDR "239.255.255.250"
#define MESSAGE "OMKAR ANVEKAR!\n"
int main() {
int sock_fd;
struct sockaddr_in multicast_addr;// Create a UDP socket
sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
if (sock_fd < 0) {
perror("socket");
exit(EXIT_FAILURE);
}
// Set up the multicast address
memset(&multicast_addr, 0, sizeof(multicast_addr));
multicast_addr.sin_family = AF_INET;
multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
multicast_addr.sin_port = htons(PORT);
// Send messages to the multicast group
while (1) {
ssize_t sent_bytes = sendto(sock_fd, MESSAGE, strlen(MESSAGE), 0,
(struct sockaddr *)&multicast_addr, sizeof(multicast_addr));
if (sent_bytes < 0) {
perror("sendto");
close(sock_fd);
exit(EXIT_FAILURE);
}
printf("Sent: %s", MESSAGE);
sleep(1); // Send a message every second
}
// Clean up
close(sock_fd);
return 0;
}
