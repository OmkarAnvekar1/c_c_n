#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define PORT 12345
#define MULTICAST_ADDR "239.255.255.250"
#define BUFFER_SIZE 1024
int main() {
int sock_fd;
struct sockaddr_in local_addr, multicast_addr;
char buffer[BUFFER_SIZE];
int n;// Create a UDP socket
sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
if (sock_fd < 0) {
perror("socket");
exit(EXIT_FAILURE);
}
// Allow multiple sockets to use the same PORT number
int reuse = 1;
if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse,
sizeof(reuse)) < 0) {
perror("setsockopt");
close(sock_fd);
exit(EXIT_FAILURE);
}
// Bind the socket to any valid IP address and the specified port
memset(&local_addr, 0, sizeof(local_addr));
local_addr.sin_family = AF_INET;
local_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all local interfaces
local_addr.sin_port = htons(PORT);
if (bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
perror("bind");
close(sock_fd);
exit(EXIT_FAILURE);
}
// Set up the multicast group address
memset(&multicast_addr, 0, sizeof(multicast_addr));
multicast_addr.sin_family = AF_INET;
multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
multicast_addr.sin_port = htons(PORT);
// Join the multicast group
struct ip_mreq mreq;
mreq.imr_multiaddr = multicast_addr.sin_addr;
mreq.imr_interface.s_addr = htonl(INADDR_ANY);
if (setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq,
sizeof(mreq)) < 0) {
perror("setsockopt");
close(sock_fd);
exit(EXIT_FAILURE);
}
printf("Listening for multicast messages on %s:%d\n", MULTICAST_ADDR, PORT);
while (1) {socklen_t addr_len = sizeof(multicast_addr);
n = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr
*)&multicast_addr, &addr_len);
if (n < 0) {
perror("recvfrom");
close(sock_fd);
exit(EXIT_FAILURE);
}
buffer[n] = '\0'; // Null-terminate the received message
printf("Received: %s", buffer);
}
// Clean up
close(sock_fd);
return 0;
}
