#include <napi.h>
#include <vector>
#include "../wstring.h"
#include "create_directory_worker.h"
#include "utils.h"
#include "shell.h"
using namespace Napi;
using namespace std;

class Create_directory_worker : public AsyncWorker {
public:
    Create_directory_worker(const Napi::Env& env, const wstring& path)
    : AsyncWorker(env)
    , path(path)
    , deferred(Promise::Deferred::New(Env())) {}
    ~Create_directory_worker() {}

    void Execute () { copy_result = create_directory(path); }

    void OnOK() {
        HandleScope scope(Env());
        auto native_err_code = get<0>(copy_result);
        auto err_code = get<1>(copy_result);
        auto err_msg = get<2>(copy_result);
        if (native_err_code == 0) 
            deferred.Resolve(Env().Null());
        else {
            Napi::Object errObj = Env().Global()
                .Get("Error").As<Napi::Function>()
                .New({ WString::New(Env(), err_msg) });        
            errObj.Set("error", WString::New(Env(), err_code));
            errObj.Set("nativeError", Number::New(Env(), (int)native_err_code));
            deferred.Reject(errObj);
        }
    }

    Promise Promise() { return deferred.Promise(); }

private:
    wstring path;
    tuple<int, wstring, wstring> copy_result;
    Promise::Deferred deferred;
};

Value CreateFolder(const CallbackInfo& info) {
    auto path = info[0].As<WString>().WValue();
    auto worker = new Create_directory_worker(info.Env(), path);
    worker->Queue();
    return worker->Promise();
}
