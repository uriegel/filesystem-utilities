#include <gtk/gtk.h>

GtkIconTheme* default_theme;

void init() {
    return;
    int argc{0};
    gtk_init(&argc, (char***)nullptr);
    default_theme = gtk_icon_theme_get_default();
}


