#include <napi.h>
#include <string>
using namespace Napi;
using namespace std;

struct Service_item {
    wstring name;
};

class Services_worker : public AsyncProgressWorker<Service_item>
{
public:
    Services_worker(const Napi::Env& env, int handle, const Function& callback)
    : AsyncProgressWorker(env)
    , callback(Persistent(callback))
    , deferred(Promise::Deferred::New(Env())) 
    , handle(handle)
   {} 
    ~Services_worker() {
        callback.Reset();
    }

    // static void FileProgressCallback(int64_t current, int64_t total, void* p) {
    //     ExecutionProgressData *epd = (ExecutionProgressData *)p;
    //     auto data = CopyFileProgress { epd->idx, current, total };
    //     epd->progress.Send(&data, 1);
    // }

    void Execute(const ExecutionProgress& progress) { 
        ExecutionProgressData epd{progress, 0};
        // for (auto &item : items)
        // {
        //     if (is_cancelled(cancellation)) {
        //         copy_result = make_cancelled_result();
        //         return;
        //     }
        //     auto source = g_file_new_for_path(item.source.c_str());
        //     auto dest = g_file_new_for_path(item.target.c_str());
        //     GError* error{nullptr};
        //     bool success = move
        //         ? g_file_move(source, dest, (overwrite ? G_FILE_COPY_OVERWRITE : G_FILE_COPY_NONE), nullptr, FileProgressCallback, (void*)&epd, &error)
        //         : g_file_copy(source, dest, (overwrite ? G_FILE_COPY_OVERWRITE : G_FILE_COPY_NONE), nullptr, FileProgressCallback, (void*)&epd, &error);
        //     if (error && error->code == 1 && g_file_query_exists(source, nullptr)) {
        //         g_error_free(error);
        //         error = nullptr;
        //         auto path = g_file_get_parent(dest);
        //         g_file_make_directory_with_parents(path, nullptr, nullptr);
        //         g_object_unref(path);
        //         success = move
        //                       ? g_file_move(source, dest, (GFileCopyFlags)(0), nullptr, FileProgressCallback, (void *)&epd, &error)
        //                       : g_file_copy(source, dest, (GFileCopyFlags)(0), nullptr, FileProgressCallback, (void *)&epd, &error);
        //     }
        //     if (!success) {
        //         if (error) {
        //             copy_result = make_result(errno, error);
        //             g_error_free(error);
        //             g_object_unref(source);
        //             g_object_unref(dest);
        //             return;
        //         }
        //     }
        //     epd.idx++;
        //     g_object_unref(source);
        //     g_object_unref(dest);
        // }
    }

    void OnProgress(const Service_item* data, size_t count) {
    //     vector<napi_value> args;
    //     args.push_back(Number::New(callback.Env(), data->idx));
    //     args.push_back(Number::New(callback.Env(), data->current));
    //     args.push_back(Number::New(callback.Env(), data->total));
    //     callback.Call(args);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    FunctionReference callback;
    Promise::Deferred deferred;
    int handle;
};

void Services_worker::OnOK() {
    HandleScope scope(Env());

    // auto native_err_code = get<0>(copy_result);
    // auto err_code = get<1>(copy_result);
    // auto err_msg = get<2>(copy_result);
    // if (native_err_code == 0) 
    //     deferred.Resolve(Env().Null());
    // else {
    //     Napi::Object errObj = Env().Global()
    //         .Get("Error").As<Napi::Function>()
    //         .New({ nodestring::New(Env(), err_msg) });        
    //     errObj.Set("error", nodestring::New(Env(), err_code));
    //     errObj.Set("nativeError", Number::New(Env(), (int)native_err_code));
    //     deferred.Reject(errObj);
    // }
    deferred.Resolve(Env().Null());
}

Value StartObservingWindowServices(const CallbackInfo& info) {
    auto handle = info[0].As<Number>().Int32Value();
    auto cb = info[1].As<Function>();

    auto worker = new Services_worker(info.Env(), handle, cb);
    worker->Queue();
    return worker->GetPromise();
}

Value StopObservingWindowServices(const CallbackInfo& info) {
    auto handle = info[0].As<Number>().Int32Value();
    return info.Env().Undefined();    
}