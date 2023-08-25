#include "tlca-application.h"

#include "tlca-setupwindow.h"
#include "tlca-mainwindow.h"

struct _TlcaApplication
{
  GtkApplication parent_instance;

  TlcaSetupWindow *setup_window;
  TlcaMainWindow *main_window;
};

G_DEFINE_TYPE (TlcaApplication, tlca_application, GTK_TYPE_APPLICATION);

static void
tlca_application_activate (GApplication *app)
{
  TlcaApplication *self = TLCA_APPLICATION (app);

  self->setup_window = tlca_setup_window_new (GTK_APPLICATION (app));
  gtk_window_present (GTK_WINDOW (self->setup_window));
}

static void
tlca_application_class_init (TlcaApplicationClass *klass)
{
  G_APPLICATION_CLASS (klass)->activate = tlca_application_activate;

  GType signal_types[] = {G_TYPE_STRING, G_TYPE_SOCKET_CONNECTION};

  g_signal_newv ("connection_made",
      G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
      NULL,
      NULL,
      NULL,
      NULL,
      G_TYPE_NONE,
      2,
      signal_types);
}

static void
tlca_application_connection_made (TlcaApplication *self,
                                   GString *nickname, GSocketConnection *conn)
{
  gtk_window_destroy (GTK_WINDOW (self->setup_window));

  self->main_window = tlca_main_window_new (GTK_APPLICATION (self), nickname->str,
      conn);

  gtk_window_present (GTK_WINDOW (self->main_window));
}

static void
tlca_application_init (TlcaApplication *self)
{
  g_signal_connect (self, "connection_made", G_CALLBACK (tlca_application_connection_made),
      NULL);
}

TlcaApplication *
tlca_application_new (void)
{
  return g_object_new (TLCA_TYPE_APPLICATION,
      //"application-id", "org.tlca.client",
      NULL);
}
