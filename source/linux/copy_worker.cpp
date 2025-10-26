#include <napi.h>
#include <gtk/gtk.h>
#include "copy_worker.h"
#include "../error.h"
#include "../cancellation.h"
using namespace Napi;
using namespace std;

struct CopyFileProgress {
    int idx;
    int64_t current;
    int64_t total;
};

struct CopyItem {
    string source;
    string target;
};

struct ExecutionProgressData {
    const AsyncProgressWorker<CopyFileProgress>::ExecutionProgress &progress;
    int idx {0};
};

class Copy_file_worker : public AsyncProgressWorker<CopyFileProgress>
{
public:
    Copy_file_worker(const Napi::Env& env, const Function& callback, const vector<CopyItem>& items, bool overwrite, bool move, const string& cancellation)
    : AsyncProgressWorker(env)
    , callback(Persistent(callback))
    , deferred(Promise::Deferred::New(Env())) 
    , items(items)
    , overwrite(overwrite)
    , move(move)
    , cancellation(cancellation)
   {} 
    ~Copy_file_worker() {
        callback.Reset();
    }

    static void FileProgressCallback(int64_t current, int64_t total, void* p) {
        ExecutionProgressData *epd = (ExecutionProgressData *)p;
        auto data = CopyFileProgress { epd->idx, current, total };
        epd->progress.Send(&data, 1);
    }

    void Execute(const ExecutionProgress& progress) { 
        register_cancellable(cancellation);

        ExecutionProgressData epd{progress, 0};
        for (auto &item : items)
        {
            if (is_cancelled(cancellation)) {
                copy_result = make_cancelled_result();
                return;
            }
            auto source = g_file_new_for_path(item.source.c_str());
            auto dest = g_file_new_for_path(item.target.c_str());
            GError* error{nullptr};
            bool success = move
                ? g_file_move(source, dest, (overwrite ? G_FILE_COPY_OVERWRITE : G_FILE_COPY_NONE), nullptr, FileProgressCallback, (void*)&epd, &error)
                : g_file_copy(source, dest, (overwrite ? G_FILE_COPY_OVERWRITE : G_FILE_COPY_NONE), nullptr, FileProgressCallback, (void*)&epd, &error);
            if (error && error->code == 1 && g_file_query_exists(source, nullptr)) {
                g_error_free(error);
                error = nullptr;
                auto path = g_file_get_parent(dest);
                g_file_make_directory_with_parents(path, nullptr, nullptr);
                g_object_unref(path);
                success = move
                              ? g_file_move(source, dest, (GFileCopyFlags)(0), nullptr, FileProgressCallback, (void *)&epd, &error)
                              : g_file_copy(source, dest, (GFileCopyFlags)(0), nullptr, FileProgressCallback, (void *)&epd, &error);
            }
            if (!success) {
                if (error) {
                    copy_result = make_result(errno, error);
                    g_error_free(error);
                    g_object_unref(source);
                    g_object_unref(dest);
                    return;
                }
            }
            epd.idx++;
            g_object_unref(source);
            g_object_unref(dest);
        }
        unregister_cancellable(cancellation);
    }

    void OnProgress(const CopyFileProgress* data, size_t count) {
        vector<napi_value> args;
        args.push_back(Number::New(callback.Env(), data->idx));
        args.push_back(Number::New(callback.Env(), data->current));
        args.push_back(Number::New(callback.Env(), data->total));
        callback.Call(args);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    FunctionReference callback;
    Promise::Deferred deferred;
    tuple<int, string, string> copy_result;
    const vector<CopyItem> items;
    bool overwrite;
    bool move;
    string cancellation;
};

void Copy_file_worker::OnOK() {
    HandleScope scope(Env());

    auto native_err_code = get<0>(copy_result);
    auto err_code = get<1>(copy_result);
    auto err_msg = get<2>(copy_result);
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
    auto copy_items = info[0].As<Array>();
    auto count = copy_items.Length();
    vector<CopyItem> input;
    for (auto i = 0u; i < count; i++) {
        auto val = copy_items.Get(i);
        auto obj = val.As<Object>();
        CopyItem copy_item;
        copy_item.source = (string)obj.Get("source").As<nodestring>();
        copy_item.target = (string)obj.Get("target").As<nodestring>();
        input.push_back(copy_item);
    }

    auto cb = info[1].As<Function>();
    auto move = info.Length() > 2 ? info[2].As<Boolean>() : false;
    auto overwrite = info.Length() > 3 ? info[3].As<Boolean>() : false;
    auto cancellation = info.Length() > 4 ? (string)info[4].As<nodestring>() : "";
    auto worker = new Copy_file_worker(info.Env(), cb, input, overwrite, move, cancellation);
    worker->Queue();
    return worker->GetPromise();
}

