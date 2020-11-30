#include <dlfcn.h>
#include "../initialize.h"

enum class Flags
{
    None = 0,
    Force_svg = 2
};


void* libgtk;
using gtk_init_fn = void(int* argc, char ***argv);
using gtk_free_fn = void(void*);
using unref_fn = void(void* obj);
using guess_content_type_fn = char*(const char* path, void*, void*, void*);
using get_icon_from_content_type_fn = void*(const char* content_type);
using get_default_icon_theme_fn = void*();
using get_icon_names_fn = void*(void*);
using theme_choose_icon_fn = void*(void* theme, void* icon_names, int size, Flags flags);
using icon_info_get_filename_fn = char*(void* icon_info);
gtk_init_fn* gtk_init;
unref_fn* unref;
guess_content_type_fn* guess_content_type;
get_icon_from_content_type_fn* get_icon_from_content_type;
gtk_free_fn* gtk_free;
get_default_icon_theme_fn* get_default_icon_theme;
get_icon_names_fn* get_icon_names;
theme_choose_icon_fn* theme_choose_icon;
icon_info_get_filename_fn* icon_info_get_filename;

void initialize() {
    libgtk = dlopen("libgtk-3.so.0", RTLD_NOW | RTLD_LOCAL);
    gtk_init = reinterpret_cast<gtk_init_fn*>(dlsym(libgtk, "gtk_init"));
    unref = reinterpret_cast<unref_fn*>(dlsym(libgtk, "g_object_unref"));
    gtk_free = reinterpret_cast<gtk_free_fn*>(dlsym(libgtk, "g_free"));
    guess_content_type = reinterpret_cast<guess_content_type_fn*>(dlsym(libgtk, "g_content_type_guess"));
    get_icon_from_content_type = reinterpret_cast<get_icon_from_content_type_fn*>(dlsym(libgtk, "g_content_type_get_icon"));
    get_default_icon_theme = reinterpret_cast<get_default_icon_theme_fn*>(dlsym(libgtk, "gtk_icon_theme_get_default"));
    get_icon_names = reinterpret_cast<get_icon_names_fn*>(dlsym(libgtk, "g_themed_icon_get_names"));
    theme_choose_icon = reinterpret_cast<theme_choose_icon_fn*>(dlsym(libgtk, "gtk_icon_theme_choose_icon"));
    icon_info_get_filename = reinterpret_cast<icon_info_get_filename_fn*>(dlsym(libgtk, "gtk_icon_info_get_filename"));
    
    
    
    
    int arg = 0;
    gtk_init(&arg, nullptr);
    
    auto type = guess_content_type(".js", nullptr, nullptr, nullptr);
    auto icon = get_icon_from_content_type(type);
    gtk_free(type);
    auto theme = get_default_icon_theme();
    auto names = get_icon_names(icon);
    auto icon_info = theme_choose_icon(theme, names, 16, Flags::Force_svg);
    auto icon_path = icon_info_get_filename(icon_info);
    unref(icon);
}