#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "err.h"

//#define BUFFER_SIZE 2000

int main(int argc, char *argv[])
{
  int sock;
  struct addrinfo addr_hints;
  struct addrinfo *addr_result;

  int i, err;
  //char buffer[BUFFER_SIZE];
  ssize_t len, rcv_len;

  if (argc < 4) {
    fatal("Usage: %s host port message ...\n", argv[0]);
  }
  
  int MES_SIZE = atoi(argv[4]);//k
  char message[MES_SIZE + 1];
  message[MES_SIZE] = '\0';
  memset(message, 'X', MES_SIZE);
  
  int BUFFER_SIZE = MES_SIZE + 1;
  char buffer[BUFFER_SIZE];

  // 'converting' host/port in string to struct addrinfo
  memset(&addr_hints, 0, sizeof(struct addrinfo));
  addr_hints.ai_family = AF_INET; // IPv4
  addr_hints.ai_socktype = SOCK_STREAM;
  addr_hints.ai_protocol = IPPROTO_TCP;
  err = getaddrinfo(argv[1], argv[2], &addr_hints, &addr_result);
  if (err == EAI_SYSTEM) { // system error
    syserr("getaddrinfo: %s", gai_strerror(err));
  }
  else if (err != 0) { // other error (host not found, etc.)
    fatal("getaddrinfo: %s", gai_strerror(err));
  }

  // initialize socket according to getaddrinfo results
  sock = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
  if (sock < 0)
    syserr("socket");

  // connect socket to the server
  if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) < 0)
    syserr("connect");

  freeaddrinfo(addr_result);

  int N = atoi(argv[3]);
  for (i = 0; i < N; i++) {

    printf("writing to socket: %s\n", message);
    if (write(sock, message, MES_SIZE) != MES_SIZE) {
      syserr("partial / failed write");
    }

    memset(buffer, 0, sizeof(buffer));
    rcv_len = read(sock, buffer, sizeof(buffer) - 1);
    if (rcv_len < 0) {
      syserr("read");
    }
    printf("read from socket: %zd bytes: %s\n", rcv_len, buffer);
  }

  (void) close(sock); // socket would be closed anyway when the program ends

  return 0;
}
