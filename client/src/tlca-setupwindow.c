#include "tlca-setupwindow.h"

struct _TlcaSetupWindow
{
  GtkApplicationWindow parent_instance;

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
tlca_setup_window_init (TlcaSetupWindow *self)
{
  self->builder = gtk_builder_new_from_resource ("/org/tlca/client/ui/setup-window.ui");

  self->root_grid = GTK_GRID (gtk_builder_get_object (self->builder, "root_grid"));
  
  self->connstatus_label = GTK_LABEL (gtk_builder_get_object (self->builder, "connstatus_label"));

  self->server_addr_entry = GTK_ENTRY (gtk_builder_get_object (self->builder, "server_addr_entry"));
  self->nickname_entry = GTK_ENTRY (gtk_builder_get_object (self->builder, "nickname_entry"));

  self->connect_button = GTK_BUTTON (gtk_builder_get_object (self->builder, "connect_button"));

  gtk_window_set_title (GTK_WINDOW (self), "Connect");
  gtk_window_set_child (GTK_WINDOW (self), GTK_WIDGET (self->root_grid));
}

TlcaSetupWindow *
tlca_setup_window_new (GtkApplication *app)
{
  return g_object_new (TLCA_TYPE_SETUP_WINDOW,
      "application", app,
      NULL);
}
