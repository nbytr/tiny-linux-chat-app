#pragma once

#include <gio/gio.h>

int
util_gsockio_read_all (GSocket *socket, size_t amount, char *buffer);

int
util_gsockio_send_all (GSocket *socket, size_t amount, void *buffer);
