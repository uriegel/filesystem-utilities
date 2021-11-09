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
    Copy_worker(const Napi::Env& env, const wstring& source_path, const wstring& target_path, const vector<wstring>& files, bool move)
    : AsyncWorker(env)
    , source_path(source_path)
    , target_path(target_path)
    , files(files)
    , move(move)

    , deferred(Promise::Deferred::New(Env())) {}
    ~Copy_worker() {}

    void Execute () { copy_files(source_path, target_path, files, vector<wstring>(), move, error, error_code); }

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
    wstring source_path;
    wstring target_path;
    vector<wstring> files;
    bool move;
    string error;
    int error_code;
    Promise::Deferred deferred;
};

Value Copy(const CallbackInfo& info) {
    auto source_path = info[0].As<WString>().WValue();
    auto target_path = info[1].As<WString>().WValue();
    auto pathes = info[2].As<Array>();
    auto count = pathes.Length();
    vector<wstring> files;
    for (auto i = 0u; i < count; i++) 
        files.push_back(pathes.Get(i).As<WString>().WValue());
    auto move = false;
    if (info.Length() > 3)
        move = info[3].As<Boolean>();
    
    auto worker = new Copy_worker(info.Env(), source_path, target_path, files, move);
    worker->Queue();
    return worker->Promise();
}
