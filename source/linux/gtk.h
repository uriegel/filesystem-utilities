#pragma once

enum class Choose_icon_flags
{
    None = 0,
    Force_svg = 2
};

using gtk_init_fn = void(int* argc, char ***argv);
using gtk_free_fn = void(void*);
using unref_fn = void(void* obj);
using guess_content_type_fn = char*(const char* path, void*, void*, void*);
using get_icon_from_content_type_fn = void*(const char* content_type);
using get_default_icon_theme_fn = void*();
using get_icon_names_fn = void*(void*);
using theme_choose_icon_fn = void*(void* theme, void* icon_names, int size, Choose_icon_flags flags);
using icon_info_get_filename_fn = char*(void* icon_info);
extern gtk_init_fn* gtk_init;
extern unref_fn* unref;
extern guess_content_type_fn* guess_content_type;
extern get_icon_from_content_type_fn* get_icon_from_content_type;
extern gtk_free_fn* gtk_free;
extern get_default_icon_theme_fn* get_default_icon_theme;
extern get_icon_names_fn* get_icon_names;
extern theme_choose_icon_fn* theme_choose_icon;
extern icon_info_get_filename_fn* icon_info_get_filename;

