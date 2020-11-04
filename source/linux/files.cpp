#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include "../files.h"
using namespace std;

vector<File_item> get_files(const string& directory) {
    vector<File_item> result;

    auto dp = opendir(directory.c_str());
    if (dp) {
        while (auto ep = readdir(dp)) {            
            if ((ep->d_type == DT_REG || ep->d_type == DT_DIR) && !(ep->d_name[0] == '.' && (ep->d_name[1] == 0 || ep->d_name[1] == '.'))) {
                struct stat fa;
                stat((directory + "/"s + ep->d_name).c_str(), &fa);
                result.emplace_back(ep->d_name, ep->d_type == DT_DIR, ep->d_name[0] == '.', fa.st_size, fa.st_mtime * 1000);
            }
        }
        closedir(dp);
    }

    return result;
}