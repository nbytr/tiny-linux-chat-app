#include "sockio.h"

int util_sockio_read_all (int sockfd, size_t amount, char *buffer)
{
  size_t total_read = 0;

  while (total_read < amount) {
    ssize_t amount_read = read(sockfd, buffer, amount - total_read);
    if (amount_read == 0)
      return -1; // The connection is closed
    if (amount_read == -1)
      return -2; // read() error

    total_read += amount_read;
    buffer += amount_read;
  }

  return 0;
}

int util_sockio_send_all (int sockfd, size_t amount, void *buffer)
{
  size_t total_sent = 0;

  while (total_sent < amount) {
    size_t amount_sent = send (sockfd, buffer, amount - total_sent, 0);

    if (amount_sent == -1)
      return -1;

    total_sent += amount_sent;
    buffer += amount_sent;
  }

  return 0;
}
