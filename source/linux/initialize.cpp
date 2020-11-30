#include <dlfcn.h>
#include "../initialize.h"
#include "gtk.h"

using gtk_init_fn = void(int* argc, void *);
void* libgtk;

void initialize() {
    libgtk = dlopen("libgtk-3.so.0", RTLD_NOW | RTLD_LOCAL);
    unref = reinterpret_cast<unref_fn*>(dlsym(libgtk, "g_object_unref"));
    gtk_free = reinterpret_cast<gtk_free_fn*>(dlsym(libgtk, "g_free"));
    guess_content_type = reinterpret_cast<guess_content_type_fn*>(dlsym(libgtk, "g_content_type_guess"));
    get_icon_from_content_type = reinterpret_cast<get_icon_from_content_type_fn*>(dlsym(libgtk, "g_content_type_get_icon"));
    get_default_icon_theme = reinterpret_cast<get_default_icon_theme_fn*>(dlsym(libgtk, "gtk_icon_theme_get_default"));
    get_icon_names = reinterpret_cast<get_icon_names_fn*>(dlsym(libgtk, "g_themed_icon_get_names"));
    theme_choose_icon = reinterpret_cast<theme_choose_icon_fn*>(dlsym(libgtk, "gtk_icon_theme_choose_icon"));
    icon_info_get_filename = reinterpret_cast<icon_info_get_filename_fn*>(dlsym(libgtk, "gtk_icon_info_get_filename"));
}

unref_fn* unref;
guess_content_type_fn* guess_content_type;
get_icon_from_content_type_fn* get_icon_from_content_type;
gtk_free_fn* gtk_free;
get_default_icon_theme_fn* get_default_icon_theme;
get_icon_names_fn* get_icon_names;
theme_choose_icon_fn* theme_choose_icon;
icon_info_get_filename_fn* icon_info_get_filename;
