#include "tlca-setupwindow.h"

struct _TlcaSetupWindow
{
  GtkApplicationWindow parent_instance;
};

G_DEFINE_TYPE (TlcaSetupWindow, tlca_setup_window, GTK_TYPE_APPLICATION_WINDOW);

static void
tlca_setup_window_class_init (TlcaSetupWindowClass *klass)
{
}

static void
tlca_setup_window_init (TlcaSetupWindow *self)
{
  gtk_window_set_title (GTK_WINDOW (self), "Connect");
  gtk_window_set_default_size (GTK_WINDOW (self), 300, 200);
}

TlcaSetupWindow *
tlca_setup_window_new (GtkApplication *app)
{
  return g_object_new (TLCA_TYPE_SETUP_WINDOW,
      "application", app,
      NULL);
}
