#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TLCA_TYPE_MAIN_WINDOW tlca_main_window_get_type ()
G_DECLARE_FINAL_TYPE (TlcaMainWindow, tlca_main_window, TLCA, MAIN_WINDOW, GtkApplicationWindow)

TlcaMainWindow *
tlca_main_window_new (GtkApplication *app, const char *nickname, GSocketConnection *conn);

G_END_DECLS
