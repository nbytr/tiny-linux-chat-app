#include <arpa/inet.h>
#include <wchar.h>

#include "tlca-mainwindow.h"

struct _TlcaMainWindow
{
  GtkApplicationWindow parent_instance;

  const char *nickname;
  GSocketConnection *conn;
  GOutputStream *conn_output;
  GInputStream *conn_input;

  GtkBuilder *builder;
  GtkGrid *root_grid;
  GtkLabel *status_label;
  GtkTextView *msg_textview;
  GtkEntry *msg_entry;
  GtkButton *send_button;

  GtkTextTag *nickname_tag;
  GtkTextTag *seperator_tag;
};

G_DEFINE_TYPE (TlcaMainWindow, tlca_main_window, GTK_TYPE_APPLICATION_WINDOW)

static gboolean 
read_data (GSocket *socket, GIOCondition condition, gpointer data)
{
  TlcaMainWindow *self = TLCA_MAIN_WINDOW (data);

  gchar* buffer;
  uint32_t msg_length = 0;

  gsize bytes_read = -1;
  g_input_stream_read_all (self->conn_input, (char *)&msg_length, 2, &bytes_read, NULL, NULL);
  if (bytes_read != 2) {
    gtk_label_set_text (self->status_label, "Disconnected from server!");
    gtk_widget_set_sensitive (GTK_WIDGET (self->msg_entry), FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (self->send_button), FALSE);
    return FALSE;
  }

  msg_length = ntohs (msg_length);

  buffer = g_malloc0 (msg_length);

  bytes_read = -1;
  g_input_stream_read_all (self->conn_input, buffer, msg_length, &bytes_read, NULL, NULL);
  if (bytes_read != msg_length) {
    gtk_label_set_text (self->status_label, "Disconnected from server!");
    gtk_widget_set_sensitive (GTK_WIDGET (self->msg_entry), FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (self->send_button), FALSE);
    return FALSE;
  }

  GtkTextBuffer *textbuffer;
  textbuffer = gtk_text_view_get_buffer (self->msg_textview);

  // Insert the message at the end, and scroll to ensure its visible
  GtkTextIter enditer;
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  gtk_text_buffer_insert (textbuffer, &enditer, buffer, -1);
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  gtk_text_view_scroll_to_iter (self->msg_textview, &enditer, 0, FALSE, 0, 0);

  // Get the location of the nickname as a pair of iterators
  int linecount = gtk_text_buffer_get_line_count (textbuffer);
  GtkTextIter iter_start, iter_end;
  gtk_text_buffer_get_iter_at_line (textbuffer, &iter_start, linecount - 2);

  int offset = g_utf8_strchr (buffer, msg_length, ':') - buffer;
  gtk_text_buffer_get_iter_at_line_offset (textbuffer, &iter_end, linecount - 2, g_utf8_strlen (buffer, offset));

  // Set the nickname to bold so that it stands out
  gtk_text_buffer_apply_tag (textbuffer, self->nickname_tag, &iter_start, &iter_end);

  // Free the buffer used to hold the message
  g_free (buffer);

  return TRUE;
}

static void
send_message (GtkWidget *widget, gpointer user_data)
{
  TlcaMainWindow *self = TLCA_MAIN_WINDOW (user_data);

  GtkEntryBuffer *entry_buffer;
  entry_buffer = gtk_entry_get_buffer (self->msg_entry);

  if (gtk_entry_buffer_get_length (entry_buffer) < 1) {
    return;
  }

  GString *tmp;
  tmp = g_string_new (self->nickname);

  g_string_append (tmp, ": ");

  const char *msg =
    gtk_entry_buffer_get_text (entry_buffer);

  g_string_append (tmp, msg);
  g_string_append (tmp, "\n");

  uint16_t length = strlen (tmp->str);
  uint16_t length_n = htons (length);

  g_output_stream_write_all (self->conn_output, &length_n, 2, NULL, NULL, NULL);
  g_output_stream_write_all (self->conn_output, tmp->str, length, NULL, NULL, NULL);

  gtk_entry_buffer_delete_text (entry_buffer, 0, -1);
}

typedef enum
{
  PROP_NICKNAME = 1,
  PROP_CONNECTION_SOCKET,
  N_PROPERTIES
} TlcaMainWindowProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};

