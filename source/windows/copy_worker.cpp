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
    Copy_worker(const Napi::Env& env, const vector<wstring>& source_pathes, const vector<wstring>& target_pathes, bool overwrite, bool move)
    : AsyncWorker(env)
    , source_pathes(source_pathes)
    , target_pathes(target_pathes)
    , overwrite(overwrite)
    , move(move)
    , deferred(Promise::Deferred::New(Env())) {}

    ~Copy_worker() {}

    void Execute () { 
        copy_result = source_pathes.size() == 1 && move
            ? rename(source_pathes[0], target_pathes[0]) 
            : copy_files(source_pathes, target_pathes, overwrite, move); 
    }

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
    vector<wstring> source_pathes;
    vector<wstring> target_pathes;
    bool overwrite;
    bool move;
    tuple<int, wstring, wstring> copy_result;
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
    auto move = info.Length() > 2 ? info[2].As<Boolean>() : false;
    auto overwrite = info.Length() > 3 ? info[3].As<Boolean>() : false;

    auto worker = new Copy_worker(info.Env(), source_pathes, target_pathes, overwrite, move);
    worker->Queue();
    return worker->Promise();
}