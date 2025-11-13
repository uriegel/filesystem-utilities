#include <napi.h>
#include <gtk/gtk.h>
#include <thread>
#include <unistd.h>     // for mkstemp, close, unlink, etc.
#include <sys/stat.h>   // for chmod
#include <iostream>   
#include "start_drag.h"
#include "drag_helper_exec.h"
using namespace Napi;
using namespace std;

static string write_temp_executable() {
    char tmpl[] = "/tmp/drag_helper_exec-XXXXXX";
    int fd = mkstemp(tmpl);
    if (fd == -1) 
        return "";

    // Write binary contents
    if (write(fd, drag_helper, drag_helper_len) != (ssize_t)drag_helper_len) {
        close(fd);
        unlink(tmpl);
        return "";
    }

    close(fd);
    chmod(tmpl, 0755);
    return string(tmpl);
}

Value StartDrag(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[0].IsArray())
        TypeError::New(env, "Expected array of file paths").ThrowAsJavaScriptException();

    Array arr = info[0].As<Array>();
    vector<string> files;
    for (uint32_t i = 0; i < arr.Length(); i++) {
        Value v = arr[i];
        if (v.IsString()) 
            files.push_back(v.As<String>().Utf8Value());
    }

    if (files.empty())
        return env.Undefined();

    for (auto file: files)
        cout << "Included file: " << file << endl;

    auto helperPath = write_temp_executable();
    if (helperPath.empty()) {
        Error::New(env, "Failed to create temp helper executable").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    vector<char*> argv;
    argv.push_back((char*)helperPath.c_str());
    for (auto &f : files)
        argv.push_back((char*)f.c_str());
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid == 0) {
        execv(helperPath.c_str(), argv.data());
        _exit(1);
    }

    // Schedule async cleanup (after process starts)
    if (pid > 0) {
        // Detach cleanup thread
        thread([helperPath]() {
            // Give GTK drag time to start and finish
            sleep(10);
            unlink(helperPath.c_str());
        }).detach();
    }

    return env.Undefined();
}
