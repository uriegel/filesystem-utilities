#include <napi.h>
#include <gtk/gtk.h>
#include "copy_worker.h"
#include "../FileResult.h"
using namespace Napi;
using namespace std;

struct CopyFileProgress {
    int64_t current;
    int64_t total;
};

class Copy_file_worker : public AsyncProgressWorker<CopyFileProgress> {
public:
    Copy_file_worker(const Napi::Env& env, const Function& callback, const string& source_file, const string& target_file, bool overwrite, bool move)
    : AsyncProgressWorker(env)
    , callback(Persistent(callback))
    , deferred(Promise::Deferred::New(Env())) 
    , result(FileResult::Success)
    , source_file(source_file)
    , target_file(target_file)
    , overwrite(overwrite)
    , move(move)
   {} 
    ~Copy_file_worker() {
        callback.Reset();
    }

    static void FileProgressCallback(int64_t current, int64_t total, void* p) {
        auto progress = (AsyncProgressWorker<CopyFileProgress>::ExecutionProgress*)p;
        auto data = CopyFileProgress { current, total };
        progress->Send(&data, sizeof(data));
    }

    void Execute(const ExecutionProgress& progress) { 
        auto source = g_file_new_for_path(source_file.c_str());
        auto dest = g_file_new_for_path(target_file.c_str());
        GError* error{nullptr};
        bool success = move
            ? g_file_move(source, dest, (GFileCopyFlags)(overwrite ? 1 : 0), nullptr, FileProgressCallback, (void*)&progress, &error)
            : g_file_copy(source, dest, (GFileCopyFlags)(overwrite ? 1 : 0), nullptr, FileProgressCallback, (void*)&progress, &error);
        if (error && error->code == 1 && g_file_query_exists(source, nullptr)) {
            g_error_free(error);
            error = nullptr;
            auto path = g_file_get_parent(dest);
            g_file_make_directory_with_parents(path, nullptr, nullptr);
            g_object_unref(path);
            success = move
                ? g_file_move(source, dest, (GFileCopyFlags)(0), nullptr, FileProgressCallback, (void*)&progress, &error)
                : g_file_copy(source, dest, (GFileCopyFlags)(0), nullptr, FileProgressCallback, (void*)&progress, &error); 
        }
        if (!success) {
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
        g_object_unref(source);
        g_object_unref(dest);
    }

    void OnProgress(const CopyFileProgress* data, size_t count) {
        vector<napi_value> args;
        args.push_back(Number::New(callback.Env(), data->current));
        args.push_back(Number::New(callback.Env(), data->total));
        callback.Call(args);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    FunctionReference callback;
    Promise::Deferred deferred;
    FileResult result;
    string error_description;
    string source_file;
    string target_file;
    bool overwrite;
    bool move;
};

void Copy_file_worker::OnOK() {
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

Value Copy(const CallbackInfo& info) {
    auto source_file = (string)info[0].As<String>();
    auto target_file = (string)info[1].As<String>();
    auto cb = info[2].As<Function>();
    auto move = false;
    if (info.Length() > 3)
        move = info[3].As<Boolean>();
    auto overwrite = info.Length() > 4 ? info[4].As<Boolean>() : false;
    auto worker = new Copy_file_worker(info.Env(), cb, source_file, target_file, overwrite, move);
    worker->Queue();
    return worker->GetPromise();
}

