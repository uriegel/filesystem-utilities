#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
#include "../files.h"
#include "../error.h"
using namespace std;

tuple<int, string, string, vector<File_item>> get_files(const string& path, bool show_hidden) {
    vector<File_item> items;

    auto dp = opendir(path.c_str());
    if (dp) {
        while (auto ep = readdir(dp)) {            
            if ((ep->d_type == DT_REG || ep->d_type == DT_DIR) && !(ep->d_name[0] == '.' && (ep->d_name[1] == 0 || ep->d_name[1] == '.'))) {
                if (ep->d_name[0] == '.' && !show_hidden)
                    continue;
                struct stat fa;
                stat((path + "/"s + ep->d_name).c_str(), &fa);
                items.emplace_back(ep->d_name, ep->d_type == DT_DIR, ep->d_name[0] == '.', fa.st_size, fa.st_mtime * 1000);
            }
        }
        closedir(dp);
        tuple<int, string, string, vector<File_item>> result(0, "", "", move(items));
        return result;
    } else 
        return tuple_cat(make_result(errno), make_tuple(move(items)));
}