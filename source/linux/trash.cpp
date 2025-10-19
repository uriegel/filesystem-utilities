#include <napi.h>
#include <gtk/gtk.h>
#include "../trash_worker.h"
#include "../error.h"
using namespace Napi;
using namespace std;

tuple<int, string, string> delete_files(const vector<string>& files) {
    for (auto &file: files)
    {
        auto path_file = g_file_new_for_path(file.c_str());
        GError* error{nullptr};
        if (!g_file_trash(path_file, nullptr, &error)) {
            if (error) {
                auto result = make_result(errno, error);
                g_error_free(error);
                g_object_unref(path_file);
                return result;
            }
        }
        g_object_unref(path_file);
    }
    tuple<int, string, string> result(0, "", "");
    return result;
}

