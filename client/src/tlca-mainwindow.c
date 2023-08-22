#include "tlca-mainwindow.h"

struct _TlcaMainWindow
{
  GtkApplicationWindow parent_instance;

  const char *nickname;
  GSocketConnection *conn;

  GtkBuilder *builder;
  GtkGrid *root_grid;
  GtkLabel *status_label;
  GtkTextView *msg_textview;
  GtkEntry *msg_entry;
  GtkButton *send_button;
};

G_DEFINE_TYPE (TlcaMainWindow, tlca_main_window, GTK_TYPE_APPLICATION_WINDOW)

static void
read_data (GSocket *socket, GIOCondition condition, gpointer data)
{
  gchar buffer[128];

  g_socket_receive (socket, buffer, 128, NULL, NULL);

  g_print ("Recieved from server: %s\n", buffer);
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
tlca_main_window_init (TlcaMainWindow *self)
{
  self->builder = gtk_builder_new_from_resource ("/org/tlca/client/ui/main-window.ui");

  self->root_grid = GTK_GRID (gtk_builder_get_object (self->builder, "root_grid"));

  self->status_label = GTK_LABEL (gtk_builder_get_object (self->builder, "status_label"));
  self->msg_textview = GTK_TEXT_VIEW (gtk_builder_get_object (self->builder, "msg_textview"));

  self->msg_entry = GTK_ENTRY (gtk_builder_get_object (self->builder, "msg_entry"));
  self->send_button = GTK_BUTTON (gtk_builder_get_object (self->builder, "send_button"));

  gtk_text_view_set_cursor_visible (self->msg_textview, FALSE);
  gtk_window_set_child (GTK_WINDOW (self), GTK_WIDGET (self->root_grid));
  gtk_window_set_title (GTK_WINDOW (self), "Chat");
}

TlcaMainWindow *
tlca_main_window_new (GtkApplication *app, const char *nickname, GSocketConnection *conn)
{
  return g_object_new (TLCA_TYPE_MAIN_WINDOW, "application", app,
      "nickname", nickname, "connsocket", conn, NULL);
}
