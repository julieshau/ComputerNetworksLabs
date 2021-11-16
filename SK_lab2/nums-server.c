#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include "err.h"

#define QUEUE_LENGTH     5
#define PORT_NUM     10001

struct __attribute__((__packed__)) DataStructure {
  uint16_t seq_no;
  uint32_t number;
};

int main(int argc, char *argv[])
{
  int sock, msg_sock, rflags;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  socklen_t client_address_len;

  struct DataStructure data_read;
  ssize_t len;

  sock = socket(PF_INET, SOCK_STREAM, 0); // creating IPv4 TCP socket
  if (sock <0)
    syserr("socket");

  server_address.sin_family = AF_INET; // IPv4
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); // listening on all interfaces
  server_address.sin_port = htons(PORT_NUM); // listening on port PORT_NUM

  // bind the socket to a concrete address
  if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    syserr("bind");

  // switch to listening (passive open)
  if (listen(sock, QUEUE_LENGTH) < 0)
    syserr("listen");

  printf("accepting client connections on port %hu\n", ntohs(server_address.sin_port));
  for (;;) {
    client_address_len = sizeof(client_address);
    // get client connection from the socket
    msg_sock = accept(sock, (struct sockaddr *) &client_address, &client_address_len);
    if (msg_sock < 0)
      syserr("accept");

    do {
      // serwer waits for the whole structure
      rflags = MSG_WAITALL;
      len = recv(msg_sock, ((char*)&data_read), sizeof(data_read), rflags);
      if (len < 0)
        syserr("reading from client socket");
      else if (len>0) {
        printf("read %zd bytes from socket\n", len);
	printf("received packet no %" PRIu16 ": %" PRIu32 "\n", ntohs(data_read.seq_no), ntohl(data_read.number));
      }      
    } while (len > 0);
    
    printf("ending connection\n");
    if (close(msg_sock) < 0)
      syserr("close");
  }
  
  return 0;
}
