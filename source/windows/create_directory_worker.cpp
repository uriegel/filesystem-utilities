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

    void Execute () { create_directory(path, error, error_code); }

    void OnOK() {
        HandleScope scope(Env());
        if (error.empty())
            deferred.Resolve(Env().Null());
        else {
            auto obj = Object::New(Env());
            obj.Set("description", String::New(Env(), error.c_str()));
            obj.Set("code", Number::New(Env(), static_cast<double>(error_code)));
            deferred.Reject(obj);
        }
    }

    Promise Promise() { return deferred.Promise(); }

private:
    wstring path;
    string error;
    int error_code;
    Promise::Deferred deferred;
};

Value CreateDirectory1(const CallbackInfo& info) {
    auto path = info[0].As<WString>().WValue();
    auto worker = new Create_directory_worker(info.Env(), path);
    worker->Queue();
    return worker->Promise();
}
