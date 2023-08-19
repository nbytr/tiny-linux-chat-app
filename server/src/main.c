#include <netinet/in.h>
#include <stdio.h>
#include <memory.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <errno.h>

#define PORT "4950"

int main (void)
{
  // Create server socket

  // Find addresses to bind to
  int yes = 1;
  int result = 0;
  struct addrinfo hints, *ai, *p;

  memset (&hints, 0, sizeof (hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((result = getaddrinfo (NULL, PORT, &hints, &ai)) != 0) {
    fprintf (stderr, "Failed to get addresses: %s\n", strerror (result));
    return 1;
  }

  // Loop through found addresses until one that can be successfully
  // bind'd to is found
  int server_sock = -1;
  for (p = ai; p != NULL; p = p->ai_next) {
    // Attempt to create the socket
    server_sock = socket (p->ai_family, p->ai_socktype, p->ai_protocol);

    if (server_sock == -1) {
      fprintf (stderr, "%d: %s\n", server_sock, "Socket FD = -1");
      continue;
    }

    // Attempt to set the socket's address to re-usable so the application
    // can re-bind quickly if relaunched
    if (setsockopt (server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) == -1) {
      fprintf (stderr, "%d: %s\n", server_sock, "setsockopt() failed");
      continue;
    }

    // Attempt to bind the socket to the address we can listen on the
    // address later on
    if (bind (server_sock, p->ai_addr, p->ai_addrlen)== -1) {
      fprintf (stderr, "%d: %s: %s\n", server_sock, "bind() failed", strerror (errno));
      close (server_sock);
      continue;
    }

    break;
  }

  // If p == NULL, then we looped through past the last valid address, meaning
  // that there was no valid address to bind to
  if (p == NULL) {
    fprintf (stderr, "Failed to find valid address to listen on!\n");
    return 2;
  }

  // Attempt to listen for incoming addresses
  if (listen (server_sock, 128) == -1) {
    fprintf (stderr, "Failed to listen for connections!\n");
    return 3;
  }

  // Output the address the socket is listening on
  if (p->ai_family == AF_INET) {
    struct in_addr servaddr = ((struct sockaddr_in *)p->ai_addr)->sin_addr;
    char str_serveraddr[INET_ADDRSTRLEN];
    inet_ntop (p->ai_family, &servaddr, str_serveraddr, INET_ADDRSTRLEN);
    printf ("Listening on %s:%s\n", str_serveraddr, PORT);
  } else {
    struct in6_addr servaddr = ((struct sockaddr_in6 *)p->ai_addr)->sin6_addr;
    char str_serveraddr[INET6_ADDRSTRLEN];
    inet_ntop (p->ai_family, &servaddr, str_serveraddr, INET6_ADDRSTRLEN);
    printf ("Listening on %s:%s\n", str_serveraddr, PORT);
  }

  // Free the linked list of addresses
  freeaddrinfo (ai);

  // Loop continously, accepting connections
  struct sockaddr_storage client_addr;
  socklen_t client_addr_len = sizeof (client_addr);
  int client_sock = -1;
  for (;;) {
    // Block until a connection is made
    if ((client_sock = accept (server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
      fprintf (stderr, "Failed to accept(): %s\n", strerror(errno));
      continue;
    }
    printf ("Got connection");

    // Output the client's address
    if (client_addr.ss_family == AF_INET) {
      struct in_addr *addr = &(((struct sockaddr_in *)&client_addr)->sin_addr);
      char straddr[INET_ADDRSTRLEN];
      memset (straddr, 0, INET_ADDRSTRLEN);
      inet_ntop (AF_INET, addr, straddr, INET_ADDRSTRLEN);
      printf (" from %s\n", straddr);
    } else {
      struct in6_addr *addr = &(((struct sockaddr_in6 *)&client_addr)->sin6_addr);
      char straddr[INET6_ADDRSTRLEN];
      memset (straddr, 0, INET6_ADDRSTRLEN);
      inet_ntop (AF_INET6, addr, straddr, INET6_ADDRSTRLEN);
      printf (" from %s\n", straddr);
    }
    // Send a small message, and close the connnection
    send (client_sock, "Hello!", 6, 0);
    close (client_sock);
  }

  return 0;
}
