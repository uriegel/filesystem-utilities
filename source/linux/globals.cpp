#include <gtk/gtk.h>
#include <mutex>
using namespace std;

GtkIconTheme* default_theme;
mutex m;
auto is_initialized{false};

void check_init()
{
    lock_guard<mutex> lockGuard(m);
    if (!is_initialized) {
        gtk_disable_setlocale();
        int argc{0};
        gtk_init(&argc, (char***)nullptr);
        default_theme = gtk_icon_theme_get_default();
        is_initialized = true;
    }
}
