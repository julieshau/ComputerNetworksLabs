#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "err.h"

//#define BUFFER_SIZE 1000

int main(int argc, char *argv[]) { //adress port N k
  int sock;
  struct addrinfo addr_hints;
  struct addrinfo *addr_result;

  int i, flags, sflags;
  size_t len;
  ssize_t snd_len, rcv_len;
  //char buffer[BUFFER_SIZE];
  struct sockaddr_in my_address;
  struct sockaddr_in srvr_address;
  socklen_t rcva_len;

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
  (void) memset(&addr_hints, 0, sizeof(struct addrinfo));
  addr_hints.ai_family = AF_INET; // IPv4
  addr_hints.ai_socktype = SOCK_DGRAM;
  addr_hints.ai_protocol = IPPROTO_UDP;
  addr_hints.ai_flags = 0;
  addr_hints.ai_addrlen = 0;
  addr_hints.ai_addr = NULL;
  addr_hints.ai_canonname = NULL;
  addr_hints.ai_next = NULL;
  if (getaddrinfo(argv[1], NULL, &addr_hints, &addr_result) != 0) {
    syserr("getaddrinfo");
  }
  
  my_address.sin_family = AF_INET; // IPv4
  my_address.sin_addr.s_addr =
    ((struct sockaddr_in*) (addr_result->ai_addr))->sin_addr.s_addr; // address IP
  my_address.sin_port = htons((uint16_t) atoi(argv[2])); // port from the command line
  
  freeaddrinfo(addr_result);
  
  sock = socket(PF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    syserr("socket");
  
  int N = atoi(argv[3]);
  for (i = 0; i < N; i++) {

    (void) printf("sending to socket: %s\n", message);
    sflags = 0;
    rcva_len = (socklen_t) sizeof(my_address);
    snd_len = sendto(sock, message, MES_SIZE, sflags,
		     (struct sockaddr *) &my_address, rcva_len);
    if (snd_len != (ssize_t) MES_SIZE) {
      syserr("partial / failed write");
    }
    
    (void) memset(buffer, 0, sizeof(buffer));
    flags = 0;
    len = (size_t) sizeof(buffer) - 1;
    rcva_len = (socklen_t) sizeof(srvr_address);
    rcv_len = recvfrom(sock, buffer, len, flags,
		       (struct sockaddr *) &srvr_address, &rcva_len);
    
    if (rcv_len < 0) {
      syserr("read");
    }
    (void) printf("read from socket: %zd bytes: %s\n", rcv_len, buffer);
  }
    //////
    sflags = 0;
    rcva_len = (socklen_t) sizeof(my_address);
    snd_len = sendto(sock, "\0", 0, sflags,
    	     (struct sockaddr *) &my_address, rcva_len);
    /////
  
  if (close(sock) == -1) { //very rare errors can occur here, but then
    syserr("close"); //it's healthy to do the check
  }
  
  return 0;
}
