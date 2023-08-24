#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/socket.h>

int util_sockio_read_all (int sockfd, size_t amount, char *buffer);

int util_sockio_send_all (int sockfd, size_t amount, void *buffer);
