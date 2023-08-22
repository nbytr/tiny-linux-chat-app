#pragma once

typedef enum {
  ERR_OBTAIN_ADDR = 0, // Failed to find addresses to bind to
  ERR_NO_VALID_ADDR,   // No valid addresses to bind to
  ERR_LISTENING,       // Failed to listen()
  ERR_OBTAIN_EPOLL,    // Failed to obtain an epoll instance
  ERR_EPOLL_SERVER,    // Failed to add the server socket to epoll instance
  N_ERRORS
} TlcaServerErrors;

const char *
tlca_server_err_to_str (TlcaServerErrors err);
