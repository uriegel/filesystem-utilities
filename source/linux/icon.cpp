#include <fstream>
#include <napi.h>
#include "../get_icon_worker.h"
#include <gio/gio.h>
#include <gtk/gtk.h>
#include "globals.h"
using namespace Napi;
using namespace std;

std::vector<char> get_icon(const std::string& extension) {
    check_init();
    auto type = g_content_type_guess(extension.c_str(), nullptr, 0, nullptr);
    auto icon = g_content_type_get_icon(type);
    auto names = g_themed_icon_get_names((GThemedIcon*)icon);
    auto handle = gtk_icon_theme_choose_icon(default_theme, (const gchar**)names, 16, (GtkIconLookupFlags)2);
    g_object_unref(icon);
    auto filename = gtk_icon_info_get_filename(handle);
    ifstream input(filename, ios::binary);
    vector<char> result(istreambuf_iterator<char>(input), {});
    return result;
}


