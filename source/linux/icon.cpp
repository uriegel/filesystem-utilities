#include <fstream>
#include <napi.h>
#include "../get_icon_worker.h"
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <iostream>
#include <thread>
#include <chrono>
using namespace Napi;
using namespace std;

GtkIconTheme* default_theme{nullptr};

void checkInitializeIcons() {
    if (!default_theme) {
#ifdef DEBUG
        int argc{0};
        gtk_init(&argc, (char***)nullptr);
#endif
        default_theme = gtk_icon_theme_get_default();
    }
}

vector<char> get_icon(const string& extension) {
    auto type = g_content_type_guess(extension.c_str(), nullptr, 0, nullptr);
    auto icon = g_content_type_get_icon(type);
    if (type)
        g_free(type);
    auto names = g_themed_icon_get_names((GThemedIcon*)icon);
    auto icon_info = gtk_icon_theme_choose_icon(default_theme, (const gchar**)names, 16, (GtkIconLookupFlags)0);
    if (icon_info == nullptr)
        cout << "icon_info is null" << endl;
    if (icon)
        g_object_unref(icon);
    auto filename = gtk_icon_info_get_filename(icon_info);
    auto filename_char = (char *)filename;
    if (icon_info == nullptr && filename_char != nullptr)
        cout << "icon file" << " - " << filename_char << endl;
    if (filename_char == nullptr)
        cout << "icon file is null" << endl;

    if (filename_char == nullptr || strncmp(filename_char, "/org/gkt", 8) == 0) {
        cout << "try again" << endl;
        this_thread::sleep_for(chrono::milliseconds(200));
        type = g_content_type_guess(extension.c_str(), nullptr, 0, nullptr);
        icon = g_content_type_get_icon(type);
        if (type)
            g_free(type);
        names = g_themed_icon_get_names((GThemedIcon*)icon);
        icon_info = gtk_icon_theme_choose_icon(default_theme, (const gchar**)names, 16, (GtkIconLookupFlags)0);
        if (icon)
            g_object_unref(icon);
        filename = gtk_icon_info_get_filename(icon_info);
    }
    ifstream input(filename, ios::binary);
    vector<char> result(istreambuf_iterator<char>(input), {});
    // if (icon_info)
    //     g_object_unref(icon_info);
    return result;
}

vector<char> get_icon_from_name(const string& name) {
    auto icon = g_content_type_get_icon(name.c_str());
    auto names = g_themed_icon_get_names((GThemedIcon*)icon);
    auto icon_info = gtk_icon_theme_choose_icon(default_theme, (const gchar**)names, 16, (GtkIconLookupFlags)0);
    if (icon_info == nullptr)
        cout << "icon_info is null" << endl;
    if (icon)
        g_object_unref(icon);
    auto filename = gtk_icon_info_get_filename(icon_info);
    auto filename_char = (char *)filename;
    if (icon_info == nullptr && filename_char != nullptr)
        cout << "icon file" << " - " << filename_char << endl;
    if (filename_char == nullptr)
        cout << "icon file is null" << endl;

    if (filename_char == nullptr || strncmp(filename_char, "/org/gkt", 8) == 0) {
        cout << "try again" << endl;
        this_thread::sleep_for(chrono::milliseconds(200));
        icon = g_content_type_get_icon(name.c_str());
        names = g_themed_icon_get_names((GThemedIcon*)icon);
        icon_info = gtk_icon_theme_choose_icon(default_theme, (const gchar**)names, 16, (GtkIconLookupFlags)0);
        if (icon)
            g_object_unref(icon);
        filename = gtk_icon_info_get_filename(icon_info);
    }
    ifstream input(filename, ios::binary);
    vector<char> result(istreambuf_iterator<char>(input), {});
    // if (icon_info)
    //     g_object_unref(icon_info);
    return result;
}

