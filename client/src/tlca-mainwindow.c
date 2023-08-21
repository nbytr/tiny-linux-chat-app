#include "tlca-mainwindow.h"

struct _TlcaMainWindow
{
  GtkApplicationWindow parent_instance;

  const char *nickname;
  GSocketConnection *conn;

  GtkLabel *nickname_label;
};

G_DEFINE_TYPE (TlcaMainWindow, tlca_main_window, GTK_TYPE_APPLICATION_WINDOW)

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

  gtk_label_set_text (self->nickname_label, tmp->str);

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
  gtk_window_set_title (GTK_WINDOW (self), "Chat");

  self->nickname_label = GTK_LABEL (gtk_label_new (""));
  gtk_window_set_child (GTK_WINDOW (self), GTK_WIDGET (self->nickname_label));
}

TlcaMainWindow *
tlca_main_window_new (GtkApplication *app, const char *nickname, GSocketConnection *conn)
{
  return g_object_new (TLCA_TYPE_MAIN_WINDOW, "application", app,
      "nickname", nickname, "connsocket", conn, NULL);
}