static void
tlca_main_window_constructed (GObject *object)
{
  TlcaMainWindow *self = TLCA_MAIN_WINDOW (object);

  GString *tmp = g_string_new ("Connected as: ");
  g_string_append (tmp, self->nickname);

  gtk_label_set_text (self->status_label, tmp->str);

  GSource *sock_source;
  sock_source = g_socket_create_source (g_socket_connection_get_socket (self->conn),
                                        G_IO_IN, NULL);
  g_source_set_callback (sock_source, G_SOURCE_FUNC (read_data), self, NULL);

  g_source_attach (sock_source, NULL);

  self->conn_output = g_io_stream_get_output_stream (G_IO_STREAM (self->conn));
  self->conn_input = g_io_stream_get_input_stream (G_IO_STREAM (self->conn));

  G_OBJECT_CLASS (tlca_main_window_parent_class)->constructed (object);
}

static void
tlca_main_window_set_property (GObject *object,
                               guint    property_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
  TlcaMainWindow *self = TLCA_MAIN_WINDOW (object);

  switch ((TlcaMainWindowProperty) property_id)
  {
    case PROP_NICKNAME:
      self->nickname = g_value_get_string (value);
      break;
    case PROP_CONNECTION_SOCKET:
      self->conn = G_SOCKET_CONNECTION (
          g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
tlca_main_window_get_property (GObject *object,
                               guint    property_id,
                               GValue *value,
                               GParamSpec *pspec)
{
  TlcaMainWindow *self = TLCA_MAIN_WINDOW (object);

  switch ((TlcaMainWindowProperty) property_id)
  {
    case PROP_NICKNAME:
      g_value_set_string (value, self->nickname);
      break;
    case PROP_CONNECTION_SOCKET:
      g_value_set_object (value, self->conn);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
tlca_main_window_class_init (TlcaMainWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = tlca_main_window_set_property;
  object_class->get_property = tlca_main_window_get_property;
  object_class->constructed = tlca_main_window_constructed;

  obj_properties[PROP_NICKNAME] =
    g_param_spec_string ("nickname", "Nickname", "The user's nickname",
        NULL, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  obj_properties[PROP_CONNECTION_SOCKET] =
    g_param_spec_object ("connsocket", "ConnectionSocket", "The connection socket to the server",
        G_TYPE_SOCKET_CONNECTION, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}

static void
msg_entry_text_entered (GtkEntryBuffer *buffer, guint position, gchar *chars,
                        guint n_chars, gpointer user_data)
{
  gtk_widget_set_sensitive (GTK_WIDGET (TLCA_MAIN_WINDOW (user_data)->send_button), true);
}
static void
msg_entry_text_deleted (GtkEntryBuffer *buffer, guint position,
                        guint n_chars, gpointer user_data)
{
  TlcaMainWindow *self = TLCA_MAIN_WINDOW (user_data);

  if (gtk_entry_buffer_get_length (buffer) < 1) {
    gtk_widget_set_sensitive (GTK_WIDGET (self->send_button), false);
  }
}

static void
tlca_main_window_init (TlcaMainWindow *self)
{
  self->builder = gtk_builder_new_from_resource ("/org/tlca/client/ui/main-window.ui");

  self->root_grid = GTK_GRID (gtk_builder_get_object (self->builder, "root_grid"));

  self->status_label = GTK_LABEL (gtk_builder_get_object (self->builder, "status_label"));
  self->msg_textview = GTK_TEXT_VIEW (gtk_builder_get_object (self->builder, "msg_textview"));

  self->nickname_tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer (self->msg_textview),
      "nickname_tag", "weight", 700, NULL);
  self->seperator_tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer (self->msg_textview),
      "seperator_tag", "line-height", 0.5, NULL);

  self->msg_entry = GTK_ENTRY (gtk_builder_get_object (self->builder, "msg_entry"));
  self->send_button = GTK_BUTTON (gtk_builder_get_object (self->builder, "send_button"));

  gtk_text_view_set_cursor_visible (self->msg_textview, FALSE);
  gtk_entry_set_max_length (self->msg_entry, 128);

  g_signal_connect (self->send_button, "clicked", G_CALLBACK (send_message), self);
  gtk_widget_set_sensitive (GTK_WIDGET (self->send_button), false);

  g_signal_connect (self->msg_entry, "activate", G_CALLBACK (send_message), self);
  g_signal_connect_after (gtk_entry_get_buffer (self->msg_entry), "inserted-text", G_CALLBACK (msg_entry_text_entered), self);
  g_signal_connect_after (gtk_entry_get_buffer (self->msg_entry), "deleted-text", G_CALLBACK (msg_entry_text_deleted), self);

  gtk_window_set_child (GTK_WINDOW (self), GTK_WIDGET (self->root_grid));
  gtk_window_set_title (GTK_WINDOW (self), "Chat");
}

TlcaMainWindow *
tlca_main_window_new (GtkApplication *app, const char *nickname, GSocketConnection *conn)
{
  return g_object_new (TLCA_TYPE_MAIN_WINDOW, "application", app,
      "nickname", nickname, "connsocket", conn, NULL);
}
