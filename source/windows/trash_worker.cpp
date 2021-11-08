#include <napi.h>
#include <vector>
#include "shell.h"
#include "../wstring.h"
#include "../trash_worker.h"
#include "../FileResult.h"
using namespace Napi;
using namespace std;

class Trash_worker : public AsyncWorker {
public:
    Trash_worker(const Napi::Env& env, const vector<wstring>& files)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
    , result(FileResult::Success)
    , files(files)
   {} 
    ~Trash_worker() { }

    void Execute() { 
        delete_files(files, error_description, result);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    FileResult result;
    wstring error_description;
    vector<wstring> files;
};

void Trash_worker::OnOK() {
    HandleScope scope(Env());
    if (result == FileResult::Success)
        deferred.Resolve(Env().Null());
    else {
        auto obj = Object::New(Env());
        obj.Set("fileResult", Number::New(Env(), (int)result));
        obj.Set("description", WString::New(Env(), error_description));
        deferred.Reject(obj);
    }
}

Value Trash(const CallbackInfo& info) {
    auto pathes = info[0].As<Array>();
    auto count = pathes.Length();
    vector<wstring> files;
    for (auto i = 0u; i < count; i++) 
        files.push_back(pathes.Get(i).As<WString>().WValue());
    
    auto worker = new Trash_worker(info.Env(), files);
    worker->Queue();
    return worker->GetPromise();
}
