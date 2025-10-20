#include <napi.h>
#include <vector>
#include "../wstring.h"
#include "rename_worker.h"
#include "utils.h"
#include "shell.h"
using namespace Napi;
using namespace std;

class Rename_worker : public AsyncWorker {
public:
    Rename_worker(const Napi::Env& env, const wstring& path, const wstring& name, const wstring& new_name)
    : AsyncWorker(env)
    , path(path)
    , name(name)
    , new_name(new_name)
    , deferred(Promise::Deferred::New(Env())) {}
    ~Rename_worker() {}

    void Execute () { copy_result = rename(path + L"\\"s + name, path + L"\\"s + new_name); }

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
    wstring name;
    wstring new_name;
    tuple<int, wstring, wstring> copy_result;
    Promise::Deferred deferred;
};

Value Rename(const CallbackInfo& info) {
    auto path = info[0].As<WString>().WValue();
    auto name = info[1].As<WString>().WValue();
    auto new_name = info[2].As<WString>().WValue();
    auto worker = new Rename_worker(info.Env(), path, name, new_name);
    worker->Queue();
    return worker->Promise();
}
