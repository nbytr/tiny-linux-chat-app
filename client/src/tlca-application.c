#include "tlca-application.h"
#include "tlca-setupwindow.h"

struct _TlcaApplication
{
  GtkApplication parent_instance;

  TlcaSetupWindow *setup_window;
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

  GType signal_types[] = {G_TYPE_SOCKET_CONNECTION};

  g_signal_newv ("connection_made",
      G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
      NULL,
      NULL,
      NULL,
      NULL,
      G_TYPE_NONE,
      1,
      signal_types);
}

static void
tlca_application_connection_made (TlcaApplication *app,
                                   gpointer user_data)
{
  g_print ("Application alerted that connection was made!\n");

  GSocketConnection *conn = G_SOCKET_CONNECTION (user_data);

  if (g_socket_connection_is_connected (conn) == TRUE) 
    g_print ("Connected!");
  else
   g_print ("Not connected!");
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
      "application-id", "org.tlca.client",
      NULL);
}
