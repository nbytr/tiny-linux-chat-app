#include "errordefs.h"

const char *
tlca_server_err_to_str (TlcaServerErrors err)
{
  switch (err)
  {
    case ERR_OBTAIN_ADDR:
      return "Error obtaining addresses to bind too";
    case ERR_NO_VALID_ADDR:
      return "Unable to bind to any addresses";
    case ERR_LISTENING:
      return "Unable to listen for connections";
    case ERR_OBTAIN_EPOLL:
      return "Unable to obtain an epoll instance";
    case ERR_EPOLL_SERVER:
      return "Unable to add the server to the epoll instance";
    default:
      return "Not a valid error!";
  }
}
