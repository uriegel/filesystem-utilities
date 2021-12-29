#include <napi.h>
#include <vector>
#include "../wstring.h"
#include "copy_worker.h"
#include "utils.h"
#include "shell.h"
using namespace Napi;
using namespace std;

class Copy_worker : public AsyncWorker {
public:
    Copy_worker(const Napi::Env& env, const vector<wstring>& source_pathes, const vector<wstring>& target_pathes, bool move)
    : AsyncWorker(env)
    , source_pathes(source_pathes)
    , target_pathes(target_pathes)
    , move(move)
    , deferred(Promise::Deferred::New(Env())) {}

    ~Copy_worker() {}

    void Execute () { 
        if (source_pathes.size() == 1 && move)
            rename(source_pathes[0], target_pathes[0], error, error_code); 
        else
            copy_files(source_pathes, target_pathes, move, error, error_code); 
    }

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
    vector<wstring> source_pathes;
    vector<wstring> target_pathes;
    bool move;
    string error;
    int error_code;
    Promise::Deferred deferred;
};

Value Copy(const CallbackInfo& info) {
    bool is_array{true};
    napi_is_array(info.Env(), info[0], &is_array);
    vector<wstring> source_pathes;
    vector<wstring> target_pathes;
    if (is_array) {
        auto arr = info[0].As<Array>();
        auto count = arr.Length();
        for (auto i = 0u; i < count; i++) 
            source_pathes.push_back(arr.Get(i).As<WString>().WValue());

        arr = info[1].As<Array>();
        count = arr.Length();
        for (auto i = 0u; i < count; i++) 
            target_pathes.push_back(arr.Get(i).As<WString>().WValue());

    } else {
        auto source = info[0].As<WString>().WValue();
        source_pathes.push_back(source);
        auto target = info[1].As<WString>().WValue();
        target_pathes.push_back(target);
    }
    auto move = false;
    if (info.Length() > 2)
        move = info[2].As<Boolean>();

    auto worker = new Copy_worker(info.Env(), source_pathes, target_pathes, move);
    worker->Queue();
    return worker->Promise();
}