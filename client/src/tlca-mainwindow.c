#include "tlca-mainwindow.h"

struct _TlcaMainWindow
{
  GtkApplicationWindow parent_instance;

  GSocketConnection *conn;
};

G_DEFINE_TYPE (TlcaMainWindow, tlca_main_window, GTK_TYPE_APPLICATION_WINDOW)

static void
tlca_main_window_class_init (TlcaMainWindowClass *klass)
{
}

static void
tlca_main_window_init (TlcaMainWindow *self)
{
}

TlcaMainWindow *
tlca_main_window_new (GtkApplication *app, GSocketConnection *conn)
{
  return g_object_new (TLCA_TYPE_MAIN_WINDOW, "application", app,
      "socketconn", conn, NULL);
}
