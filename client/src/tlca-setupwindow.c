#include "tlca-application.h"

#include "tlca-setupwindow.h"

struct _TlcaSetupWindow
{
  GtkApplicationWindow parent_instance;

  GString *nickname;

  GtkBuilder *builder;

  GtkGrid *root_grid;

  GtkLabel *connstatus_label;

  GtkEntry *server_addr_entry;
  GtkEntry *nickname_entry;

  GtkButton *connect_button;
};

G_DEFINE_TYPE (TlcaSetupWindow, tlca_setup_window, GTK_TYPE_APPLICATION_WINDOW);

static void
tlca_setup_window_class_init (TlcaSetupWindowClass *klass)
{
}

static void
connection_attempt_done (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  TlcaSetupWindow *self = TLCA_SETUP_WINDOW (user_data);

  GSocketClient *client = G_SOCKET_CLIENT (source_object);

  GSocketConnection *conn =
    g_socket_client_connect_to_host_finish (client, res, NULL);

  if (conn == NULL) {
    gtk_label_set_text (self->connstatus_label, "Failed to connect!");
    return;
  }

  gtk_label_set_text (self->connstatus_label, "Connected successfully!");

  TlcaApplication *app = TLCA_APPLICATION (gtk_window_get_application (GTK_WINDOW (self)));

  // Make sure the nickname is null-terminated
  self->nickname->str[self->nickname->len] = '\0';

  g_signal_emit_by_name (app, "connection_made", self->nickname, conn);
}

static void
attempt_connection (GtkWidget *widget, gpointer user_data)
{
  TlcaSetupWindow *self = TLCA_SETUP_WINDOW (user_data);

  // Get the server address and nickname
  GtkEntryBuffer *server_addr_buffer = gtk_entry_get_buffer (self->server_addr_entry);
  GtkEntryBuffer *nickname_buffer = gtk_entry_get_buffer (self->nickname_entry);
  
  // Check neither of the buffers are empty
  if (gtk_entry_buffer_get_length (server_addr_buffer)
      * gtk_entry_buffer_get_length (nickname_buffer) <= 0) {
    gtk_label_set_text (self->connstatus_label, "All fields must be filled");
    return;
  }

  const char *server_addr = gtk_entry_buffer_get_text (server_addr_buffer);
  GString *nickname = g_string_new (gtk_entry_buffer_get_text (nickname_buffer));
  self->nickname = nickname;

  GSocketClient *client;
  client = g_socket_client_new ();

  g_socket_client_connect_to_host_async (client, server_addr, -1, NULL,
      connection_attempt_done, self);

  gtk_label_set_text (self->connstatus_label, "Attempting connection...");

  g_object_unref (client);
}

static void
tlca_setup_window_init (TlcaSetupWindow *self)
{
  self->builder = gtk_builder_new_from_resource ("/org/tlca/client/ui/setup-window.ui");

  self->root_grid = GTK_GRID (gtk_builder_get_object (self->builder, "root_grid"));
  
  self->connstatus_label = GTK_LABEL (gtk_builder_get_object (self->builder, "connstatus_label"));

  self->server_addr_entry = GTK_ENTRY (gtk_builder_get_object (self->builder, "server_addr_entry"));
  self->nickname_entry = GTK_ENTRY (gtk_builder_get_object (self->builder, "nickname_entry"));

  self->connect_button = GTK_BUTTON (gtk_builder_get_object (self->builder, "connect_button"));
  g_signal_connect (self->connect_button, "clicked", G_CALLBACK (attempt_connection), self);

  gtk_entry_set_max_length (self->nickname_entry, 32);

  gtk_window_set_title (GTK_WINDOW (self), "Connect");
  gtk_window_set_resizable (GTK_WINDOW (self), FALSE);
  gtk_window_set_child (GTK_WINDOW (self), GTK_WIDGET (self->root_grid));
}

TlcaSetupWindow *
tlca_setup_window_new (GtkApplication *app)
{
  return g_object_new (TLCA_TYPE_SETUP_WINDOW,
      "application", app,
      NULL);
}
