#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    if (argc < 2) {
        std::cerr << "Usage: drag-helper <file1> <file2> ..." << std::endl;
        return 1;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_widget_realize(window);

    while (g_main_context_pending(nullptr))
        g_main_context_iteration(nullptr, false);

    GtkTargetEntry targets[] = {
        { (gchar*)"text/uri-list", 0, 0 },
        { (gchar*)"application/x-gnome-copied-files", 0, 1 }
    };

    gtk_drag_source_set(window, GDK_BUTTON1_MASK, targets, 2, GDK_ACTION_COPY);

    auto *paths = new std::vector<std::string>(argv + 1, argv + argc);

    g_signal_connect(window, "drag-data-get",
        G_CALLBACK(+[](GtkWidget*, GdkDragContext*, GtkSelectionData *sel,
                       guint info, guint, gpointer user_data) {
            auto *paths = static_cast<std::vector<std::string>*>(user_data);
            std::string uris;
            for (auto &p : *paths)
                uris += "file://" + p + "\n";

            if (info == 1) {
                std::string payload = "copy\n" + uris;
                gtk_selection_data_set(sel,
                    gdk_atom_intern_static_string("application/x-gnome-copied-files"),
                    8, (const guchar*)payload.c_str(), payload.size());
            } else {
                gtk_selection_data_set(sel,
                    gdk_atom_intern_static_string("text/uri-list"),
                    8, (const guchar*)uris.c_str(), uris.size());
            }
        }),
        paths
    );

    gtk_drag_begin_with_coordinates(
        window,
        gtk_target_list_new(targets, 2),
        GDK_ACTION_COPY,
        1,
        nullptr,
        -1, -1
    );

    gtk_main();
    return 0;
}
