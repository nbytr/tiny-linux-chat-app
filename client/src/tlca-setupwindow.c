#include "tlca-setupwindow.h"

struct _TlcaSetupWindow
{
  GtkApplicationWindow parent_instance;

  GtkBuilder *builder;
  GtkLabel *test_label;
};

G_DEFINE_TYPE (TlcaSetupWindow, tlca_setup_window, GTK_TYPE_APPLICATION_WINDOW);

static void
tlca_setup_window_class_init (TlcaSetupWindowClass *klass)
{
}

static void
tlca_setup_window_init (TlcaSetupWindow *self)
{
  self->builder = gtk_builder_new_from_resource ("/org/tlca/client/ui/setup-window.ui");

  self->test_label = GTK_LABEL (gtk_builder_get_object (self->builder, "test_label"));
  
  gtk_window_set_title (GTK_WINDOW (self), "Connect");
  gtk_window_set_default_size (GTK_WINDOW (self), 300, 200);
  
  gtk_window_set_child (GTK_WINDOW (self), GTK_WIDGET (self->test_label));
}

TlcaSetupWindow *
tlca_setup_window_new (GtkApplication *app)
{
  return g_object_new (TLCA_TYPE_SETUP_WINDOW,
      "application", app,
      NULL);
}
