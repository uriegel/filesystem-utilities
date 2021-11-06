#include <napi.h>
#include <gtk/gtk.h>
#include "copy_file_worker.h"
#include "../FileResult.h"
using namespace Napi;
using namespace std;

class Trash_worker : public AsyncWorker {
public:
    Trash_worker(const Napi::Env& env, const string& path)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
    , path(path)
   {} 
    ~Trash_worker() { }

    void Execute() { 
        auto path_file = g_file_new_for_path(path.c_str());
        GError* error{nullptr};
        if (!g_file_trash(path_file, nullptr, &error)) {
            if (!error)
                result = FileResult::Unknown;
            else {
                error_description = error->message;
                switch (error->code) {
                    case 1:
                        result = FileResult::FileNotFound;
                        break;
                    case 2:
                        result = FileResult::FileExists;
                        break;
                    case 14:
                        result = FileResult::AccessDenied;
                        break;
                    default:
                        result = FileResult::Unknown;
                        break;
                }
            }
        }
        if (error)
            g_error_free(error);
        g_object_unref(path_file);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    FileResult result;
    string error_description;
    string path;
};

void Trash_worker::OnOK() {
    HandleScope scope(Env());
    if (result == FileResult::Success)
        deferred.Resolve(Env().Null());
    else {
        auto obj = Object::New(Env());
        obj.Set("fileResult", Number::New(Env(), (int)result));
        obj.Set("description", String::New(Env(), error_description));
        deferred.Reject(obj);
    }
}

Value Trash(const CallbackInfo& info) {
    auto path = (string)info[0].As<String>();
    auto worker = new Trash_worker(info.Env(), path);
    worker->Queue();
    return worker->GetPromise();
}
