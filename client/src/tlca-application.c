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
}

static void
tlca_application_init (TlcaApplication *self)
{
}

TlcaApplication *
tlca_application_new (void)
{
  return g_object_new (TLCA_TYPE_APPLICATION,
      "application-id", "org.tlca.client",
      NULL);
}
