#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TLCA_TYPE_APPLICATION tlca_application_get_type ()
G_DECLARE_FINAL_TYPE (TlcaApplication, tlca_application, TLCA, APPLICATION, GtkApplication)

TlcaApplication *tlca_application_new (void);

G_END_DECLS
