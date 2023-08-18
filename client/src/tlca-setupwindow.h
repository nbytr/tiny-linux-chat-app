#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TLCA_TYPE_SETUP_WINDOW tlca_setup_window_get_type ()
G_DECLARE_FINAL_TYPE (TlcaSetupWindow, tlca_setup_window, TLCA, SETUP_WINDOW, GtkApplicationWindow)

TlcaSetupWindow *tlca_setup_window_new (GtkApplication *app);

G_END_DECLS
