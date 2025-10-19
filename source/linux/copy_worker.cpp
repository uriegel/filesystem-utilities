#include <napi.h>
#include <gtk/gtk.h>
#include "copy_worker.h"
#include "../error.h"
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
        // TODO progress->Send(&data, sizeof(data));
        progress->Send(&data, 1);
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
            if (error) {
                files_result = make_result(errno, error);
                g_error_free(error);
                g_object_unref(source);
                g_object_unref(dest);
                return;
            }
        }
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
    tuple<int, string, string> files_result;
    string source_file;
    string target_file;
    bool overwrite;
    bool move;
};

void Copy_file_worker::OnOK() {
    HandleScope scope(Env());

    auto native_err_code = get<0>(files_result);
    auto err_code = get<1>(files_result);
    auto err_msg = get<2>(files_result);
    if (native_err_code == 0) 
        deferred.Resolve(Env().Null());
    else {
        Napi::Object errObj = Env().Global()
            .Get("Error").As<Napi::Function>()
            .New({ nodestring::New(Env(), err_msg) });        
        errObj.Set("error", nodestring::New(Env(), err_code));
        errObj.Set("nativeError", Number::New(Env(), (int)native_err_code));
        deferred.Reject(errObj);
    }
}

Value Copy(const CallbackInfo& info) {
    auto source_file = (string)info[0].As<String>();
    auto target_file = (string)info[1].As<String>();
    auto cb = info[2].As<Function>();
    auto move = info.Length() > 3 ? info[3].As<Boolean>() : false;
    auto overwrite = info.Length() > 4 ? info[4].As<Boolean>() : false;
    auto worker = new Copy_file_worker(info.Env(), cb, source_file, target_file, overwrite, move);
    worker->Queue();
    return worker->GetPromise();
}

