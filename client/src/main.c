#include "tlca-application.h"

int main (int argc, char **argv)
{
  TlcaApplication *app;

  app = tlca_application_new ();

  int status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return status;
}
