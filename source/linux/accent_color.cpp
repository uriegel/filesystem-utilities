#include <napi.h>
#include <gio/gio.h>
#include "accent_color.h"
#include "platform.h"
using namespace Napi;
using namespace std;

string get_accent_color() {
    auto settings = g_settings_new("org.gnome.desktop.interface");
    if (!settings)
        return "blue";
    auto keys = g_settings_list_keys(settings);
    bool key_exists = false;
    for (auto k = keys; *k != nullptr; ++k) {
        if (string(*k) == "accent-color") {
            key_exists = true;
            break;
        }
    }       
    g_strfreev(keys);
    if (!key_exists) 
        return "blue";

    auto accent = g_settings_get_string(settings, "accent-color");
    g_object_unref(settings);        
    if (accent) {
        return accent;
        g_free(accent);
    } else 
        return "blue";
}

Value GetAccentColor(const CallbackInfo& info) {
    return nodestring::New(info.Env(), get_accent_color());
}