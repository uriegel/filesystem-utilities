#include <napi.h>
#include <vector>
#include "trash_worker.h"
#if WINDOWS
    #include "windows/platform.h"
    #include "windows/shell.h"
#elif LINUX
    #include "linux/platform.h"
    #include "trash.h"
#endif

using namespace Napi;
using namespace std;

class Trash_worker : public AsyncWorker {
public:
    Trash_worker(const Napi::Env& env, const vector<stdstring>& files)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
    , files(files)
   {} 
    ~Trash_worker() { }

    void Execute() { 
        files_result = delete_files(files);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    tuple<int, stdstring, stdstring> files_result;
    vector<stdstring> files;
};

void Trash_worker::OnOK() {
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

Value Trash(const CallbackInfo& info) {
    auto pathes = info[0].As<Array>();
    auto count = pathes.Length();
    vector<stdstring> files;
    for (auto i = 0u; i < count; i++) 
        files.push_back((stdstring)pathes.Get(i).As<nodestring>());
    
    auto worker = new Trash_worker(info.Env(), files);
    worker->Queue();
    return worker->GetPromise();
}
